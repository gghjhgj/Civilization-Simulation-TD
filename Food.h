#pragma once
#include "World.h"
#include "Config.h"
class Food
{
    public:
    int foodsCount = 0;
    void addFood(World &world, int index);
    void addFoodNoVec(World &world, int index);
    void createFood(World &world);
    void foodRespawn(World &world);
};