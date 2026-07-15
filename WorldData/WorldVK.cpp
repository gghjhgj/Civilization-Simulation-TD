#include "WorldVK.h"

WorldVK::WorldVK()
{

}

WorldVK::~WorldVK()
{
    std::cout << "destroying WorldVK..." << std::endl;

    worldStorageBuffer.destroy(vkContext.device);
    vkContext.destroy();

    std::cout << "WorldVK destroyed" << std::endl;
}

void WorldVK::init()
{
    std::cout << "initializing WorldVK..." << std::endl;

    vkContext.init();
    worldStorageBuffer.initStorage(vkContext, 10 * 1024 * 1024);
    
    std::cout << "WorldVK inited" << std::endl;
}

void WorldVK::uploadWorldGrid(const ChunkRegion* gridData)
{
    VkDeviceSize dataSize = WorldConfig::CHUNK_REGIONS_COUNT * sizeof(ChunkRegion);
    worldStorageBuffer.upload(gridData, dataSize);

    std::cout << "world grid sent to GPU. SIZE: " << dataSize << " bytes" << std::endl;
}