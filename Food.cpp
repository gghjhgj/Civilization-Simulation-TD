#include "Food.h"

void Food::addFood(World& world, RendererSFML &renderer, uint16_t x, uint16_t y)
{
    world.setCell(x, y, TerrainType::LandWithFood);
    renderer.addToDirtyCells(world, x, y, sf::Color(255, 165, 0));
    foodsCount++;
}

void Food::createFood(World& world, RendererSFML &renderer)
{
    std::mt19937 rng(std::random_device{}());

    for (int i = 0; i < Config::foodCount; i++)
    {
        int tries = 0;
        while (tries < Config::maxFoodSpawnTries)
        {
            uint16_t x = rng() % Config::sizeX;
            uint16_t y = rng() % Config::sizeY;

            if (world.getCell(x, y) == TerrainType::Land)
            {
                world.setCell(x, y, TerrainType::LandWithFood);
                renderer.addToDirtyCells(world, x, y, sf::Color(255, 165, 0));
                foodsCount++;
                break;
            }
            tries++;
        }
    }
}

void Food::foodRespawn(World& world, RendererSFML &renderer)
{
    if (foodsCount >= Config::maxFood) return;

    std::mt19937 rng(std::random_device{}());

    for (int i = 0; i < Config::foodCount; i++)
    {
        int tries = 0;
        while (tries < Config::maxFoodSpawnTries)
        {
            uint16_t x = rng() % Config::sizeX;
            uint16_t y = rng() % Config::sizeY;

            auto ref = world.getCellRef(x, y);

            if(world.hasChunkFlag(ref.chunkX, ref.chunkY, ChunkFlag::CivZone))
            {
                tries++;
                continue;
            }
            if(world.getCell(x, y) != TerrainType::Land)
            {
                tries++;
                continue;
            }
            world.setCell(x, y, TerrainType::LandWithFood);
            renderer.addToDirtyCells(world, x, y, sf::Color(255,165,0));
            foodsCount++;
            break;
        }
    }
}