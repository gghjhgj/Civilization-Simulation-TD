#pragma once

#include "VK/VulkanContext.h"
#include "VK/VulkanBuffer.h"
#include "VK/VulkanComputePipeline.h"

#include "ChunkData/ChunkRegion.h"
#include "World.h"

#include <vector>
#include <cstring>
#include <iostream>

class WorldVK
{
    public:
    WorldVK();
    ~WorldVK();

    void init();
    void uploadWorldGrid(const ChunkRegion* gridData);

    private:

    VulkanContext vkContext;
    VulkanBuffer worldStorageBuffer;

};