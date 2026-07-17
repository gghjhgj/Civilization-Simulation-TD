#include "StatsManagerVK.h"

void StatsManager::init(VulkanContext& context) 
{
    statsBuffer.initStorage(context, sizeof(EndStats));
}

void StatsManager::destroy(VkDevice device)
{
    statsBuffer.destroy(device);
}

void StatsManager::reset(VulkanContext& context)
{
    EndStats zeroStats{};
    statsBuffer.upload(&zeroStats, sizeof(EndStats));
}

void StatsManager::deltaStats(
    VulkanContext &context,
    Food &food, 
    Stone &stone,
    Tree &tree,
    Civilization &civ
    )
{
    EndStats stats;
    download(context, stats);
    food.foodsCount -= stats.foodCollected;
    tree.treesCount -= stats.woodCollected;
    stone.stonesCount -= stats.stoneCollected;
    civ.resources.food += stats.foodCollected;
    civ.resources.wood += stats.woodCollected;
    civ.resources.stone += stats.stoneCollected;

    civ.constructions[HOUSE] -= stats.housesBuilt;
    civ.constructions[FARM] -= stats.farmsBuilt;
    civ.constructions[SAWMILL] -= stats.sawmillsBuilt;
    civ.constructions[MINE] -= stats.minesBuilt;

    civ.realWorkers[FARM] -= stats.newHumansInFarms;
    civ.realWorkers[SAWMILL] -= stats.newHumansInSawmills;
    civ.realWorkers[MINE] -= stats.newHumansInMines;

    reset(context);
}

void StatsManager::download(VulkanContext& context, EndStats& outStats) 
{
    statsBuffer.download(&outStats, sizeof(EndStats));
}