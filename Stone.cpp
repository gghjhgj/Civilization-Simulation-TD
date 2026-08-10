#include "Stone.h"

void Stone::addStone(
    World &world,
    RendererSFML &renderer,
    uint16_t x,
    uint16_t y)
{
    world.setCell(
        x,
        y,
        TerrainType::MountainWithStone);

    renderer.addToDirtyCells(
        world,
        x,
        y,
        sf::Color::White);

    stonesCount++;
}

void Stone::createStone(
    World &world,
    RendererSFML &renderer)
{
    std::mt19937 rng(
        std::random_device{}());

    std::uniform_int_distribution<int> mountainDist(
        0,
        Config::numberOfMountains - 1);

    std::uniform_int_distribution<uint16_t> coordDist;

    for (int i = 0;
         i < Config::stoneCount;
         ++i)
    {
        int tries = 0;

        while (tries < Config::maxStoneSpawnTries)
        {
            const int mountainID =
                mountainDist(rng);

            const auto &mountain =
                world.mountainsRanges[mountainID];

            const uint16_t x =
                coordDist(
                    rng,
                    decltype(coordDist)::param_type{
                        mountain.minX,
                        mountain.maxX});

            const uint16_t y =
                coordDist(
                    rng,
                    decltype(coordDist)::param_type{
                        mountain.minY,
                        mountain.maxY});

            if (world.getCell(x, y) ==
                TerrainType::Mountain)
            {
                addStone(
                    world,
                    renderer,
                    x,
                    y);

                break;
            }

            ++tries;
        }
    }
}

void Stone::stoneRespawn(
    World &world,
    RendererSFML &renderer)
{
    if (stonesCount >= Config::maxStone)
        return;

    std::mt19937 rng(
        std::random_device{}());

    std::uniform_int_distribution<int> mountainDist(
        0,
        Config::numberOfMountains - 1);

    std::uniform_int_distribution<uint16_t> coordDist;

    for (int i = 0;
         i < Config::StoneRespawn;
         ++i)
    {
        int tries = 0;

        while (tries < Config::maxStoneSpawnTries)
        {
            const int mountainID =
                mountainDist(rng);

            const auto &mountain =
                world.mountainsRanges[mountainID];

            const uint16_t x =
                coordDist(
                    rng,
                    decltype(coordDist)::param_type{
                        mountain.minX,
                        mountain.maxX});

            const uint16_t y =
                coordDist(
                    rng,
                    decltype(coordDist)::param_type{
                        mountain.minY,
                        mountain.maxY});

            const auto ref =
                world.getCellRef(x, y);

            if (world.hasChunkFlag(
                    ref.chunkX,
                    ref.chunkY,
                    ChunkFlag::CivZone))
            {
                ++tries;
                continue;
            }

            if (world.getCell(x, y) !=
                TerrainType::Mountain)
            {
                ++tries;
                continue;
            }

            addStone(
                world,
                renderer,
                x,
                y);

            break;
        }
    }
}