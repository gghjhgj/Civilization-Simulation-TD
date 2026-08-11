#include "Tree.h"


void Tree::createSeed(World& world, RendererSFML &renderer, int& tr, std::mt19937& rng, bool addToDirty)
{
    int seedTries = 0;


    while (seedTries < Config::resources.trees.maxSpawnTries)
    {
        uint16_t x = rng() % ConfigConstexpr::sizeX;
        uint16_t y = rng() % ConfigConstexpr::sizeY;


        if (world.getCell(x, y) == TerrainType::Land)
        {
            if (!world.isValid(x, y))
                continue;


            auto ref = world.getCellRef(x, y);


            if (world.getCell(x, y) != TerrainType::Land)
                return;


            if (world.hasChunkFlag(ref.chunkX, ref.chunkY, ChunkFlag::CivZone))
                return;
                
            world.setCell(x, y, TerrainType::LandWithTree);
            if(addToDirty)
                renderer.addToDirtyCells(world, x, y, sf::Color(0, 120, 0));


            last.x = x;
            last.y = y;
            tr++;
            return;
        }
        seedTries++;
    }
}


void Tree::addTree(World& world, RendererSFML &renderer, uint16_t x, uint16_t y, bool addToDirty)
{
    world.setCell(x, y, TerrainType::LandWithTree);
    if(addToDirty)
        renderer.addToDirtyCells(world, x, y, sf::Color(0, 120, 0));
    treesCount++;
}


void Tree::createTree(World& world, RendererSFML &renderer)
{
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> distDir(0, 3);


    int tr = 0;
    int maxTreeSpawnTries = Config::resources.trees.maxSpawnTries;
    for (int i = 0; i < Config::resources.trees.forestCount || tr < Config::resources.trees.count; i++)
    {
        int trBefore = tr;
        createSeed(world, renderer, tr, rng, false);
        if (tr == trBefore) continue;


        int treesPerForest = Config::resources.trees.count / Config::resources.trees.forestCount;
        for (int j = 0; j < treesPerForest && tr < Config::resources.trees.count; j++)
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
                addTree(world, renderer, x, y, false);
                last.x = x;
                last.y = y;
                tr++;
            }
            else
            {
                trBefore = tr;
                createSeed(world, renderer, tr, rng, true);
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


    if (treesCount >= Config::resources.trees.max) return;


    for (int i = 0; i < Config::resources.trees.respawn; i++)
    {
        int k = 1;
        int x, y;
        bool placed = false;
        int maxTreeSpawnTries = Config::resources.trees.maxSpawnTries;
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
            createSeed(world, renderer, treesCount, rng, true);
        }
    }
}