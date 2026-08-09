#include "Human.h"

#include "WorldData/World.h"

#include "Civilization.h"

void Human::createHuman(World &world, Civilization &civilization)
{
    size_t threadsCount = tbb::this_task_arena::max_concurrency();

    threadResults.resize(threadsCount);

    size_t reserveCount = Config::humanCount + 40000;

    reserveHumans(foodCollectors, reserveCount);
    reserveHumans(woodCollectors, reserveCount);
    reserveHumans(stoneCollectors, reserveCount);
    reserveHumans(builders, reserveCount);
    reserveHumans(assigned, reserveCount);

    uint16_t x = static_cast<uint16_t>(civilization.spawn.chunkX * ChunkConfig::CHUNK_SIZE);
    uint16_t y = static_cast<uint16_t>(civilization.spawn.chunkY * ChunkConfig::CHUNK_SIZE);

    uint16_t x2;
    uint16_t y2;

    int maxRange = (Config::sizeX > Config::sizeY) ? Config::sizeX : Config::sizeY;

    for (int i = 0; i < Config::humanCount; i++)
    {
        int r = i + 1;
        if (r > maxRange)
            r = maxRange;

        do
        {
            int dx = (rand() % (2 * r + 1)) - r;
            int dy = (rand() % (2 * r + 1)) - r;

            x2 = x + dx;
            y2 = y + dy;
        } while (!world.isValid(x2, y2) ||
                 world.getCell(x2, y2) != TerrainType::Land);

        addHuman(*this, this->foodCollectors, BuildingType::None, x2, y2);
    }

    /*
 for (int i = 0; i < Config::humanCount; i++)
 {
    addHuman(*this, this->foodCollectors, BuildingType::None, x, y);
 }
    */
}

