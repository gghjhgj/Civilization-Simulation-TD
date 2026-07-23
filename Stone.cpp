#include "Stone.h"

void Stone::addStone(World &world, RendererSFML &renderer, uint16_t x, uint16_t y)
{
    world.setCell(x, y, TerrainType::MountainWithStone);
    renderer.addToDirtyCells(world, x, y, sf::Color::White);
    stonesCount++;
}

void Stone::createStone(World &world, RendererSFML &renderer)
{
    std::mt19937 rng(std::random_device{}());

    for(int i = 0; i < Config::stoneCount; i++)
    {
        int tries = 0;
        while(tries < Config::maxStoneSpawnTries)
        {
            uint16_t x = rng() % Config::sizeX;
            uint16_t y = rng() % Config::sizeY;

            if(world.getCell(x, y) == TerrainType::Mountain)
            {
                world.setCell(x, y, TerrainType::MountainWithStone);
                renderer.addToDirtyCells(world, x, y, sf::Color::White);
                stonesCount++;
                break;
            }
            tries++;
        }
    }
}

void Stone::stoneRespawn(World &world, RendererSFML &renderer)
{
    if (stonesCount >= Config::maxStone) return;

    std::mt19937 rng(std::random_device{}());

    for(int i = 0; i < Config::StoneRespawn; i++)
    {
        int tries = 0;
        while(tries < Config::maxStoneSpawnTries)
        {
            uint16_t x = rng() % Config::sizeX;
            uint16_t y = rng() % Config::sizeY;

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
            renderer.addToDirtyCells(world, x, y, sf::Color::White);
            stonesCount++;
            break;
        }
    }
}