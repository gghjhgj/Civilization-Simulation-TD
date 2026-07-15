#pragma once 

#include <cstdint>
namespace ChunkConfig
{
    constexpr uint32_t CHUNK_SIZE = 3;//(3x3=)
    constexpr uint32_t CELL_COUNT = CHUNK_SIZE * CHUNK_SIZE;//(=9)
    constexpr uint32_t BITS_PER_CELL = 3;
    constexpr uint32_t TERRAIN_BITS = CELL_COUNT * BITS_PER_CELL;
    constexpr uint32_t CELL_MASK = 0x7;

    constexpr uint32_t BUILDING_SHIFT = TERRAIN_BITS;
    constexpr uint32_t BUILDING_MASK = 0x7;

    constexpr uint32_t FLAG_SHIFT = TERRAIN_BITS + 3;
}