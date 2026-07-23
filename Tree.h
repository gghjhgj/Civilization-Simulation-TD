#pragma once
#include "WorldData/World.h"
#include "RendererSFML.h"
#include "Config.h"
class Tree
{
    public:
    int treesCount = 0;
    struct LastTree
    {
        uint16_t x = 0;
        uint16_t y = 0;
    };
    LastTree last;
    void createSeed(World &world, RendererSFML &renderer, int &tr, std::mt19937 &rng);
    void addTree(World &world, RendererSFML &renderer, uint16_t x, uint16_t y);
    void createTree(World &world, RendererSFML &renderer);
    void treeRespawn(World &world, RendererSFML &renderer);
};