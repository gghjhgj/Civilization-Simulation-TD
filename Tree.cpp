#include "Tree.h"

void Tree::addTree(World &world, int index)
{
    world.grid[index].treeHP += Config::treeHP;
    world.trees.push_back(index);
    world.addToDirtyCells(index);
    treesCount++;
}
void Tree::addTreeNoVec(World &world, int index)
{
    world.grid[index].treeHP += Config::treeHP;
    world.addToDirtyCells(index);
    treesCount++;
}

void Tree::createTree(World &world)
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> distLands(0, world.lands.size() - 1); 
    std::uniform_int_distribution<int> distDir(0, 3);                 

    int tr = 0;
    int maxTreeSpawnTries = Config::maxTreeSpawnTries;
    for (int i = 0; i < Config::forestCount || tr < Config::treeCount; i++)
    {
        int id;
        do
        {
            id = world.lands[distLands(rng)];
        } while (!world.isEmpty(id));
        
        addTree(world, id);
        tr++;

        for (int j = 0; j < Config::treeCount / Config::forestCount && tr < Config::treeCount; j++)
        {
            int k = 1;
            int x, y;
            do
            {
                id = world.trees.back();
                x = id % Config::sizeX;
                y = id / Config::sizeX;
                
                int dir = distDir(rng);
                
                if(dir == 0)
                {
                    x += k;
                    y += k;
                }
                else if(dir == 1)
                {
                    x -= k;
                    y -= k;
                }
                else if(dir == 2)
                {
                    x -= k;
                    y += k;
                }
                else if(dir == 3)
                {
                    x += k;
                    y -= k;
                }
                k++;
                if(k > maxTreeSpawnTries) break;
            } while (!world.isValid(x , y) || !world.isEmpty(x, y) || !(world.grid[world.index(x, y)].flags & Land));
            
            if(k <= maxTreeSpawnTries)
            {
                addTree(world, (y * Config::sizeX + x));
                tr++;
            }
        }
    }
}

void Tree::treeRespawn(World &world)
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> distLands(0, world.lands.size() - 1); 
    std::uniform_int_distribution<int> distDir(0, 3);                 

    int tr = 0;
    int id;
        if(treesCount < Config::maxTree)
        {
        for (int i = 0; i < Config::treeRespawn; i++)
        {
            int k = 1;
            int x, y;
            int maxTreeSpawnTries = Config::maxTreeSpawnTries;
            do
            {
                std::uniform_int_distribution<int> distTree(0, world.trees.size() - 1);
                id = world.trees[distTree(rng)];
                x = id % Config::sizeX;
                y = id / Config::sizeX;
                
                int dir = distDir(rng);
                
                if(dir == 0)
                {
                    x += k;
                    y += k;
                }
                else if(dir == 1)
                {
                    x -= k;
                    y -= k;
                }
                else if(dir == 2)
                {
                    x -= k;
                    y += k;
                }
                else if(dir == 3)
                {
                    x += k;
                    y -= k;
                }
                k++;
                if(k > maxTreeSpawnTries) break;
            } while (!world.isValid(x , y) || !world.isEmpty(x, y) || !(world.grid[world.index(x, y)].flags & Land) || world.grid[id].civZone > 0);
            
            if(k <= maxTreeSpawnTries)
            {
                addTreeNoVec(world, (y * Config::sizeX + x));
                tr++;
            }
        }
        }
    }