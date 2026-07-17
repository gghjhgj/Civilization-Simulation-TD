#include "WorldVK.h"

WorldVK::WorldVK()
{

}

WorldVK::~WorldVK()
{
    std::cout << "destroying WorldVK..." << std::endl;

   if (vkContext) 
    {
        worldStorageBuffer.destroy(vkContext->device);
        terrainGeneratorPipeline.destroy(vkContext->device);
    }

    std::cout << "WorldVK destroyed" << std::endl;
}

void WorldVK::init(VulkanContext &context)
{
    std::cout << "initializing WorldVK..." << std::endl;

    vkContext = &context;
    uint32_t totalRegions = WORLD_REGIONS_X * WORLD_REGIONS_Y;
    uint32_t bufferSize = totalRegions * sizeof(ChunkRegion);

    worldStorageBuffer.initStorage(*vkContext, bufferSize);
    terrainGeneratorPipeline.init(
        *vkContext, 
        "shaders/world/terrain_gen.spv", 
        sizeof(WorldConfigPushConstant),
        ChunkRegionConfig::CHUNKS_COUNT,
        1
        );
    std::cout << "WorldVK inited" << std::endl;
}

void WorldVK::debugCheck()
{
    std::cout << "DEBUG CHECK ON" << std::endl;

    WorldConfigPushConstant config;

    terrainGeneratorPipeline.bindBuffers(*vkContext, {worldStorageBuffer});
    terrainGeneratorPipeline.dispatch(
        *vkContext, 
        WORLD_REGIONS_X, 
        WORLD_REGIONS_Y,
        1,
        &config,
        sizeof(config)
    );
    vkQueueWaitIdle(vkContext->computeQueue);//tego nie chcemy
}

void WorldVK::uploadWorldGrid(const ChunkRegion* gridData)
{
    VkDeviceSize dataSize = WorldConfig::CHUNK_REGIONS_COUNT * sizeof(ChunkRegion);
    worldStorageBuffer.upload(gridData, dataSize);

    std::cout << "world grid sent to GPU. SIZE: " << dataSize << " bytes" << std::endl;
}

void WorldVK::downloadWorldGrid(ChunkRegion* gridData)
{
    VkDeviceSize dataSize = WorldConfig::CHUNK_REGIONS_COUNT * sizeof(ChunkRegion);
    worldStorageBuffer.download(gridData, dataSize);

    std::cout << "world grid downloaded from GPU. SIZE: " << dataSize << " bytes" << std::endl;
}