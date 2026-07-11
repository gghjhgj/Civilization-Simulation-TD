#pragma once
#include "WorldData/World.h"
#include "Config.h"
class Stone
{
    public:
    int stonesCount = 0;
    void addStone(World &world, uint32_t x, uint32_t y);
    void createStone(World &world);
    void stoneRespawn(World &world);
};