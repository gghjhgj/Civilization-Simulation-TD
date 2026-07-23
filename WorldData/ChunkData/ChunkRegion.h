#pragma once

#include "Chunk.h"

namespace ChunkRegionConfig
{
    constexpr uint32_t CHUNK_REGION_SIZE = 4;
    constexpr uint32_t CHUNKS_COUNT = CHUNK_REGION_SIZE*CHUNK_REGION_SIZE;
}
struct ChunkRegion
{
    Chunk chunks[ChunkRegionConfig::CHUNKS_COUNT];

    Chunk& getChunk(uint16_t x, uint16_t y)
    {
        return chunks[y * ChunkRegionConfig::CHUNK_REGION_SIZE + x];
    }
};

