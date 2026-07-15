#pragma once

#include <cstdint>
#include "Config.h"
#include "ChunkData/ChunkConfig.h"
#include "ChunkData/ChunkRegionConfig.h"

namespace WorldConfig
{
    constexpr uint32_t CHUNKS_X = (Config::sizeX + ChunkConfig::CHUNK_SIZE - 1) / ChunkConfig::CHUNK_SIZE;
    constexpr uint32_t CHUNKS_Y = (Config::sizeY + ChunkConfig::CHUNK_SIZE - 1) / ChunkConfig::CHUNK_SIZE;
    constexpr uint32_t CHUNKS_COUNT = CHUNKS_X * CHUNKS_Y;

    constexpr uint32_t CHUNK_REGIONS_X = (CHUNKS_X + ChunkRegionConfig::CHUNK_REGION_SIZE - 1) / ChunkRegionConfig::CHUNK_REGION_SIZE;
    constexpr uint32_t CHUNK_REGIONS_Y = (CHUNKS_Y + ChunkRegionConfig::CHUNK_REGION_SIZE - 1) / ChunkRegionConfig::CHUNK_REGION_SIZE;
    constexpr uint32_t CHUNK_REGIONS_COUNT = CHUNK_REGIONS_X * CHUNK_REGIONS_Y;
}