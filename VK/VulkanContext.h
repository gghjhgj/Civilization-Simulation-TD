#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <iostream>
#include <vector>

class VulkanContext
{
    public:

    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;

    VkQueue computeQueue = VK_NULL_HANDLE;
    uint32_t computeQueueFamilyIndex = 0;

    void init();
    void destroy();


    private:

    void createInstance();
    void pickPhysicalDevice();
    void createLogicalDevice();

};