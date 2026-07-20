#pragma once

#include "../XY/XY.h"
#include "WorldData/ChunkData/ChunkEnums/TerrainType.h"
#include "WorldData/ChunkData/ChunkEnums/BuildingType.h"

#include <vector>


enum class HumanType
{
    FoodCollector,
    WoodCollector,
    StoneCollector,
    Builder,
    Assigned,

    COUNT
};
struct HumanBase
{
    XY pos;
    XY oldPos;
    XY targetPos = { UINT32_MAX, UINT32_MAX };
    uint16_t points = 100;
    uint16_t moves = 0;
    BuildingType targetBuilding;
};

struct DeadHuman
{
    XY oldPos;
    XY pos;
};