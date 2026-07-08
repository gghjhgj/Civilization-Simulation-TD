#include "Human.h"
#include "WorldData/World.h"
#include "Civilization.h"



void Human::addHuman(World &world, int index, int vecId)
{
    world.grid[index].humanIndex = vecId;
    HumanData h;
    h.index = index;
    h.id = vecId;
    humans.push_back(h);
    world.addToDirtyCells(index);
    humansCount++;
}

void Human::createHuman(World &world, Civilization &civilization)
{
    int id = civilization.spawn;
    int id2;
    
    int maxRange = (Config::sizeX > Config::sizeY) ? Config::sizeX : Config::sizeY;

    for(int i = 0; i < Config::humanCount; i++)
    {
        int r = i + 1;
        if (r > maxRange) r = maxRange;

        do
        {
            int dx = (rand() % (2 * r + 1)) - r;
            int dy = (rand() % (2 * r + 1)) - r;

            id2 = id + dx + (Config::sizeX * dy);
            
        } while (!world.isValid(id2) ||
                 !world.isEmpty(id2) ||
                 (world.grid[id2].flags & Water));
        
        addHuman(world, id2, i);
    }
}

void Human::eraseHuman(World &world, Civilization &civilization, int vecID)
{
    world.addToDirtyCells(humans[vecID].index);
    world.grid[humans[vecID].index].humanIndex = 0;
    if (vecID < static_cast<int>(humans.size()) - 1)
    {
        auto& lastHuman = humans.back();
        humans[vecID] = lastHuman;
        world.grid[humans[vecID].index].humanIndex = vecID;
    }
    humans.pop_back();
}
void Human::humanRespawn(World &world, Civilization &civilization)
{
    int newPeople = static_cast<int>(std::cbrt(humansCount));
    for(int i = 0; i < newPeople; i++)
    {
        int id = humans.size();
        addHuman(world, civilization.spawn, id);
    }
}
int Human::humanFindResource(World& world, int startIndex, ResourceType type)
{
    int sx = startIndex % Config::sizeX;
    int sy = startIndex / Config::sizeX;

    for (int dy = -Config::vision; dy <= Config::vision; dy++)
    {
        for (int dx = -Config::vision; dx <= Config::vision; dx++)
        {
            int x = sx + dx;
            int y = sy + dy;

            if (x < 0 || y < 0 ||
                x >= Config::sizeX ||
                y >= Config::sizeY)
                continue;

            int idx = y * Config::sizeX + x;

            bool found = false;

            switch(type)
            {
                case ResourceType::food:
                    found = world.grid[idx].food > 0;
                    break;

                case ResourceType::tree:
                    found = world.grid[idx].treeHP > 0;
                    break;

                case ResourceType::stone:
                    found = world.grid[idx].stoneHP > 0;
                    break;
                
                case ResourceType::construction:
                    found = world.grid[idx].construction.hitsNeeded > 0;
                    break;
            }

            if(found)
                return idx;
        }
    }

    return -1;
}

bool Human::gotResource(int h, int f)
{
    if(h == f) return true;
    return false;
}

