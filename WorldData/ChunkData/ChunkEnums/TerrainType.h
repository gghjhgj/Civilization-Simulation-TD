#pragma once
#include <cstdint>
enum class TerrainType : uint32_t
{
    Unused1             = 0b000,
    Desert              = 0b001,
    Mountain            = 0b010,
    MountainWithStone   = 0b011,
    Land                = 0b100,
    LandWithFood        = 0b101,
    LandWithTree        = 0b110,
    Unused2             = 0b111
};