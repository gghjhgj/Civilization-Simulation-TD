#pragma once

#include <cstdint>

namespace ChunkRegionConfig
{
    constexpr uint32_t CHUNK_REGION_SIZE = 4;
    constexpr uint32_t CHUNKS_COUNT = CHUNK_REGION_SIZE*CHUNK_REGION_SIZE;
}