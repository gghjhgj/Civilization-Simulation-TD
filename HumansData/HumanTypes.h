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
    std::vector<uint16_t> posX;
    std::vector<uint16_t> posY;
    std::vector<uint16_t> targetX;
    std::vector<uint16_t> targetY;
    std::vector<uint8_t> points;
    std::vector<uint8_t> moves;
    std::vector<BuildingType> targetBuilding;
};