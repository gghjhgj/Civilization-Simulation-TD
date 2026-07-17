#include "HumanVK.h"

const uint32_t MAX_HUMANS_CAPACITY = 100000;

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
        deadPipeline.destroy(vkContext->device);
    }

    std::cout << "HumanVK destroyed." << std::endl;
}

void HumanVK::init(VulkanContext &context)
{
    std::cout << "initializing HumanVK" << std::endl;

    vkContext = &context;

    foodCollectorsBuf.initStorage(*vkContext, MAX_HUMANS_CAPACITY * sizeof(HumanBase));
    woodCollectorsBuf.initStorage(*vkContext, MAX_HUMANS_CAPACITY * sizeof(HumanBase));
    stoneCollectorsBuf.initStorage(*vkContext, MAX_HUMANS_CAPACITY * sizeof(HumanBase));
    buildersBuf.initStorage(*vkContext, MAX_HUMANS_CAPACITY * sizeof(HumanBase));
    assignedBuf.initStorage(*vkContext, MAX_HUMANS_CAPACITY * sizeof(HumanBase));
    deadBuf.initStorage(*vkContext, MAX_HUMANS_CAPACITY * sizeof(DeadHuman));

    // Pipeline'y shaderów
    uint32_t pushConstantSize = 16; 
    uint32_t bindingCount = 2; 

    foodCollectorPipeline.init(*vkContext, "shaders/humans/food_collector.spv", pushConstantSize, bindingCount);
    woodCollectorPipeline.init(*vkContext, "shaders/humans/wood_collector.spv", pushConstantSize, bindingCount);
    stoneCollectorPipeline.init(*vkContext, "shaders/humans/stone_collector.spv", pushConstantSize, bindingCount);
    builderPipeline.init(*vkContext, "shaders/humans/builder.spv", pushConstantSize, bindingCount);
    assignedPipeline.init(*vkContext, "shaders/humans/assigned.spv", pushConstantSize, bindingCount);
    deadPipeline.init(*vkContext, "shaders/humans/dead.spv", pushConstantSize, bindingCount);

    std::cout << "HumanVK inited" << std::endl;
}