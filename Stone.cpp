#include "Stone.h"

void Stone::addStone(World &world, uint32_t x, uint32_t y)
{
    world.setCell(x, y, TerrainType::MountainWithStone);
    world.addToDirtyCells(x, y);
    stonesCount++;
}

void Stone::createStone(World &world)
{
    std::mt19937 rng(std::random_device{}());

    for(int i = 0; i < Config::stoneCount; i++)
    {
        int tries = 0;
        while(tries < Config::maxStoneSpawnTries)
        {
            uint32_t x = rng() % Config::sizeX;
            uint32_t y = rng() % Config::sizeY;

            if(world.getCell(x, y) == TerrainType::Mountain)
            {
                world.setCell(x, y, TerrainType::MountainWithStone);
                world.addToDirtyCells(x, y);
                stonesCount++;
                break;
            }
            tries++;
        }
    }
}

void Stone::stoneRespawn(World &world)
{
    if (stonesCount >= Config::maxStone) return;

    std::mt19937 rng(std::random_device{}());

    for(int i = 0; i < Config::StoneRespawn; i++)
    {
        int tries = 0;
        while(tries < Config::maxStoneSpawnTries)
        {
            uint32_t x = rng() % Config::sizeX;
            uint32_t y = rng() % Config::sizeY;

            auto ref = world.getCellRef(x, y);

            if(world.hasChunkFlag(ref.chunkX, ref.chunkY, ChunkFlag::CivZone))
            {
                tries++;
                continue;
            }
            if(world.getCell(x, y) != TerrainType::Mountain)
            {
                tries++;
                continue;
            }
            world.setCell(x, y, TerrainType::MountainWithStone);
            world.addToDirtyCells(x, y);
            stonesCount++;
            break;
        }
    }
}