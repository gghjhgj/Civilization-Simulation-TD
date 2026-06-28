#pragma once
#include "World.h"
class Stone
{
    public:
    int stonesCount = 0;
    void addStone(World &world, int index);
    void addStoneNoVec(World &world, int index);
    void createStone(World &world);
    void stoneRespawn(World &world);
};