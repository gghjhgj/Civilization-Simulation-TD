#pragma once 

#include "VK/VulkanBuffer.h"
#include "VK/VulkanContext.h"
#include "VK/VulkanComputePipeline.h"
#include "EndStats.h"

#include "Food.h"
#include "Stone.h"
#include "Tree.h"
#include "Civilization.h"

class StatsManager
{
    public:

    VulkanBuffer statsBuffer;

    void init(VulkanContext &context);
    void destroy(VkDevice device);
    void reset(VulkanContext &context);
    void deltaStats(
        VulkanContext &context,
        Food &food, 
        Stone &stone,
        Tree &tree,
        Civilization &civ
    );
    void download(VulkanContext &context, EndStats& outStats);

};