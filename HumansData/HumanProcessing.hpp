#pragma once

#include "HumanTypes.h"
#include "immintrin.h"
#include "../WorldData/World.h"

class Human;

template <HumanType HType>
inline void processHumanTypeBatch(
    Human &human,
    HumanBase &h,
    World &world,
    RendererSFML &renderer,
    size_t begin,
    int threadID)
{
    constexpr size_t BATCH_SIZE = 16;

    __m256i posX =
        _mm256_loadu_si256(
            reinterpret_cast<const __m256i *>(&h.posX[begin]));

    __m256i posY =
        _mm256_loadu_si256(
            reinterpret_cast<const __m256i *>(&h.posY[begin]));

    __m256i targetX =
        _mm256_loadu_si256(
            reinterpret_cast<const __m256i *>(&h.targetX[begin]));

    __m256i targetY =
        _mm256_loadu_si256(
            reinterpret_cast<const __m256i *>(&h.targetY[begin]));

    const __m256i bias16 =
        _mm256_set1_epi16(
            static_cast<int>(static_cast<uint16_t>(0x8000)));

    const __m256i one16 =
        _mm256_set1_epi16(1);

    const __m256i invalidTarget16 =
        _mm256_set1_epi16(
            static_cast<int>(static_cast<uint16_t>(UINT16_MAX)));

    const __m256i worldWidth =
        _mm256_set1_epi16(
            static_cast<int>(
                static_cast<uint16_t>(Config::sizeX)));

    const __m256i worldHeight =
        _mm256_set1_epi16(
            static_cast<int>(
                static_cast<uint16_t>(Config::sizeY)));

    __m256i noTargetMask =
        _mm256_or_si256(
            _mm256_cmpeq_epi16(
                targetX,
                invalidTarget16),
            _mm256_cmpeq_epi16(
                targetY,
                invalidTarget16));

    __m256i posXBias =
        _mm256_xor_si256(posX, bias16);

    __m256i posYBias =
        _mm256_xor_si256(posY, bias16);

    __m256i tgtXBias =
        _mm256_xor_si256(targetX, bias16);

    __m256i tgtYBias =
        _mm256_xor_si256(targetY, bias16);

    __m256i gtX =
        _mm256_cmpgt_epi16(
            tgtXBias,
            posXBias);

    __m256i ltX =
        _mm256_cmpgt_epi16(
            posXBias,
            tgtXBias);

    __m256i dirX =
        _mm256_sub_epi16(
            _mm256_and_si256(gtX, one16),
            _mm256_and_si256(ltX, one16));

    __m256i gtY =
        _mm256_cmpgt_epi16(
            tgtYBias,
            posYBias);

    __m256i ltY =
        _mm256_cmpgt_epi16(
            posYBias,
            tgtYBias);

    __m256i dirY =
        _mm256_sub_epi16(
            _mm256_and_si256(gtY, one16),
            _mm256_and_si256(ltY, one16));

    static constexpr int16_t lookupX[8] =
        {
            1, 1, 0, -1, -1, -1, 0, 1};

    static constexpr int16_t lookupY[8] =
        {
            0, 1, 1, 1, 0, -1, -1, -1};

    alignas(32) int16_t fallbackDirX[16];
    alignas(32) int16_t fallbackDirY[16];

    for (size_t lane = 0; lane < BATCH_SIZE; ++lane)
    {
        size_t i = begin + lane;

        uint8_t idx =
            static_cast<uint8_t>(
                (i + h.points[i]) & 7u);

        fallbackDirX[lane] = lookupX[idx];
        fallbackDirY[lane] = lookupY[idx];
    }

    __m256i fallbackDirXVec =
        _mm256_load_si256(
            reinterpret_cast<const __m256i *>(
                fallbackDirX));

    __m256i fallbackDirYVec =
        _mm256_load_si256(
            reinterpret_cast<const __m256i *>(
                fallbackDirY));

    dirX =
        _mm256_blendv_epi8(
            dirX,
            fallbackDirXVec,
            noTargetMask);

    dirY =
        _mm256_blendv_epi8(
            dirY,
            fallbackDirYVec,
            noTargetMask);

    __m256i newX =
        _mm256_add_epi16(
            posX,
            dirX);

    __m256i newY =
        _mm256_add_epi16(
            posY,
            dirY);

    __m256i newXBias =
        _mm256_xor_si256(
            newX,
            bias16);

    __m256i newYBias =
        _mm256_xor_si256(
            newY,
            bias16);

    __m256i widthBias =
        _mm256_xor_si256(
            worldWidth,
            bias16);

    __m256i heightBias =
        _mm256_xor_si256(
            worldHeight,
            bias16);

    __m256i validX =
        _mm256_cmpgt_epi16(
            widthBias,
            newXBias);

    __m256i validY =
        _mm256_cmpgt_epi16(
            heightBias,
            newYBias);

    __m256i validMask =
        _mm256_and_si256(
            validX,
            validY);

    __m256i finalX =
        _mm256_blendv_epi8(
            posX,
            newX,
            validMask);

    __m256i finalY =
        _mm256_blendv_epi8(
            posY,
            newY,
            validMask);

    _mm256_storeu_si256(
        reinterpret_cast<__m256i *>(&h.posX[begin]),
        finalX);

    _mm256_storeu_si256(
        reinterpret_cast<__m256i *>(&h.posY[begin]),
        finalY);

    alignas(32) uint16_t validLane[16];

    _mm256_store_si256(
        reinterpret_cast<__m256i *>(validLane),
        validMask);

    for (size_t lane = 0; lane < BATCH_SIZE; ++lane)
    {
        size_t i = begin + lane;

        uint16_t fx = h.posX[i];
        uint16_t fy = h.posY[i];

        bool valid =
            (validLane[lane] != 0);

        if constexpr (HType == HumanType::FoodCollector)
        {
            if (h.targetX[i] != UINT16_MAX &&
                h.targetY[i] != UINT16_MAX &&
                human.gotResource(
                    fx,
                    fy,
                    h.targetX[i],
                    h.targetY[i]))
            {
                h.targetX[i] = UINT16_MAX;
                h.targetY[i] = UINT16_MAX;

                if (valid &&
                    world.getCell(fx, fy) ==
                        TerrainType::LandWithFood)
                {
                    h.points[i] += h.points[i] & 3;

                    human.threadResults[threadID]
                        .foodCollected++;

                    world.setCell(
                        fx,
                        fy,
                        TerrainType::Land);

                    renderer.addToDirtyBuffer(
                        world,
                        fx,
                        fy,
                        sf::Color::Green,
                        threadID);
                }
            }

            if (!valid)
            {
                h.points[i] -= 4;
            }
        }

        else if constexpr (HType == HumanType::WoodCollector)
        {
            if (h.targetX[i] != UINT16_MAX &&
                h.targetY[i] != UINT16_MAX &&
                human.gotResource(
                    fx,
                    fy,
                    h.targetX[i],
                    h.targetY[i]))
            {
                h.targetX[i] = UINT16_MAX;
                h.targetY[i] = UINT16_MAX;

                if (valid &&
                    world.getCell(fx, fy) ==
                        TerrainType::LandWithTree)
                {
                    h.points[i] += h.points[i] & 3;

                    human.threadResults[threadID]
                        .woodCollected++;

                    world.setCell(
                        fx,
                        fy,
                        TerrainType::Land);

                    renderer.addToDirtyBuffer(
                        world,
                        fx,
                        fy,
                        sf::Color::Green,
                        threadID);
                }
            }

            if (!valid)
            {
                h.points[i] -= 4;
            }
        }

        else if constexpr (HType == HumanType::StoneCollector)
        {
            if (h.targetX[i] != UINT16_MAX &&
                h.targetY[i] != UINT16_MAX &&
                human.gotResource(
                    fx,
                    fy,
                    h.targetX[i],
                    h.targetY[i]))
            {
                h.targetX[i] = UINT16_MAX;
                h.targetY[i] = UINT16_MAX;

                if (valid &&
                    world.getCell(fx, fy) ==
                        TerrainType::MountainWithStone)
                {
                    h.points[i] += h.points[i] & 3;

                    human.threadResults[threadID]
                        .stoneCollected++;

                    world.setCell(
                        fx,
                        fy,
                        TerrainType::Mountain);

                    renderer.addToDirtyBuffer(
                        world,
                        fx,
                        fy,
                        sf::Color(120, 120, 120),
                        threadID);
                }
            }

            if (!valid)
            {
                h.points[i] -= 4;
            }
        }

        else if constexpr (HType == HumanType::Builder)
        {
            if (h.targetX[i] != UINT16_MAX &&
                h.targetY[i] != UINT16_MAX &&
                human.gotResource(
                    fx,
                    fy,
                    h.targetX[i],
                    h.targetY[i]))
            {
                h.points[i]++;

                auto ref =
                    world.getCellRef(fx, fy);

                if (world.getBuilding(
                        ref.chunkX,
                        ref.chunkY) != BuildingType::None &&
                    world.hasChunkFlag(
                        ref.chunkX,
                        ref.chunkY,
                        ChunkFlag::Construction))
                {
                    BuildingType building =
                        world.getBuilding(
                            ref.chunkX,
                            ref.chunkY);

                    Type type =
                        GetTypeBuilding(building);

                    human.threadResults[threadID]
                        .constr
                        .push_back(
                            {ref.chunkX,
                             ref.chunkY,
                             type});
                }
                else
                {
                    h.targetX[i] = UINT16_MAX;
                    h.targetY[i] = UINT16_MAX;
                }
            }

            if (!valid)
            {
                h.points[i] -= 4;
            }
        }

        else if constexpr (HType == HumanType::Assigned)
        {
            if (h.targetX[i] != UINT16_MAX &&
                h.targetY[i] != UINT16_MAX &&
                human.gotResource(
                    fx,
                    fy,
                    h.targetX[i],
                    h.targetY[i]))
            {
                h.points[i]++;

                auto ref =
                    world.getCellRef(fx, fy);

                if (world.getBuilding(
                        ref.chunkX,
                        ref.chunkY) ==
                        h.targetBuilding[i] &&
                    !world.hasChunkFlag(
                        ref.chunkX,
                        ref.chunkY,
                        ChunkFlag::Construction))
                {
                    switch (h.targetBuilding[i])
                    {
                    case BuildingType::Farm:
                        human.threadResults[threadID]
                            .farmWorkersDelta++;
                        break;

                    case BuildingType::Sawmill:
                        human.threadResults[threadID]
                            .sawmillWorkersDelta++;
                        break;

                    case BuildingType::Mine:
                        human.threadResults[threadID]
                            .mineWorkersDelta++;
                        break;

                    default:
                        break;
                    }

                    human.threadResults[threadID]
                        .assignedRemoveQueue
                        .push_back(i);

                    continue;
                }
                else
                {
                    h.targetX[i] = UINT16_MAX;
                    h.targetY[i] = UINT16_MAX;
                }
            }

            if (!valid)
            {
                h.points[i] -= 4;
            }
        }
    }
}

