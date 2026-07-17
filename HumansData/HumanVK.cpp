#include "HumanVK.h"

const uint32_t MAX_HUMANS_CAPACITY = 100000 + Config::humanCount;

HumanVK::HumanVK()
{

}

HumanVK::~HumanVK()
{
    std::cout << "destroying humanVK" << std::endl;

    if (vkContext) 
    {
        foodCollectorsBuf.destroy(vkContext->device);
        woodCollectorsBuf.destroy(vkContext->device);
        stoneCollectorsBuf.destroy(vkContext->device);
        buildersBuf.destroy(vkContext->device);
        assignedBuf.destroy(vkContext->device);
        deadBuf.destroy(vkContext->device);

        foodCollectorPipeline.destroy(vkContext->device);
        woodCollectorPipeline.destroy(vkContext->device);
        stoneCollectorPipeline.destroy(vkContext->device);
        builderPipeline.destroy(vkContext->device);
        assignedPipeline.destroy(vkContext->device);
    }

    std::cout << "HumanVK destroyed." << std::endl;
}

void HumanVK::init(
    VulkanContext &context, 
    const VulkanBuffer& worldBuffer, 
    const VulkanBuffer &statsBuffer,
    const VulkanBuffer &dirtyBuffer
)
{
    std::cout << "initializing HumanVK" << std::endl;

    vkContext = &context;

    foodCollectorsBuf.initStorage(*vkContext, MAX_HUMANS_CAPACITY * sizeof(HumanBase));
    woodCollectorsBuf.initStorage(*vkContext, MAX_HUMANS_CAPACITY * sizeof(HumanBase));
    stoneCollectorsBuf.initStorage(*vkContext, MAX_HUMANS_CAPACITY * sizeof(HumanBase));
    buildersBuf.initStorage(*vkContext, MAX_HUMANS_CAPACITY * sizeof(HumanBase));
    assignedBuf.initStorage(*vkContext, MAX_HUMANS_CAPACITY * sizeof(HumanBase));
    deadBuf.initStorage(*vkContext, MAX_HUMANS_CAPACITY * sizeof(DeadHuman));

    uint32_t pushConstantSize = sizeof(PushConstants); 
    uint32_t workGroupSizeX = 64;

    auto initAll = [&](VulkanComputePipeline& pipe, const std::string& path) {
        pipe.init(*vkContext, path, pushConstantSize, workGroupSizeX, 4);
    };

    initAll(foodCollectorPipeline, "shaders/humans/food_collector.spv");
    initAll(woodCollectorPipeline, "shaders/humans/wood_collector.spv");
    initAll(stoneCollectorPipeline, "shaders/humans/stone_collector.spv");
    initAll(builderPipeline,       "shaders/humans/builder.spv");
    initAll(assignedPipeline,      "shaders/humans/assigned.spv");

    auto bindAll = [&](VulkanComputePipeline& pipe, VulkanBuffer& buf) {
        pipe.bindBuffers(*vkContext, { buf, worldBuffer, statsBuffer, dirtyBuffer });
    };

    bindAll(foodCollectorPipeline, foodCollectorsBuf);
    bindAll(woodCollectorPipeline, woodCollectorsBuf);
    bindAll(stoneCollectorPipeline, stoneCollectorsBuf);
    bindAll(builderPipeline,       buildersBuf);
    bindAll(assignedPipeline,      assignedBuf);

    std::cout << "HumanVK inited with stats buffer" << std::endl;
}

void HumanVK::uploadHumanType(const std::vector<HumanBase>& cpuVector, VulkanBuffer& gpuBuffer)
{
    uint32_t humanCount = static_cast<uint32_t>(cpuVector.size());
    if (humanCount == 0) return;

    gpuBuffer.upload(cpuVector.data(), humanCount * sizeof(HumanBase));
    //std::cout << "humanType sent to gpu " << humanCount * sizeof(HumanBase) << " bytes" << std::endl;
}

void HumanVK::dispatchHumanType(uint32_t humanCount, VulkanBuffer& gpuBuffer, VulkanComputePipeline& pipeline, const VulkanBuffer& worldBuffer)
{
    if (humanCount == 0) return;

    PushConstants push;
    push.humanCount = humanCount;

    uint32_t groupCountX = (humanCount + 63) / 64;

    pipeline.dispatch(
        *vkContext, 
        groupCountX, 1, 1, 
        &push, sizeof(PushConstants)
    );
}

void HumanVK::downloadHumanType(std::vector<HumanBase>& cpuVector, VulkanBuffer& gpuBuffer)
{
    uint32_t humanCount = static_cast<uint32_t>(cpuVector.size());
    if (humanCount == 0) return;

    gpuBuffer.download(cpuVector.data(), humanCount * sizeof(HumanBase));
    //std::cout << "humanType Downloaded From GPU size: " << humanCount * sizeof(HumanBase) << " bytes" << std::endl;
}

void HumanVK::uploadDead(const std::vector<DeadHuman>& cpuVector)
{
    uint32_t deadCount = static_cast<uint32_t>(cpuVector.size());
    if (deadCount == 0) return;

    deadBuf.upload(cpuVector.data(), deadCount * sizeof(DeadHuman));
}

void HumanVK::downloadDead(std::vector<DeadHuman>& cpuVector)
{
    uint32_t deadCount = static_cast<uint32_t>(cpuVector.size());
    if (deadCount == 0) return;

    deadBuf.download(cpuVector.data(), deadCount * sizeof(DeadHuman));
}

void HumanVK::uploadAll(const Human &human)
{
    uploadHumanType(human.foodCollectors, foodCollectorsBuf);
    uploadHumanType(human.woodCollectors, woodCollectorsBuf);
    uploadHumanType(human.stoneCollectors, stoneCollectorsBuf);
    uploadHumanType(human.builders, buildersBuf);
    uploadHumanType(human.assigned, assignedBuf);
    uploadDead(human.dead);
}

void HumanVK::dispatchAll(const Human &human, const VulkanBuffer& worldBuffer)
{
    dispatchHumanType(human.foodCollectors.size(), foodCollectorsBuf, foodCollectorPipeline, worldBuffer);
    dispatchHumanType(human.woodCollectors.size(), woodCollectorsBuf, woodCollectorPipeline, worldBuffer);
    dispatchHumanType(human.stoneCollectors.size(), stoneCollectorsBuf, stoneCollectorPipeline, worldBuffer);
    dispatchHumanType(human.builders.size(),        buildersBuf,        builderPipeline,        worldBuffer);
    dispatchHumanType(human.assigned.size(),       assignedBuf,       assignedPipeline,       worldBuffer);
}

void HumanVK::downloadAll(Human &human)
{
    downloadHumanType(human.foodCollectors, foodCollectorsBuf);
    downloadHumanType(human.woodCollectors, woodCollectorsBuf);
    downloadHumanType(human.stoneCollectors, stoneCollectorsBuf);
    downloadHumanType(human.builders,        buildersBuf);
    downloadHumanType(human.assigned,       assignedBuf);

    downloadDead(human.dead);
}