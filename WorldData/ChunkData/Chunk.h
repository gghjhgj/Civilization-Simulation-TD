#pragma once
#include "ChunkEnums/TerrainType.h"
#include "ChunkEnums/BuildingType.h"
#include "ChunkEnums/ChunkFlag.h"

#include <cstdint>
#include <cassert>
namespace ChunkConfig
{
    constexpr uint16_t CHUNK_SIZE = 3;//(3x3=)
    constexpr uint16_t CELL_COUNT = CHUNK_SIZE * CHUNK_SIZE;//(=9)
    constexpr uint32_t BITS_PER_CELL = 3;
    constexpr uint32_t TERRAIN_BITS = CELL_COUNT * BITS_PER_CELL;
    constexpr uint32_t CELL_MASK = 0x7;

    constexpr uint32_t BUILDING_SHIFT = TERRAIN_BITS;
    constexpr uint32_t BUILDING_MASK = 0x7;

    constexpr uint32_t FLAG_SHIFT = TERRAIN_BITS + 3;
}

struct Chunk
{
    uint32_t data = 0;

    void setCell(uint32_t index, TerrainType type)
    {
        assert(index < ChunkConfig::CELL_COUNT);

        uint32_t shift = index * ChunkConfig::BITS_PER_CELL;
        uint32_t value = static_cast<uint32_t>(type);

        data &= ~(ChunkConfig::CELL_MASK << shift);
        data |= (value & ChunkConfig::CELL_MASK) << shift;
    }
    TerrainType getCell(uint32_t index) const
    {
        assert(index < ChunkConfig::CELL_COUNT);

        uint32_t shift = index * ChunkConfig::BITS_PER_CELL;
        uint32_t value = (data >> shift) & ChunkConfig::CELL_MASK;
        return static_cast<TerrainType>(value);
    }
    
    void setBuilding(BuildingType type)
    {
        uint32_t value = static_cast<uint32_t>(type);
        data &= ~(ChunkConfig::BUILDING_MASK << ChunkConfig::BUILDING_SHIFT);
        data |= (value & ChunkConfig::BUILDING_MASK) << ChunkConfig::BUILDING_SHIFT;
    }
    BuildingType getBuilding() const
    {
        uint32_t value = (data >> ChunkConfig::BUILDING_SHIFT) & ChunkConfig::BUILDING_MASK;
        return static_cast<BuildingType>(value);
    }


    void setFlag(ChunkFlag flag)
    {
        data |= static_cast<uint32_t>(flag) << ChunkConfig::FLAG_SHIFT;
    }
    void clearFlag(ChunkFlag flag)
    {
        data &= ~(static_cast<uint32_t>(flag) << ChunkConfig::FLAG_SHIFT);
    }
    bool hasFlag(ChunkFlag flag) const
    {
        return (data & (static_cast<uint32_t>(flag) << ChunkConfig::FLAG_SHIFT)) != 0;
    }
};