template <HumanType HType>
inline void processHumanType(
    Human &human,
    HumanBase &h,
    World &world,
    RendererSFML &renderer,
    size_t i,
    int threadID)
{
    Human::Dirs dir =
        human.humanMoveDecision(
            h.posX[i],
            h.posY[i],
            h.targetX[i],
            h.targetY[i],
            i + h.points[i]);

    uint16_t newX =
        h.posX[i] + dir.x;

    uint16_t newY =
        h.posY[i] + dir.y;

    if constexpr (HType == HumanType::FoodCollector)
    {
        if (h.targetX[i] != UINT16_MAX &&
            h.targetY[i] != UINT16_MAX &&
            human.gotResource(
                newX,
                newY,
                h.targetX[i],
                h.targetY[i]))
        {
            h.targetX[i] = UINT16_MAX;
            h.targetY[i] = UINT16_MAX;

            if (world.getCell(newX, newY) ==
                TerrainType::LandWithFood)
            {
                h.points[i] += h.points[i] & 3;

                human.threadResults[threadID]
                    .foodCollected++;

                world.setCell(
                    newX,
                    newY,
                    TerrainType::Land);

                renderer.addToDirtyBuffer(
                    world,
                    newX,
                    newY,
                    sf::Color::Green,
                    threadID);
            }
        }
    }

    else if constexpr (HType == HumanType::WoodCollector)
    {
        if (h.targetX[i] != UINT16_MAX &&
            h.targetY[i] != UINT16_MAX &&
            human.gotResource(
                newX,
                newY,
                h.targetX[i],
                h.targetY[i]))
        {
            h.targetX[i] = UINT16_MAX;
            h.targetY[i] = UINT16_MAX;

            if (world.getCell(newX, newY) ==
                TerrainType::LandWithTree)
            {
                h.points[i] += h.points[i] & 3;

                human.threadResults[threadID]
                    .woodCollected++;

                world.setCell(
                    newX,
                    newY,
                    TerrainType::Land);

                renderer.addToDirtyBuffer(
                    world,
                    newX,
                    newY,
                    sf::Color::Green,
                    threadID);
            }
        }
    }

    else if constexpr (HType == HumanType::StoneCollector)
    {
        if (h.targetX[i] != UINT16_MAX &&
            h.targetY[i] != UINT16_MAX &&
            human.gotResource(
                newX,
                newY,
                h.targetX[i],
                h.targetY[i]))
        {
            h.targetX[i] = UINT16_MAX;
            h.targetY[i] = UINT16_MAX;

            if (world.getCell(newX, newY) ==
                TerrainType::MountainWithStone)
            {
                h.points[i] += h.points[i] & 3;

                human.threadResults[threadID]
                    .stoneCollected++;

                world.setCell(
                    newX,
                    newY,
                    TerrainType::Mountain);

                renderer.addToDirtyBuffer(
                    world,
                    newX,
                    newY,
                    sf::Color(120, 120, 120),
                    threadID);
            }
        }
    }

    else if constexpr (HType == HumanType::Builder)
    {
        if (h.targetX[i] != UINT16_MAX &&
            h.targetY[i] != UINT16_MAX &&
            human.gotResource(
                newX,
                newY,
                h.targetX[i],
                h.targetY[i]))
        {
            h.points[i]++;

            auto ref =
                world.getCellRef(
                    newX,
                    newY);

            if (world.getBuilding(
                    ref.chunkX,
                    ref.chunkY) != BuildingType::None &&
                world.hasChunkFlag(
                    ref.chunkX,
                    ref.chunkY,
                    ChunkFlag::Construction))
            {
                BuildingType building =
                    world.getBuilding(
                        ref.chunkX,
                        ref.chunkY);

                Type type =
                    GetTypeBuilding(building);

                human.threadResults[threadID]
                    .constr
                    .push_back(
                        {ref.chunkX,
                         ref.chunkY,
                         type});
            }
            else
            {
                h.targetX[i] = UINT16_MAX;
                h.targetY[i] = UINT16_MAX;
            }
        }
    }

    else if constexpr (HType == HumanType::Assigned)
    {
        if (h.targetX[i] != UINT16_MAX &&
            h.targetY[i] != UINT16_MAX &&
            human.gotResource(
                newX,
                newY,
                h.targetX[i],
                h.targetY[i]))
        {
            h.points[i]++;

            auto ref =
                world.getCellRef(
                    newX,
                    newY);

            if (world.getBuilding(
                    ref.chunkX,
                    ref.chunkY) ==
                    h.targetBuilding[i] &&
                !world.hasChunkFlag(
                    ref.chunkX,
                    ref.chunkY,
                    ChunkFlag::Construction))
            {
                switch (h.targetBuilding[i])
                {
                case BuildingType::Farm:
                    human.threadResults[threadID]
                        .farmWorkersDelta++;
                    break;

                case BuildingType::Sawmill:
                    human.threadResults[threadID]
                        .sawmillWorkersDelta++;
                    break;

                case BuildingType::Mine:
                    human.threadResults[threadID]
                        .mineWorkersDelta++;
                    break;

                default:
                    break;
                }

                human.threadResults[threadID]
                    .assignedRemoveQueue
                    .push_back(i);

                return;
            }
            else
            {
                h.targetX[i] = UINT16_MAX;
                h.targetY[i] = UINT16_MAX;
            }
        }
    }

    if (world.isValid(newX, newY))
    {
        h.posX[i] = newX;
        h.posY[i] = newY;
    }
    else
    {
        h.points[i] -= 4;
    }
}