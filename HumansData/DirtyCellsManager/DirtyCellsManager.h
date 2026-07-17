#pragma once

#include "DirtyCells.h"
#include "VK/VulkanBuffer.h"
#include "VK/VulkanContext.h"
#include <vector>
#include <algorithm>
#include <iostream>

class DirtyCellsManager
{
    public:

    VulkanBuffer dirtyBuffer;

    void init(VulkanContext &context);
    void destroy(VkDevice device);
    void reset(VulkanContext &context);
    void sync(VulkanContext &context, std::vector<XY>& cpuVector);
};