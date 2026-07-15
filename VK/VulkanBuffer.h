#pragma once

#include <vulkan/vulkan.h>

#include <cstring>
#include <stdexcept>

class VulkanContext;

class VulkanBuffer
{
    public:

    VkBuffer handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    void* mappedData = nullptr;
    VkDeviceSize size = 0;

    void init(const VulkanContext& context, 
              VkDeviceSize size, 
              VkBufferUsageFlags usage, 
              VkMemoryPropertyFlags properties,
              bool mapMemory = false);

    void upload(const void* data, VkDeviceSize dataSize);
    void destroy(VkDevice device);

    void initStorage(const VulkanContext& context, VkDeviceSize size, bool mapMemory = true)
    {
        init(context, 
            size, 
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            mapMemory
        );
    }

    void initUniform(const VulkanContext& context, VkDeviceSize size, bool mapMemory = true)
    {
        init(context,
            size,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            mapMemory
        );
    }


    private:

    uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
};