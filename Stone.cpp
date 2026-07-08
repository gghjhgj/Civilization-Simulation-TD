#include "Stone.h"

void Stone::addStone(World &world, int index)
{
    world.grid[index].stoneHP += Config::StoneHP;
    world.stones.push_back(index);
    world.addToDirtyCells(index);
    stonesCount++;
}
void Stone::addStoneNoVec(World &world, int index)
{
    world.grid[index].stoneHP += Config::StoneHP;
    world.addToDirtyCells(index);
    stonesCount++;
}

void Stone::createStone(World &world)
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(0, world.mountains.size() - 1);

    for(int i = 0; i < Config::stoneCount; i++)
    {
        int id;
        int maxTries = Config::maxStoneSpawnTries;
        int tries = 0;
        do
        {
            id = world.mountains[dist(rng)]; 
            tries++;
            if(tries > maxTries ) break;
        } while (!world.isEmpty(id));
         if(tries <= maxTries)
        addStone(world, id);
    }
}

void Stone::stoneRespawn(World &world)
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(0, world.mountains.size() - 1);
    if(stonesCount < Config::maxStone)
    for(int i = 0; i < Config::StoneRespawn; i++)
    {
        int id;
        int tries = 0;
        int maxTries = Config::maxStoneSpawnTries;
        do
        {
            id = world.mountains[dist(rng)]; 
            tries++;
            if(tries > maxTries ) break;
        } while (!world.isEmpty(id) || world.grid[id].civZone > 0);
        if(tries <= maxTries)
        addStoneNoVec(world, id);
    }
}