#pragma once
#include "WorldData/World.h"
#include "RendererSFML.h"
#include "Config.h"
class Stone
{
    public:
    int stonesCount = 0;
    void addStone(World &world, RendererSFML &renderer, uint16_t x, uint16_t y);
    void createStone(World &world, RendererSFML &renderer);
    void stoneRespawn(World &world, RendererSFML &renderer);
};