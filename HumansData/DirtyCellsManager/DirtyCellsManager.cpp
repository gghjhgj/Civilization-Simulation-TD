#include "DirtyCellsManager.h"

void DirtyCellsManager::init(VulkanContext& context) 
{
    dirtyBuffer.initStorage(context, sizeof(DirtyCells));
}

void DirtyCellsManager::destroy(VkDevice device) 
{
    dirtyBuffer.destroy(device);
}

void DirtyCellsManager::reset(VulkanContext& context) {
    uint32_t zero = 0;
    dirtyBuffer.upload(&zero, sizeof(uint32_t));
}

void DirtyCellsManager::sync(VulkanContext& context, std::vector<XY>& cpuVector)
{
    DirtyCells dirty;
    dirtyBuffer.download(&dirty, sizeof(DirtyCells));

    uint32_t count = std::min(dirty.count, MAX_DIRTY_CELLS);
    if(count == MAX_DIRTY_CELLS) std::cout << "max dirty cells broke" << std::endl;

    for(uint32_t i = 0; i < count; i++)
    {
        cpuVector.push_back(dirty.cells[i]);
    }
    reset(context);
}