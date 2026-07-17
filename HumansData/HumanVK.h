#pragma once

#include "VK/VulkanContext.h"
#include "VK/VulkanBuffer.h"
#include "VK/VulkanComputePipeline.h"
#include "StatsManager/StatsManagerVK.h"
#include "DirtyCellsManager/DirtyCellsManager.h"
#include "Human.h"

#include <vector>
#include <cstring>
#include <iostream>


struct PushConstants
{
    uint32_t humanCount;
    uint32_t worldRegionsX = WorldConfig::CHUNK_REGIONS_X;
    uint32_t chunksCount = ChunkRegionConfig::CHUNKS_COUNT;
    uint32_t mapWidth = Config::sizeX;
    uint32_t mapHeight = Config::sizeY;
};

class HumanVK
{
    public:

    HumanVK();
    ~HumanVK();

    VulkanBuffer statsBuffer;
    void init(
        VulkanContext &context, 
        const VulkanBuffer& worldBuffer, 
        const VulkanBuffer &statsBuffer,
        const VulkanBuffer &dirtyBuffer
    );

    void uploadHumanType(const std::vector<HumanBase>& cpuVector, VulkanBuffer& gpuBuffer);
    void dispatchHumanType(uint32_t humanCount, VulkanBuffer& gpuBuffer, VulkanComputePipeline& pipeline, const VulkanBuffer& worldBuffer);
    void downloadHumanType(std::vector<HumanBase>& cpuVector, VulkanBuffer& gpuBuffer);
    void uploadDead(const std::vector<DeadHuman>& cpuVector);
    void downloadDead(std::vector<DeadHuman>& cpuVector);
    void uploadAll(const Human &human);
    void dispatchAll(const Human &human, const VulkanBuffer& worldBuffer);
    void downloadAll(Human &human);

    private:

    VulkanContext* vkContext = nullptr;

    VulkanBuffer foodCollectorsBuf;
    VulkanBuffer woodCollectorsBuf;
    VulkanBuffer stoneCollectorsBuf;
    VulkanBuffer buildersBuf;
    VulkanBuffer assignedBuf;
    VulkanBuffer deadBuf;

    VulkanComputePipeline foodCollectorPipeline;
    VulkanComputePipeline woodCollectorPipeline;
    VulkanComputePipeline stoneCollectorPipeline;
    VulkanComputePipeline builderPipeline;
    VulkanComputePipeline assignedPipeline;
};