void Human::humanRespawn(World &world, Civilization &civilization)
{
    int newPeople = static_cast<int>(std::cbrt(humansCount));
    uint16_t x = static_cast<uint16_t>(civilization.spawn.chunkX * ChunkConfig::CHUNK_SIZE);
    uint16_t y = static_cast<uint16_t>(civilization.spawn.chunkY * ChunkConfig::CHUNK_SIZE);
    for (int i = 0; i < newPeople; i++)
    {
        addHuman(*this, this->foodCollectors, BuildingType::None, x, y); // x+random100(), y+random100());
    }
}
XY Human::humanFindResource(World &world, uint16_t x, uint16_t y, TerrainType type)
{
    if (!world.isValid(x, y))
    {
        return {UINT16_MAX, UINT16_MAX};
    }

    const uint16_t chunkX = x / 3;
const uint16_t chunkY = y / 3;

const uint16_t regionX =
    chunkX / ChunkRegionConfig::CHUNK_REGION_SIZE;

const uint16_t regionY =
    chunkY / ChunkRegionConfig::CHUNK_REGION_SIZE;

const uint16_t localX =
    chunkX % ChunkRegionConfig::CHUNK_REGION_SIZE;

const uint16_t localY =
    chunkY % ChunkRegionConfig::CHUNK_REGION_SIZE;

const uint32_t regionIndex =
    regionY * WorldConfig::CHUNK_REGIONS_X + regionX;

const uint16_t localChunkIndex =
    localY * ChunkRegionConfig::CHUNK_REGION_SIZE + localX;

    uint16_t cell =
        world.grid[regionIndex]
            .chunks[localChunkIndex]
            .whereType(type);

    if (cell != ChunkConfig::CELL_COUNT)
    {
        return {
            static_cast<uint16_t>(chunkX * 3 + cell % 3),
            static_cast<uint16_t>(chunkY * 3 + cell / 3)};
    }

    return {UINT16_MAX, UINT16_MAX};
}
XY Human::humanFindFlagChunk(World &world, uint16_t x, uint16_t y, ChunkFlag flag)
{
    if (!world.isValid(x, y))
    {
        return {UINT16_MAX, UINT16_MAX};
    }
    auto ref = world.getCellRef(x, y);

    auto &region = world.grid[ref.chunkRegionIndex];

    for (uint16_t i = 0; i < ChunkRegionConfig::CHUNK_REGION_SIZE * ChunkRegionConfig::CHUNK_REGION_SIZE; i++)
    {
        auto &chunk = region.chunks[i];

        if (chunk.hasFlag(ChunkFlag::Construction))
        {
            uint16_t localChunkX =
                i % ChunkRegionConfig::CHUNK_REGION_SIZE;

            uint16_t localChunkY =
                i / ChunkRegionConfig::CHUNK_REGION_SIZE;

            uint16_t regionX =
                ref.chunkX / ChunkRegionConfig::CHUNK_REGION_SIZE;

            uint16_t regionY =
                ref.chunkY / ChunkRegionConfig::CHUNK_REGION_SIZE;

            uint16_t chunkX =
                regionX * ChunkRegionConfig::CHUNK_REGION_SIZE + localChunkX;

            uint16_t chunkY =
                regionY * ChunkRegionConfig::CHUNK_REGION_SIZE + localChunkY;

            return {
                static_cast<uint16_t>(chunkX * ChunkConfig::CHUNK_SIZE),
                static_cast<uint16_t>(chunkY * ChunkConfig::CHUNK_SIZE)};
        }
    }
    return {UINT16_MAX, UINT16_MAX};
}
XY Human::humanFindWorkingBuildingChunk(
    World &world,
    uint16_t x,
    uint16_t y,
    BuildingType type)
{
    if (!world.isValid(x, y))
    {
        return {UINT16_MAX, UINT16_MAX};
    }
    auto ref = world.getCellRef(x, y);

    auto &region = world.grid[ref.chunkRegionIndex];

    uint16_t regionX =
        ref.chunkX / ChunkRegionConfig::CHUNK_REGION_SIZE;

    uint16_t regionY =
        ref.chunkY / ChunkRegionConfig::CHUNK_REGION_SIZE;

    for (uint16_t i = 0;
         i < ChunkRegionConfig::CHUNK_REGION_SIZE * ChunkRegionConfig::CHUNK_REGION_SIZE;
         i++)
    {
        auto &chunk = region.chunks[i];

        if (chunk.getBuilding() == type &&
            !chunk.hasFlag(ChunkFlag::Construction))
        {
            uint16_t localChunkX =
                i % ChunkRegionConfig::CHUNK_REGION_SIZE;

            uint16_t localChunkY =
                i / ChunkRegionConfig::CHUNK_REGION_SIZE;

            uint16_t chunkX =
                regionX * ChunkRegionConfig::CHUNK_REGION_SIZE + localChunkX;

            uint16_t chunkY =
                regionY * ChunkRegionConfig::CHUNK_REGION_SIZE + localChunkY;

            return {
                static_cast<uint16_t>(chunkX * ChunkConfig::CHUNK_SIZE),
                static_cast<uint16_t>(chunkY * ChunkConfig::CHUNK_SIZE)};
        }
    }

    return {UINT16_MAX, UINT16_MAX};
}
inline bool Human::gotResource(uint16_t hx, uint16_t hy, uint16_t rx, uint16_t ry)
{
    if (hx == rx && hy == ry)
        return true;
    return false;
}

inline Human::Dirs Human::humanMoveDecision(
    uint16_t x, uint16_t y,
    uint16_t targetX, uint16_t targetY,
    uint8_t points)
{
    int a;
    if (targetX == UINT16_MAX || targetY == UINT16_MAX)
    {
        uint8_t directionIndex = points & 7;
        static constexpr int lookupX[8] = {1, 1, 0, -1, -1, -1, 0, 1};
        static constexpr int lookupY[8] = {0, 1, 1, 1, 0, -1, -1, -1};
        return {
            static_cast<int8_t>(lookupX[directionIndex]),
            static_cast<int8_t>(lookupY[directionIndex])};
    }

    int dx = targetX - x;
    int dy = targetY - y;
    int8_t dirX = (dx > 0) - (dx < 0);
    int8_t dirY = (dy > 0) - (dy < 0);

    return {dirX, dirY};
}

