#pragma once
#include "WorldData/World.h"
#include "Config.h"
class Food
{
    public:
    int foodsCount = 0;
    void addFood(World &world, RendererSFML &renderer, uint16_t x, uint16_t y);
    void createFood(World &world, RendererSFML &renderer);
    void foodRespawn(World &world, RendererSFML &renderer);
};