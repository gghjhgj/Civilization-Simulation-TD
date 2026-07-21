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



enum Type
{
    HOUSE,
    FARM,
    SAWMILL,
    MINE,

    COUNT
};
inline BuildingType GetBuildingType(Type t) {
    switch(t) {
        case Type::HOUSE:   return BuildingType::House;
        case Type::FARM:    return BuildingType::Farm;
        case Type::SAWMILL: return BuildingType::Sawmill;
        case Type::MINE:    return BuildingType::Mine;
        default:            return BuildingType::None;
    }
}
inline Type GetTypeBuilding(BuildingType b) {
    switch(b) {
        case BuildingType::House:   return Type::HOUSE;
        case BuildingType::Farm:    return Type::FARM;
        case BuildingType::Sawmill: return Type::SAWMILL;
        case BuildingType::Mine:    return Type::MINE;
        default:            return Type::COUNT;
    }
}