void Human::processFoodCollector(
    World &world,
    RendererSFML &renderer,
    size_t i,
    int threadID)
{
    auto &h = foodCollectors;
    Dirs dir = humanMoveDecision(
        h.posX[i],
        h.posY[i],
        h.targetX[i],
        h.targetY[i],
        i + h.points[i]);

    uint16_t newX = h.posX[i] + dir.x;
    uint16_t newY = h.posY[i] + dir.y;

    if (h.targetX[i] != UINT16_MAX &&
        gotResource(
            newX,
            newY,
            h.targetX[i],
            h.targetY[i]))
    {
        h.targetX[i] = UINT16_MAX;
        h.targetY[i] = UINT16_MAX;
        if (world.getCell(newX, newY) == TerrainType::LandWithFood)
        {
            h.points[i] += h.points[i] & 3;
            threadResults[threadID].foodCollected++;

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

void Human::processFoodCollectorBatch(
    World &world,
    RendererSFML &renderer,
    size_t begin,
    int threadID)
{
    auto &h = foodCollectors;

    constexpr size_t BATCH_SIZE = 16;

    // ============================================================
    // LOAD
    // ============================================================

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

    // ============================================================
    // HELPERS
    // ============================================================

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
            static_cast<int>(static_cast<uint16_t>(Config::sizeX)));

    const __m256i worldHeight =
        _mm256_set1_epi16(
            static_cast<int>(static_cast<uint16_t>(Config::sizeY)));

    // ============================================================
    // noTarget = targetX == MAX || targetY == MAX
    // ============================================================

    __m256i noTargetMask =
        _mm256_or_si256(
            _mm256_cmpeq_epi16(targetX, invalidTarget16),
            _mm256_cmpeq_epi16(targetY, invalidTarget16));

    // ============================================================
    // dir = sign(target - pos)   (unsigned compare via bias)
    // ============================================================

    __m256i posXBias = _mm256_xor_si256(posX, bias16);
    __m256i posYBias = _mm256_xor_si256(posY, bias16);

    __m256i tgtXBias = _mm256_xor_si256(targetX, bias16);
    __m256i tgtYBias = _mm256_xor_si256(targetY, bias16);

    __m256i gtX =
        _mm256_cmpgt_epi16(tgtXBias, posXBias);

    __m256i ltX =
        _mm256_cmpgt_epi16(posXBias, tgtXBias);

    __m256i dirX =
        _mm256_sub_epi16(
            _mm256_and_si256(gtX, one16),
            _mm256_and_si256(ltX, one16));

    __m256i gtY =
        _mm256_cmpgt_epi16(tgtYBias, posYBias);

    __m256i ltY =
        _mm256_cmpgt_epi16(posYBias, tgtYBias);

    __m256i dirY =
        _mm256_sub_epi16(
            _mm256_and_si256(gtY, one16),
            _mm256_and_si256(ltY, one16));

    // ============================================================
    // Fallback: points & 7 -> lookup
    // i = begin + lane
    // directionIndex = (i + points[i]) & 7
    // ============================================================

    static constexpr int16_t lookupX[8] =
    {
        1, 1, 0, -1, -1, -1, 0, 1
    };

    static constexpr int16_t lookupY[8] =
    {
        0, 1, 1, 1, 0, -1, -1, -1
    };

    alignas(32) int16_t fallbackDirX[16];
    alignas(32) int16_t fallbackDirY[16];

    for (size_t lane = 0; lane < BATCH_SIZE; ++lane)
    {
        size_t i = begin + lane;
        uint8_t idx = static_cast<uint8_t>((i + h.points[i]) & 7u);
        fallbackDirX[lane] = lookupX[idx];
        fallbackDirY[lane] = lookupY[idx];
    }

    __m256i fallbackDirXVec =
        _mm256_load_si256(
            reinterpret_cast<const __m256i *>(fallbackDirX));

    __m256i fallbackDirYVec =
        _mm256_load_si256(
            reinterpret_cast<const __m256i *>(fallbackDirY));

    // ============================================================
    // noTarget -> fallback
    // target   -> sign(target - pos)
    // ============================================================

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

    // ============================================================
    // NEW X / NEW Y
    // ============================================================

    __m256i newX =
        _mm256_add_epi16(
            posX,
            dirX);

    __m256i newY =
        _mm256_add_epi16(
            posY,
            dirY);

    // ============================================================
    // VALID = newX < sizeX && newY < sizeY
    // unsigned compare via bias
    // ============================================================

    __m256i newXBias = _mm256_xor_si256(newX, bias16);
    __m256i newYBias = _mm256_xor_si256(newY, bias16);

    __m256i widthBias = _mm256_xor_si256(worldWidth, bias16);
    __m256i heightBias = _mm256_xor_si256(worldHeight, bias16);

    __m256i validX =
        _mm256_cmpgt_epi16(widthBias, newXBias);

    __m256i validY =
        _mm256_cmpgt_epi16(heightBias, newYBias);

    __m256i validMask =
        _mm256_and_si256(validX, validY);

    // ============================================================
    // VALID -> NEW
    // INVALID -> OLD
    // ============================================================

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

    // ============================================================
    // SCALAR TAIL:
    // gotResource / target reset / world.getCell / setCell / renderer
    // points -= 4 dla invalid
    // ============================================================

    alignas(32) uint16_t validLane[16];

    _mm256_store_si256(
        reinterpret_cast<__m256i *>(validLane),
        validMask);

    for (size_t lane = 0; lane < BATCH_SIZE; ++lane)
    {
        size_t i = begin + lane;

        uint16_t fx = h.posX[i];
        uint16_t fy = h.posY[i];
        bool valid = (validLane[lane] != 0);

        if (h.targetX[i] != UINT16_MAX &&
            h.targetY[i] != UINT16_MAX &&
            gotResource(fx, fy, h.targetX[i], h.targetY[i]))
        {
            h.targetX[i] = UINT16_MAX;
            h.targetY[i] = UINT16_MAX;

            if (valid &&
                world.getCell(fx, fy) == TerrainType::LandWithFood)
            {
                h.points[i] += h.points[i] & 3;

                threadResults[threadID].foodCollected++;

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
}

void Human::processFoodCollectors(
    World &world,
    RendererSFML &renderer)
{
    aiArena.execute(
        [&]()
        {
            tbb::parallel_for(
                tbb::blocked_range<size_t>(
                    0,
                    foodCollectors.posX.size(),
                    Config::GRAIN),

                [&](const tbb::blocked_range<size_t> &range)
                {
                    auto &h = foodCollectors;
                    int threadID =
                        tbb::this_task_arena::current_thread_index();
                    for (int i = 0; i < ticksToDo; i++)
                    {
                        if ((humanTicks + i) % 3 == 0)
                        {
                            for (size_t i = range.begin(); i < range.end(); i++)
                            {
                                XY target =
                                    humanFindResource(
                                        world,
                                        h.posX[i],
                                        h.posY[i],
                                        TerrainType::LandWithFood);

                                h.targetX[i] = target.x;
                                h.targetY[i] = target.y;
                            }
                        }
                        constexpr size_t BATCH_SIZE = 16;

                        size_t begin = range.begin();
                        size_t end = range.end();

                        size_t batchEnd =
                            begin + ((end - begin) / BATCH_SIZE) * BATCH_SIZE;

                        for (size_t batch = begin; batch < batchEnd; batch += BATCH_SIZE)
                        {
                            processFoodCollectorBatch(
                                world,
                                renderer,
                                batch,
                                threadID);
                        }

                        for (size_t i = batchEnd; i < end; ++i)
                        {
                            processFoodCollector(
                                world,
                                renderer,
                                i,
                                threadID);
                        }
                    }
                },
                foodCollectorsPartitioner);
        });
}

void Human::processWoodCollectors(
    World &world,
    RendererSFML &renderer)
{
    aiArena.execute(
        [&]()
        {
            tbb::parallel_for(
                tbb::blocked_range<size_t>(
                    0,
                    woodCollectors.posX.size(),
                    Config::GRAIN),

                [&](const tbb::blocked_range<size_t> &range)
                {
                    auto &h = woodCollectors;
                    int threadID =
                        tbb::this_task_arena::current_thread_index();
                    for (int i = 0; i < ticksToDo; i++)
                    {
                        if ((humanTicks + i) % 3 == 0)
                        {
                            for (size_t i = range.begin(); i < range.end(); i++)
                            {
                                XY target =
                                    humanFindResource(
                                        world,
                                        h.posX[i],
                                        h.posY[i],
                                        TerrainType::LandWithTree);

                                h.targetX[i] = target.x;
                                h.targetY[i] = target.y;
                            }
                        }
                        for (size_t i = range.begin();
                             i < range.end();
                             i++)
                        {
                            Dirs dir;

                            dir = humanMoveDecision(
                                h.posX[i],
                                h.posY[i],
                                h.targetX[i],
                                h.targetY[i],
                                i + h.points[i]);

                            uint16_t newX = h.posX[i] + dir.x;
                            uint16_t newY = h.posY[i] + dir.y;

                            if (h.targetX[i] != UINT16_MAX &&
                                gotResource(
                                    newX,
                                    newY,
                                    h.targetX[i],
                                    h.targetY[i]))
                            {
                                h.targetX[i] = UINT16_MAX;
                                h.targetY[i] = UINT16_MAX;

                                if (world.getCell(newX, newY) == TerrainType::LandWithTree)
                                {
                                    h.points[i] += h.points[i] & 3;

                                    threadResults[threadID]
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
                    }
                },
                woodCollectorsPartitioner);
        });
}

void Human::processStoneCollectors(
    World &world,
    RendererSFML &renderer)
{
    aiArena.execute(
        [&]()
        {
            tbb::parallel_for(
                tbb::blocked_range<size_t>(
                    0,
                    stoneCollectors.posX.size(),
                    Config::GRAIN),

                [&](const tbb::blocked_range<size_t> &range)
                {
                    auto &h = stoneCollectors;
                    int threadID =
                        tbb::this_task_arena::current_thread_index();

                    for (int i = 0; i < ticksToDo; i++)
                    {
                        if ((humanTicks + i) % 3 == 0)
                        {
                            for (size_t i = range.begin(); i < range.end(); i++)
                            {
                                XY target =
                                    humanFindResource(
                                        world,
                                        h.posX[i],
                                        h.posY[i],
                                        TerrainType::MountainWithStone);

                                h.targetX[i] = target.x;
                                h.targetY[i] = target.y;
                            }
                        }
                        for (size_t i = range.begin();
                             i < range.end();
                             i++)
                        {
                            Dirs dir;

                            dir = humanMoveDecision(
                                h.posX[i],
                                h.posY[i],
                                h.targetX[i],
                                h.targetY[i],
                                i + h.points[i]);

                            uint16_t newX = h.posX[i] + dir.x;
                            uint16_t newY = h.posY[i] + dir.y;

                            if (h.targetX[i] != UINT16_MAX &&
                                gotResource(
                                    newX,
                                    newY,
                                    h.targetX[i],
                                    h.targetY[i]))
                            {
                                h.targetX[i] = UINT16_MAX;
                                h.targetY[i] = UINT16_MAX;

                                if (world.getCell(newX, newY) == TerrainType::MountainWithStone)
                                {
                                    h.points[i] += h.points[i] & 3;

                                    threadResults[threadID]
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
                    }
                },
                stoneCollectorsPartitioner);
        });
}

void Human::processBuilders(
    World &world,
    RendererSFML &renderer,
    Civilization &civilization)
{
    for (int i = 0; i < ticksToDo; i++)
    {
        aiArena.execute(
            [&]()
            {
                tbb::parallel_for(
                    tbb::blocked_range<size_t>(
                        0,
                        builders.posX.size(),
                        Config::GRAIN),

                    [&](const tbb::blocked_range<size_t> &range)
                    {
                        auto &h = builders;
                        int threadID =
                            tbb::this_task_arena::current_thread_index();
                        if ((humanTicks + i) % 13 == 0)
                        {
                            for (size_t i = range.begin();
                                 i < range.end();
                                 i++)
                            {
                                XY target =
                                    humanFindFlagChunk(
                                        world,
                                        h.posX[i],
                                        h.posY[i],
                                        ChunkFlag::Construction);

                                h.targetX[i] = target.x;
                                h.targetY[i] = target.y;
                            }
                        }

                        for (size_t i = range.begin();
                             i < range.end();
                             i++)
                        {
                            Dirs dir =
                                humanMoveDecision(
                                    h.posX[i],
                                    h.posY[i],
                                    h.targetX[i],
                                    h.targetY[i],
                                    i + h.points[i]);

                            uint16_t newX = h.posX[i] + dir.x;
                            uint16_t newY = h.posY[i] + dir.y;

                            if (h.targetX[i] != UINT16_MAX &&
                                gotResource(
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

                                    threadResults[threadID]
                                        .constr.push_back(
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
                    },
                    buildersPartitioner);
            });
        for (const auto &res : threadResults)
        {
            allConstructionsToEnd.insert(
                allConstructionsToEnd.end(),
                res.constr.begin(),
                res.constr.end());
        }
        if (!allConstructionsToEnd.empty())
        {
            std::sort(allConstructionsToEnd.begin(), allConstructionsToEnd.end());
            allConstructionsToEnd.erase(std::unique(allConstructionsToEnd.begin(), allConstructionsToEnd.end()), allConstructionsToEnd.end());

            for (auto constr : allConstructionsToEnd)
            {
                civilization.endConstruction(
                    world,
                    renderer,
                    *this,
                    constr.chunkX,
                    constr.chunkY,
                    constr.type);
            }
            allConstructionsToEnd.clear();
        }
        for (auto &res : threadResults)
        {
            res.constr.clear();
        }
    }
}

void Human::processAssigned(
    World &world,
    RendererSFML &renderer)
{
    for (int i = 0; i < ticksToDo; i++)
    {
        aiArena.execute(
            [&]()
            {
                tbb::parallel_for(
                    tbb::blocked_range<size_t>(
                        0,
                        assigned.posX.size(),
                        Config::GRAIN),

                    [&](const tbb::blocked_range<size_t> &range)
                    {
                        auto &h = assigned;
                        int threadID =
                            tbb::this_task_arena::current_thread_index();
                        if ((humanTicks + i) % 13 == 0)
                        {
                            for (size_t i = range.begin();
                                 i < range.end();
                                 i++)
                            {
                                XY target =
                                    humanFindWorkingBuildingChunk(
                                        world,
                                        h.posX[i],
                                        h.posY[i],
                                        h.targetBuilding[i]);

                                h.targetX[i] = target.x;
                                h.targetY[i] = target.y;
                            }
                        }

                        for (size_t i = range.begin();
                             i < range.end();
                             i++)
                        {
                            Dirs dir =
                                humanMoveDecision(
                                    h.posX[i],
                                    h.posY[i],
                                    h.targetX[i],
                                    h.targetY[i],
                                    i + h.points[i]);

                            uint16_t newX = h.posX[i] + dir.x;
                            uint16_t newY = h.posY[i] + dir.y;

                            if (h.targetX[i] != UINT16_MAX &&
                                gotResource(
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
                                        ref.chunkY) == h.targetBuilding[i] &&
                                    !world.hasChunkFlag(
                                        ref.chunkX,
                                        ref.chunkY,
                                        ChunkFlag::Construction))
                                {
                                    switch (h.targetBuilding[i])
                                    {
                                    case BuildingType::Farm:
                                        threadResults[threadID]
                                            .farmWorkersDelta++;
                                        break;

                                    case BuildingType::Sawmill:
                                        threadResults[threadID]
                                            .sawmillWorkersDelta++;
                                        break;

                                    case BuildingType::Mine:
                                        threadResults[threadID]
                                            .mineWorkersDelta++;
                                        break;

                                    default:
                                        break;
                                    }

                                    threadResults[threadID]
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
                    },
                    assignedPartitioner);
            });
        for (const auto &res : threadResults)
        {
            allAssignedToRemove.insert(
                allAssignedToRemove.end(),
                res.assignedRemoveQueue.begin(),
                res.assignedRemoveQueue.end());
        }
        if (!allAssignedToRemove.empty())
        {
            std::sort(allAssignedToRemove.rbegin(), allAssignedToRemove.rend());
            allAssignedToRemove.erase(std::unique(allAssignedToRemove.begin(), allAssignedToRemove.end()), allAssignedToRemove.end());

            for (size_t id : allAssignedToRemove)
            {
                if (id >= assigned.posX.size())
                {
                    continue;
                }
                eraseHuman(*this, assigned, id);
            }
            allAssignedToRemove.clear();
        }
        for (auto &res : threadResults)
        {
            res.assignedRemoveQueue.clear();
        }
    }
}

void Human::humanMove(World &world, Civilization &civilization, Food &food, Tree &tree, Stone &stone, RendererSFML &renderer)
{
    for (auto &r : threadResults)
    {
        r.clear();
    }
    constexpr uint32_t t = 1000000 + Config::humanCount * 10;
    ticksToDo = t / (foodCollectors.posX.size() + woodCollectors.posX.size() + stoneCollectors.posX.size() + builders.posX.size() + assigned.posX.size());

    actionsToDo = (ticksToDo + ticksLeft) / Config::ticksForNewHumans;
    ticksLeft = (ticksToDo + ticksLeft) % Config::ticksForNewHumans;

    // a)
    /*
    processFoodCollectors(world, renderer);
    processWoodCollectors(world, renderer);
    processStoneCollectors(world, renderer);
    processBuilders(world, renderer);
    processAssigned(world, renderer);
    humanTicks++;
    */

    // b)

    tbb::parallel_invoke(
        [&]
        { processFoodCollectors(world, renderer); },
        [&]
        { processWoodCollectors(world, renderer); },
        [&]
        { processStoneCollectors(world, renderer); },
        [&]
        { processBuilders(world, renderer, civilization); },
        [&]
        { processAssigned(world, renderer); });

    humanTicks += ticksToDo;
    ///////////////////////////sync

    for (const auto &res : threadResults)
    {
        food.foodsCount -= res.foodCollected;
        civilization.resources.food += res.foodCollected;

        tree.treesCount -= res.woodCollected;
        civilization.resources.wood += res.woodCollected;

        stone.stonesCount -= res.stoneCollected;
        civilization.resources.stone += res.stoneCollected;
        civilization.realWorkers[FARM] += res.farmWorkersDelta;
        civilization.realWorkers[SAWMILL] += res.sawmillWorkersDelta;
        civilization.realWorkers[MINE] += res.mineWorkersDelta;
    }

    for (int i = 0; i < actionsToDo; i++)
    {
        humanRespawn(world, civilization);

        civilization.assignHumansToBuilding(*this, Type::FARM);
        civilization.assignHumansToBuilding(*this, Type::SAWMILL);
        civilization.assignHumansToBuilding(*this, Type::MINE);

        civilization.civilizationDecision(*this, food, stone, tree);
    }
}