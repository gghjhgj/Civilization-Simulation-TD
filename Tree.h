#pragma once
#include "WorldData/World.h"
#include "Config.h"
class Tree
{
    public:
    int treesCount = 0;
    struct LastTree
    {
        uint32_t x = 0;
        uint32_t y = 0;
    };
    LastTree last;
    void createSeed(World &world, int &tr, std::mt19937 &rng);
    void addTree(World &world, uint32_t x, uint32_t y);
    void createTree(World &world);
    void treeRespawn(World &world);
};