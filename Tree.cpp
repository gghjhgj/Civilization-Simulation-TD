#include "Tree.h"

void Tree::createSeed(World& world, RendererSFML &renderer, int& tr, std::mt19937& rng)
{
    int seedTries = 0;

    while (seedTries < Config::maxTreeSpawnTries)
    {
        uint32_t x = rng() % Config::sizeX;
        uint32_t y = rng() % Config::sizeY;

        if (world.getCell(x, y) == TerrainType::Land)
        {
            world.setCell(x, y, TerrainType::LandWithTree);
            renderer.addToDirtyCells(world, x, y, sf::Color(0, 120, 0));

            last.x = x;
            last.y = y;
            tr++;
            return;
        }
        seedTries++;
    }
}

void Tree::addTree(World& world, RendererSFML &renderer, uint32_t x, uint32_t y)
{
    world.setCell(x, y, TerrainType::LandWithTree);
    renderer.addToDirtyCells(world, x, y, sf::Color(0, 120, 0));
    treesCount++;
}

void Tree::createTree(World& world, RendererSFML &renderer)
{
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> distDir(0, 3);

    int tr = 0;
    int maxTreeSpawnTries = Config::maxTreeSpawnTries;
    for (int i = 0; i < Config::forestCount || tr < Config::treeCount; i++)
    {
        int trBefore = tr;
        createSeed(world, renderer, tr, rng);
        if (tr == trBefore) continue;

        int treesPerForest = Config::treeCount / Config::forestCount;
        for (int j = 0; j < treesPerForest && tr < Config::treeCount; j++)
        {
            int k = 1;
            int x, y;
            do
            {
                x = last.x;
                y = last.y;

                int dir = distDir(rng);

                if (dir == 0)
                {
                    x += k;
                    y += k;
                }
                else if (dir == 1)
                {
                    x -= k;
                    y -= k;
                }
                else if (dir == 2)
                {
                    x -= k;
                    y += k;
                }
                else if (dir == 3)
                {
                    x += k;
                    y -= k;
                }
                k++;
                if (k > maxTreeSpawnTries) break;
            } while (!world.isValid(x, y) || world.getCell(x, y) != TerrainType::Land);
            if (k <= maxTreeSpawnTries)
            {
                addTree(world, renderer, x, y);
                last.x = x;
                last.y = y;
                tr++;
            }
            else
            {
                trBefore = tr;
                createSeed(world, renderer, tr, rng);
                if (tr == trBefore) break;
                treesCount++;
            }
        }
    }
}

void Tree::treeRespawn(World& world, RendererSFML &renderer)
{
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> distDir(0, 3);

    if (treesCount >= Config::maxTree) return;

    for (int i = 0; i < Config::treeRespawn; i++)
    {
        int k = 1;
        int x, y;
        bool placed = false;
        int maxTreeSpawnTries = Config::maxTreeSpawnTries;
        do
        {
            x = last.x;
            y = last.y;
            int dir = distDir(rng);

            if (dir == 0)
            {
                x += k;
                y += k;
            }
            else if (dir == 1)
            {
                x -= k;
                y -= k;
            }
            else if (dir == 2)
            {
                x -= k;
                y += k;
            }
            else if (dir == 3)
            {
                x += k;
                y -= k;
            }
            k++;
            if (k > maxTreeSpawnTries)
                break;

            if (!world.isValid(x, y))
                continue;

            auto ref = world.getCellRef(x, y);

            if (world.getCell(x, y) != TerrainType::Land)
                continue;

            if (world.hasChunkFlag(ref.chunkX, ref.chunkY, ChunkFlag::CivZone))
                continue;

            break;
        } while(true);
        if (k <= maxTreeSpawnTries)
        {
            world.setCell(x, y, TerrainType::LandWithTree);
            renderer.addToDirtyCells(world, x, y, sf::Color(0, 120, 0));
            last.x = x;
            last.y = y;
            treesCount++;
            placed = true;
        }
        else
        {
            createSeed(world, renderer, treesCount, rng);
        }
    }
}