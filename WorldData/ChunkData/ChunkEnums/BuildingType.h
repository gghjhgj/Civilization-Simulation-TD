#pragma once
#include <cstdint>

enum class BuildingType : uint32_t
{
    None            = 0b000,
    House           = 0b001,
    Farm            = 0b010,
    Sawmill         = 0b011,
    Mine            = 0b100,
    Unused1         = 0b101,
    Unused2         = 0b110,
    Unused3         = 0b111
};