int Human::humanMoveDecision(int fromIndex, int targetIndex, int i)
{
    int sx = fromIndex % Config::sizeX;
    int sy = fromIndex / Config::sizeX;

    int tx = targetIndex % Config::sizeX;
    int ty = targetIndex / Config::sizeX;

    int dx = tx - sx;
    int dy = ty - sy;

    int dirX = (dx > 0) - (dx < 0);
    int dirY = (dy > 0) - (dy < 0);
    
    if(targetIndex != -1 && (rand() % 100 > 15))
    {
    if (dirY == -1 && dirX == -1) return 0;
    if (dirY == -1 && dirX ==  0) return 1;
    if (dirY == -1 && dirX ==  1) return 2;
    if (dirY ==  0 && dirX == -1) return 3;
    if (dirY ==  0 && dirX ==  1) return 4;
    if (dirY ==  1 && dirX == -1) return 5;
    if (dirY ==  1 && dirX ==  0) return 6;
    if (dirY ==  1 && dirX ==  1) return 7;
    }

    return (humans[i].id + humans[i].points) % 8;
}
void Human::humanMove(World &world, Civilization &civilization, Food &food, Tree &tree, Stone &stone, Human &human)
{
    static const int offsets[8] = {
    -Config::sizeX - 1, // NW
    -Config::sizeX,     // N
    -Config::sizeX + 1, // NE
    -1,                 // W
    +1,                 // E
    +Config::sizeX - 1, // SW
    +Config::sizeX,     // S
    +Config::sizeX + 1  // SE
    };

    for(int i = 0; i < humans.size(); i++)
    {
        int dir = 0;
        int newIndex = -1;
        bool humanRemoved = false;

        if(humans[i].task == foodFinding)
        {
            if(humans[i].moves % (Config::vision + 1) == 0)
            {
                humans[i].targetIndex = humanFindResource(world, humans[i].index, ResourceType::food);
            }
            dir = humanMoveDecision(humans[i].index, humans[i].targetIndex, i);
            newIndex = humans[i].index + offsets[dir];

            if(humans[i].targetIndex != -1 && gotResource(newIndex, humans[i].targetIndex))
            {
                auto& targetTile = world.grid[humans[i].targetIndex];
                if(targetTile.food > 0) 
                {
                    targetTile.food = 0; 
                    civilization.resources.food++;
                    food.foodsCount--;
                    world.addToDirtyCells(humans[i].targetIndex);
                    humans[i].targetIndex = -1; 
                }
                else 
                {
                    humans[i].targetIndex = -1; 
                }
            }
        }
        else if(humans[i].task == treeFinding)
        {
            if(humans[i].moves % (Config::vision + 1) == 0)
            {
                humans[i].targetIndex = humanFindResource(world, humans[i].index, ResourceType::tree);
            }
            dir = humanMoveDecision(humans[i].index, humans[i].targetIndex, i);
            newIndex = humans[i].index + offsets[dir];
        
            if(humans[i].targetIndex != -1 && gotResource(newIndex, humans[i].targetIndex))
            {
                auto& targetTile = world.grid[humans[i].targetIndex];
                if(targetTile.treeHP > 0)
                {
                    targetTile.treeHP -= 1;
                    if(targetTile.treeHP <= 0)
                    {
                        civilization.resources.wood++;
                        tree.treesCount--;
                        targetTile.treeHP = 0; 
                        world.addToDirtyCells(humans[i].targetIndex);
                        humans[i].targetIndex = -1;
                    }
                }
                else 
                {
                    humans[i].targetIndex = -1;
                }
            }
        }
        else if(humans[i].task == stoneFinding)
        {
            if(humans[i].moves % (Config::vision + 1) == 0)
            {
                humans[i].targetIndex = humanFindResource(world, humans[i].index, ResourceType::stone);
            }
            dir = humanMoveDecision(humans[i].index, humans[i].targetIndex, i);
            newIndex = humans[i].index + offsets[dir];

           if(humans[i].targetIndex != -1 && gotResource(newIndex, humans[i].targetIndex))
            {
                auto& targetTile = world.grid[humans[i].targetIndex];
                if(targetTile.stoneHP > 0)
                {
                    targetTile.stoneHP -= 1;
                    if(targetTile.stoneHP <= 0)
                    {
                        targetTile.stoneHP = 0;
                        civilization.resources.stone++;
                        stone.stonesCount--;
                        world.addToDirtyCells(humans[i].targetIndex);
                        humans[i].targetIndex = -1;
                    }
                }
                else 
                {
                    humans[i].targetIndex = -1; 
                }
            }
        }
        else if(humans[i].task == building)
{
    if(humans[i].moves % (Config::vision + 1) == 0)
    {
        humans[i].targetIndex = humanFindResource(world, humans[i].index, ResourceType::construction);
    }
    
    dir = humanMoveDecision(humans[i].index, humans[i].targetIndex, i);
    
    if(dir < 0 || dir >= 8) dir = 0; 
    
    newIndex = humans[i].index + offsets[dir];

    if(humans[i].targetIndex != -1 && humans[i].targetIndex == newIndex && world.isValid(newIndex))
    {
        bool restartTargetIndex = false;
        if(world.grid[newIndex].construction.hitsNeeded > 0)
        {
            world.grid[newIndex].construction.hitsNeeded--;
            restartTargetIndex = true;
        }
        if(world.grid[newIndex].construction.hitsNeeded == 0)
        {
            world.grid[newIndex].construction.hitsNeeded = -1;
            civilization.endConstruction(world, human, humans[i].targetIndex, world.grid[newIndex].construction.result);
            restartTargetIndex = true;
        }
        if(restartTargetIndex) humans[i].targetIndex = -1;
    }
}
        else if(humans[i].task == goingToBuilding)
        {
            dir = humanMoveDecision(humans[i].index, humans[i].targetIndex, i);
            newIndex = humans[i].index + offsets[dir];
            if(humans[i].targetIndex == newIndex)
            {
                if(world.grid[humans[i].targetIndex].building == farm)
                {
                    civilization.realWorkers[farm]++;
                }
                else if(world.grid[humans[i].targetIndex].building == sawmill)
                {
                    civilization.realWorkers[sawmill]++;
                }
                else if(world.grid[humans[i].targetIndex].building == mine)
                {
                    civilization.realWorkers[mine]++;
                }
                world.grid[humans[i].index].humanIndex = 0;
                if (i < humans.size() - 1)
                {
                    humans[i] = humans.back();
                    world.grid[humans[i].index].humanIndex = i;
                }
                humans.pop_back();
                humanRemoved = true; 
            }
        }

        if (humanRemoved)
        {
            i--; 
            continue; 
        }
        if (world.isValid(newIndex) &&
            world.grid[newIndex].humanIndex <= 0 &&
            !(world.grid[newIndex].flags & Water))
        {
            int id = world.grid[humans[i].index].humanIndex;
            world.grid[humans[i].index].humanIndex = 0;

            humans[i].moves++;
            humans[i].index = newIndex;
            
            world.grid[humans[i].index].humanIndex = id;
        }
        else
        {
            humans[i].points -= (1 + (rand()%7));
        }
        
        if(humans[i].points <= 0) humans[i].points += 1000;
        
        if (rand() % 100 < Config::chanceToChangeDir) 
        {
            humans[i].points += (rand() % 2 == 0) ? 1 : -1;
        }
    }
}