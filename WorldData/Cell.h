#pragma once

#include "BuildingsType.h"

#include <vector>
#include <cstdint>
#include <cassert>

enum CellFlags
    {
        Water = 1 << 0,
        Land = 1 << 1,
        Sand = 1 << 2,
        Mountain = 1 << 3
    };
    struct Construction
    {
        int hitsNeeded;
        BuildingsType result;
    };
    struct Cell
    {
        uint8_t flags;

        uint8_t food;
        uint8_t treeHP;
        uint8_t stoneHP;

        uint8_t civilizationPlace;

        int humanIndex = -1;//nie ma

        int civZone = 0;
        BuildingsType building;
        Construction construction;
    };
