#pragma once

#include "VK/VulkanContext.h"
#include "VK/VulkanBuffer.h"
#include "VK/VulkanComputePipeline.h"

#include "Human.h"

#include <vector>
#include <cstring>
#include <iostream>



class HumanVK
{
    public:

    HumanVK();
    ~HumanVK();

    void init(VulkanContext& context);

    void uploadFoodCollectors(const std::vector<CollectorHuman>& foodCollectors);
    void uploadWoodCollectors(const std::vector<CollectorHuman>& woodCollectors);
    void uploadStoneCollectors(const std::vector<CollectorHuman>& stoneCollectors);
    void uploadBuilders(const std::vector<HumanBuilder>& builders);
    void uploadAssigned(const std::vector<HumanAssigned>& assigned);
    void uploadDead(const std::vector<DeadHuman>& dead);

    void downloadFoodCollectors(std::vector<CollectorHuman>& foodCollectors);
    void downloadWoodCollectors(std::vector<CollectorHuman>& woodCollectors);
    void downloadStoneCollectors(std::vector<CollectorHuman>& stoneCollectors);
    void downloadBuilders(std::vector<HumanBuilder>& builders);
    void downloadAssigned(std::vector<HumanAssigned>& assigned);
    void downloadDead(std::vector<DeadHuman>& dead);

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
    VulkanComputePipeline deadPipeline;
};