#pragma once

#include <cstdint>
#include <assert.h>
namespace ChunkConfig
{
    constexpr uint32_t CHUNK_SIZE = 3;//(3x3=)
    constexpr uint32_t CELL_COUNT = CHUNK_SIZE * CHUNK_SIZE;//(=9)
    constexpr uint32_t BITS_PER_CELL = 3;
    constexpr uint32_t TERRAIN_BITS = CELL_COUNT * BITS_PER_CELL;
    constexpr uint32_t CELL_MASK = 0x7;
}
enum class TerrainType : uint32_t
{
    Water               = 0b000,
    Desert              = 0b001,
    Mountain            = 0b010,
    MountainWithStone   = 0b011,
    Land                = 0b100,
    LandWithFood        = 0b101,
    LandWithTree        = 0b110,
    Empty               = 0b111
};
enum class ChunkFlag : uint32_t
{
    CivZone     = 1 << 0,
    Buildings   = 1 << 1,
    Empty1      = 1 << 2,
    Empty2      = 1 << 3,
    Empty3      = 1 << 4
};
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

    void setFlag(ChunkFlag flag)
    {
        data |= static_cast<uint32_t>(flag) << ChunkConfig::TERRAIN_BITS;
    }
    void clearFlag(ChunkFlag flag)
    {
        data &= ~(static_cast<uint32_t>(flag) << ChunkConfig::TERRAIN_BITS);
    }
    bool hasFlag(ChunkFlag flag) const
    {
        return (data & (static_cast<uint32_t>(flag) << ChunkConfig::TERRAIN_BITS)) != 0;
    }
};