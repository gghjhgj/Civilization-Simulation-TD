#pragma once

#include "XY.h"
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
    uint8_t points = 100;
    uint8_t moves = 0;
    uint16_t padding;
};

struct CollectorHuman : public HumanBase
{
    using DataType = BuildingType;
    BuildingType targetBuilding;
};

struct HumanBuilder : public HumanBase
{
    using DataType = BuildingType;
    BuildingType targetBuilding;
};

struct HumanAssigned : public HumanBase
{
    using DataType = BuildingType;
    BuildingType targetBuilding;
};

struct DeadHuman
{
    XY oldPos;
    XY pos;
};