#pragma once

#include "Chunk.h"
#include "ChunkRegionConfig.h"

struct ChunkRegion
{
    Chunk chunks[ChunkRegionConfig::CHUNKS_COUNT];

    Chunk& getChunk(uint32_t x, uint32_t y)
    {
        return chunks[y * ChunkRegionConfig::CHUNK_REGION_SIZE + x];
    }
};

