#pragma once

#include "VK/VulkanContext.h"
#include "VK/VulkanBuffer.h"
#include "VK/VulkanComputePipeline.h"

#include "ChunkData/ChunkRegionConfig.h"
#include "World.h"

#include <vector>
#include <cstring>
#include <iostream>

struct WorldConfigPushConstant
{
    uint32_t worldRegionsX = WorldConfig::CHUNK_REGIONS_X;
    uint32_t chunksCount = ChunkRegionConfig::CHUNKS_COUNT;
};

class WorldVK
{
    public:
    WorldVK();
    ~WorldVK();

    void init();
    void debugCheck();
    void uploadWorldGrid(const ChunkRegion* gridData);
    void downloadWorldGrid(ChunkRegion* gridData);

    using Chunk = uint32_t;
    struct Region
    {
        Chunk chunks[ChunkRegionConfig::CHUNKS_COUNT];
    };
    const uint32_t WORLD_REGIONS_X = WorldConfig::CHUNK_REGIONS_X;
    const uint32_t WORLD_REGIONS_Y = WorldConfig::CHUNK_REGIONS_Y;
    const uint32_t WORLD_REGIONS_COUNT = WorldConfig::CHUNK_REGIONS_COUNT;

    private:

    VulkanContext vkContext;
    VulkanBuffer worldStorageBuffer;
    VulkanComputePipeline terrainGeneratorPipeline;

};