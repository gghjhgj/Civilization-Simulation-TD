#include "Food.h"

void Food::addFood(World &world, int index)
{
    world.grid[index].food++;
    world.foods.push_back(index);
    world.addToDirtyCells(index);
    foodsCount++;
}

void Food::addFoodNoVec(World &world, int index)
{
    world.grid[index].food++;
    world.addToDirtyCells(index);
    foodsCount++;
}

void Food::createFood(World &world)
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(0, world.lands.size() - 1);

    for(int i = 0; i < Config::foodCount; i++)
    {
        int id;
        int tr = 0;
        int maxTries = Config::maxFoodSpawnTries;
        do
        {
            id = world.lands[dist(rng)]; 
            tr++;
            if(tr > maxTries) break;
        } while (!world.isEmpty(id));
        if(tr <= maxTries) addFood(world, id);
    }
}

void Food::foodRespawn(World &world)
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(0, world.lands.size() - 1);
    if(Food::foodsCount < Config::maxFood)
    for(int i = 0; i < Config::foodRespawn; i++)
    {
        int id;
        int tr = 0;
        int maxTries = Config::maxFoodSpawnTries;
        do
        {
            id = world.lands[dist(rng)]; 
            tr++;
            if(tr > maxTries) break;
        } while (!world.isEmpty(id) || world.grid[id].civZone > 0);
        if(tr <= maxTries) addFoodNoVec(world, id);
    }
}