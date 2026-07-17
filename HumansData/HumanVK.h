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

    void uploadFoodCollectors(const std::vector<HumanBase>& foodCollectors);
    void uploadWoodCollectors(const std::vector<HumanBase>& woodCollectors);
    void uploadStoneCollectors(const std::vector<HumanBase>& stoneCollectors);
    void uploadBuilders(const std::vector<HumanBase>& builders);
    void uploadAssigned(const std::vector<HumanBase>& assigned);
    void uploadDead(const std::vector<DeadHuman>& dead);

    void downloadFoodCollectors(std::vector<HumanBase>& foodCollectors);
    void downloadWoodCollectors(std::vector<HumanBase>& woodCollectors);
    void downloadStoneCollectors(std::vector<HumanBase>& stoneCollectors);
    void downloadBuilders(std::vector<HumanBase>& builders);
    void downloadAssigned(std::vector<HumanBase>& assigned);
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