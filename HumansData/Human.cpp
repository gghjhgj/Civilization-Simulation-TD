#include "Human.h"

#include "WorldData/World.h"

#include "Civilization.h"

void Human::createHuman(World &world, Civilization &civilization)
{
    size_t threadsCount = tbb::this_task_arena::max_concurrency();

    threadResults.resize(threadsCount);

    size_t reserveCount = Config::humanCount + 40000;

    reserveHumans(foodCollectors, reserveCount);
    reserveHumans(woodCollectors, reserveCount);
    reserveHumans(stoneCollectors, reserveCount);
    reserveHumans(builders, reserveCount);
    reserveHumans(assigned, reserveCount);

    uint16_t x = static_cast<uint16_t>(civilization.spawnXY.x);
    uint16_t y = static_cast<uint16_t>(civilization.spawnXY.y);

    uint16_t x2;
    uint16_t y2;

    int maxRange = Config::maxHumansSpawnRange;

    for (int i = 0; i < Config::humanCount; i++)
    {
        int r = i + 1;
        if (r > maxRange)
            r = maxRange;

        do
        {
            int dx = (rand() % (2 * r + 1)) - r;
            int dy = (rand() % (2 * r + 1)) - r;

            x2 = x + dx;
            y2 = y + dy;
        } while (!world.isValid(x2, y2));

        addHuman(*this, this->foodCollectors, BuildingType::None, x2, y2);
    }

    /*
 for (int i = 0; i < Config::humanCount; i++)
 {
    addHuman(*this, this->foodCollectors, BuildingType::None, x, y);
 }
    */
}

void Human::humanRespawn(World &world, Civilization &civilization)
{
    int newPeople = static_cast<int>(std::cbrt(humansCount));
    uint16_t x = static_cast<uint16_t>(civilization.spawnXY.x);
    uint16_t y = static_cast<uint16_t>(civilization.spawnXY.y);
    for (int i = 0; i < newPeople; i++)
    {
        addHuman(*this, this->foodCollectors, BuildingType::None, x, y);
    }
}
XY Human::humanFindResource(World &world, uint16_t x, uint16_t y, TerrainType type)
{
    if (!world.isValid(x, y))
    {
        return {UINT16_MAX, UINT16_MAX};
    }

    const uint16_t chunkX = x / 3;
    const uint16_t chunkY = y / 3;

    const uint16_t regionX =
        chunkX / ChunkRegionConfig::CHUNK_REGION_SIZE;

    const uint16_t regionY =
        chunkY / ChunkRegionConfig::CHUNK_REGION_SIZE;

    const uint16_t localX =
        chunkX % ChunkRegionConfig::CHUNK_REGION_SIZE;

    const uint16_t localY =
        chunkY % ChunkRegionConfig::CHUNK_REGION_SIZE;

    const uint32_t regionIndex =
        regionY * WorldConfig::CHUNK_REGIONS_X + regionX;

    const uint16_t localChunkIndex =
        localY * ChunkRegionConfig::CHUNK_REGION_SIZE + localX;

    uint16_t cell =
        world.grid[regionIndex]
            .chunks[localChunkIndex]
            .whereType(type);

    if (cell != ChunkConfig::CELL_COUNT)
    {
        return {
            static_cast<uint16_t>(chunkX * 3 + cell % 3),
            static_cast<uint16_t>(chunkY * 3 + cell / 3)};
    }

    return {UINT16_MAX, UINT16_MAX};
}
XY Human::humanFindFlagChunk(World &world, uint16_t x, uint16_t y, ChunkFlag flag)
{
    if (!world.isValid(x, y))
    {
        return {UINT16_MAX, UINT16_MAX};
    }
    auto ref = world.getCellRef(x, y);

    auto &region = world.grid[ref.chunkRegionIndex];

    for (uint16_t i = 0; i < ChunkRegionConfig::CHUNK_REGION_SIZE * ChunkRegionConfig::CHUNK_REGION_SIZE; i++)
    {
        auto &chunk = region.chunks[i];

        if (chunk.hasFlag(ChunkFlag::Construction))
        {
            uint16_t localChunkX =
                i % ChunkRegionConfig::CHUNK_REGION_SIZE;

            uint16_t localChunkY =
                i / ChunkRegionConfig::CHUNK_REGION_SIZE;

            uint16_t regionX =
                ref.chunkX / ChunkRegionConfig::CHUNK_REGION_SIZE;

            uint16_t regionY =
                ref.chunkY / ChunkRegionConfig::CHUNK_REGION_SIZE;

            uint16_t chunkX =
                regionX * ChunkRegionConfig::CHUNK_REGION_SIZE + localChunkX;

            uint16_t chunkY =
                regionY * ChunkRegionConfig::CHUNK_REGION_SIZE + localChunkY;

            return {
                static_cast<uint16_t>(chunkX * ChunkConfig::CHUNK_SIZE),
                static_cast<uint16_t>(chunkY * ChunkConfig::CHUNK_SIZE)};
        }
    }
    return {UINT16_MAX, UINT16_MAX};
}
XY Human::humanFindWorkingBuildingChunk(
    World &world,
    uint16_t x,
    uint16_t y,
    BuildingType type)
{
    if (!world.isValid(x, y))
    {
        return {UINT16_MAX, UINT16_MAX};
    }
    auto ref = world.getCellRef(x, y);

    auto &region = world.grid[ref.chunkRegionIndex];

    uint16_t regionX =
        ref.chunkX / ChunkRegionConfig::CHUNK_REGION_SIZE;

    uint16_t regionY =
        ref.chunkY / ChunkRegionConfig::CHUNK_REGION_SIZE;

    for (uint16_t i = 0;
         i < ChunkRegionConfig::CHUNK_REGION_SIZE * ChunkRegionConfig::CHUNK_REGION_SIZE;
         i++)
    {
        auto &chunk = region.chunks[i];

        if (chunk.getBuilding() == type &&
            !chunk.hasFlag(ChunkFlag::Construction))
        {
            uint16_t localChunkX =
                i % ChunkRegionConfig::CHUNK_REGION_SIZE;

            uint16_t localChunkY =
                i / ChunkRegionConfig::CHUNK_REGION_SIZE;

            uint16_t chunkX =
                regionX * ChunkRegionConfig::CHUNK_REGION_SIZE + localChunkX;

            uint16_t chunkY =
                regionY * ChunkRegionConfig::CHUNK_REGION_SIZE + localChunkY;

            return {
                static_cast<uint16_t>(chunkX * ChunkConfig::CHUNK_SIZE),
                static_cast<uint16_t>(chunkY * ChunkConfig::CHUNK_SIZE)};
        }
    }

    return {UINT16_MAX, UINT16_MAX};
}
inline bool Human::gotResource(uint16_t hx, uint16_t hy, uint16_t rx, uint16_t ry)
{
    if (hx == rx && hy == ry)
        return true;
    return false;
}

inline Human::Dirs Human::humanMoveDecision(
    uint16_t x, uint16_t y,
    uint16_t targetX, uint16_t targetY,
    uint8_t points)
{
    int a;
    if (targetX == UINT16_MAX || targetY == UINT16_MAX)
    {
        uint8_t directionIndex = points & 7;
        static constexpr int lookupX[8] = {1, 1, 0, -1, -1, -1, 0, 1};
        static constexpr int lookupY[8] = {0, 1, 1, 1, 0, -1, -1, -1};
        return {
            static_cast<int8_t>(lookupX[directionIndex]),
            static_cast<int8_t>(lookupY[directionIndex])};
    }

    int dx = targetX - x;
    int dy = targetY - y;
    int8_t dirX = (dx > 0) - (dx < 0);
    int8_t dirY = (dy > 0) - (dy < 0);

    return {dirX, dirY};
}

void Human::processFoodCollectors(
    World &world,
    RendererSFML &renderer,
    Civilization &civilization)
{
    aiArena.execute(
        [&]()
        {
            tbb::parallel_for(
                tbb::blocked_range<size_t>(
                    0,
                    foodCollectors.posX.size(),
                    Config::GRAIN),

                [&](const tbb::blocked_range<size_t> &range)
                {
                    auto &h = foodCollectors;
                    int threadID =
                        tbb::this_task_arena::current_thread_index();
                    for (int i = 0; i < ticksToDo; i++)
                    {
                        if ((humanTicks + i) % 3 == 0)
                        {
                            for (size_t i = range.begin(); i < range.end(); i++)
                            {
                                if (!isInCivRange(
                                        h.posX[i],
                                        h.posY[i],
                                        civilization))
                                {
                                    setWorkerTargetAsCivRangeBoundary(
                                        civilization,
                                        h,
                                        i);

                                    continue;
                                }

                                XY target =
                                    humanFindResource(
                                        world,
                                        h.posX[i],
                                        h.posY[i],
                                        TerrainType::LandWithFood);

                                h.targetX[i] = target.x;
                                h.targetY[i] = target.y;
                            }
                        }
                        processHumanRange<HumanType::FoodCollector>(
                            *this,
                            foodCollectors,
                            world,
                            renderer,
                            civilization,
                            range.begin(),
                            range.end(),
                            threadID);
                    }
                },
                foodCollectorsPartitioner);
        });
}

void Human::processWoodCollectors(
    World &world,
    RendererSFML &renderer,
    Civilization &civilization)
{
    aiArena.execute(
        [&]()
        {
            tbb::parallel_for(
                tbb::blocked_range<size_t>(
                    0,
                    woodCollectors.posX.size(),
                    Config::GRAIN),

                [&](const tbb::blocked_range<size_t> &range)
                {
                    auto &h = woodCollectors;

                    int threadID =
                        tbb::this_task_arena::current_thread_index();

                    for (int i = 0; i < ticksToDo; i++)
                    {
                        if ((humanTicks + i) % 3 == 0)
                        {
                            for (size_t i = range.begin();
                                 i < range.end();
                                 i++)
                            {
                                if (!isInCivRange(
                                        h.posX[i],
                                        h.posY[i],
                                        civilization))
                                {
                                    setWorkerTargetAsCivRangeBoundary(
                                        civilization,
                                        h,
                                        i);

                                    continue;
                                }

                                XY target =
                                    humanFindResource(
                                        world,
                                        h.posX[i],
                                        h.posY[i],
                                        TerrainType::LandWithTree);

                                h.targetX[i] = target.x;
                                h.targetY[i] = target.y;
                            }
                        }

                        processHumanRange<HumanType::WoodCollector>(
                            *this,
                            woodCollectors,
                            world,
                            renderer,
                            civilization,
                            range.begin(),
                            range.end(),
                            threadID);
                    }
                },
                woodCollectorsPartitioner);
        });
}

void Human::processStoneCollectors(
    World &world,
    RendererSFML &renderer,
    Civilization &civilization)
{
    aiArena.execute(
        [&]()
        {
            tbb::parallel_for(
                tbb::blocked_range<size_t>(
                    0,
                    stoneCollectors.posX.size(),
                    Config::GRAIN),

                [&](const tbb::blocked_range<size_t> &range)
                {
                    auto &h = stoneCollectors;

                    int threadID =
                        tbb::this_task_arena::current_thread_index();

                    for (int i = 0; i < ticksToDo; i++)
                    {
                        if ((humanTicks + i) % 3 == 0)
                        {
                            for (size_t i = range.begin();
                                 i < range.end();
                                 i++)
                            {
                                if (!isOnMountain(
                                        world,
                                        h.posX[i],
                                        h.posY[i]))
                                {
                                    setStoneCollectorMountainTarget(
                                        world,
                                        h,
                                        i);

                                        continue;
                                }
                                XY target =
                                    humanFindResource(
                                        world,
                                        h.posX[i],
                                        h.posY[i],
                                        TerrainType::MountainWithStone);

                                h.targetX[i] = target.x;
                                h.targetY[i] = target.y;
                            }
                        }

                        processHumanRange<HumanType::StoneCollector>(
                            *this,
                            stoneCollectors,
                            world,
                            renderer,
                            civilization,
                            range.begin(),
                            range.end(),
                            threadID);
                    }
                },
                stoneCollectorsPartitioner);
        });
}

void Human::processBuilders(
    World &world,
    RendererSFML &renderer,
    Civilization &civilization)
{
    for (int i = 0; i < ticksToDo; i++)
    {
        aiArena.execute(
            [&]()
            {
                tbb::parallel_for(
                    tbb::blocked_range<size_t>(
                        0,
                        builders.posX.size(),
                        Config::GRAIN),

                    [&](const tbb::blocked_range<size_t> &range)
                    {
                        auto &h = builders;

                        int threadID =
                            tbb::this_task_arena::current_thread_index();

                        if ((humanTicks + i) % 13 == 0)
                        {
                            for (size_t i = range.begin();
                                 i < range.end();
                                 i++)
                            {
                                if (!isInCivZone(
                                        h.posX[i],
                                        h.posY[i],
                                        civilization))
                                {
                                    setWorkerTargetAsCivSpawn(
                                        civilization,
                                        h,
                                        i);

                                    continue;
                                }

                                XY target =
                                    humanFindFlagChunk(
                                        world,
                                        h.posX[i],
                                        h.posY[i],
                                        ChunkFlag::Construction);

                                h.targetX[i] = target.x;
                                h.targetY[i] = target.y;
                            }
                        }

                        processHumanRange<HumanType::Builder>(
                            *this,
                            builders,
                            world,
                            renderer,
                            civilization,
                            range.begin(),
                            range.end(),
                            threadID);
                    },
                    buildersPartitioner);
            });
    }

    for (const auto &res : threadResults)
    {
        allConstructionsToEnd.insert(
            allConstructionsToEnd.end(),
            res.constr.begin(),
            res.constr.end());
    }

    if (!allConstructionsToEnd.empty())
    {
        std::sort(
            allConstructionsToEnd.begin(),
            allConstructionsToEnd.end());

        allConstructionsToEnd.erase(
            std::unique(
                allConstructionsToEnd.begin(),
                allConstructionsToEnd.end()),
            allConstructionsToEnd.end());

        for (auto constr : allConstructionsToEnd)
        {
            civilization.endConstruction(
                world,
                renderer,
                *this,
                constr.chunkX,
                constr.chunkY,
                constr.type);
        }

        allConstructionsToEnd.clear();
    }

    for (auto &res : threadResults)
    {
        res.constr.clear();
    }
}

void Human::processAssigned(
    World &world,
    RendererSFML &renderer,
    Civilization &civilization)
{
    for (int i = 0; i < ticksToDo; i++)
    {
        aiArena.execute(
            [&]()
            {
                tbb::parallel_for(
                    tbb::blocked_range<size_t>(
                        0,
                        assigned.posX.size(),
                        Config::GRAIN),

                    [&](const tbb::blocked_range<size_t> &range)
                    {
                        auto &h = assigned;

                        int threadID =
                            tbb::this_task_arena::current_thread_index();

                        if ((humanTicks + i) % 13 == 0)
                        {
                            for (size_t i = range.begin();
                                 i < range.end();
                                 i++)
                            {
                                if (!isInCivZone(
                                        h.posX[i],
                                        h.posY[i],
                                        civilization))
                                {
                                    setWorkerTargetAsCivSpawn(
                                        civilization,
                                        h,
                                        i);

                                    continue;
                                }
                                XY target =
                                    humanFindWorkingBuildingChunk(
                                        world,
                                        h.posX[i],
                                        h.posY[i],
                                        h.targetBuilding[i]);

                                h.targetX[i] = target.x;
                                h.targetY[i] = target.y;
                            }
                        }

                        processHumanRange<HumanType::Assigned>(
                            *this,
                            assigned,
                            world,
                            renderer,
                            civilization,
                            range.begin(),
                            range.end(),
                            threadID);
                    },
                    assignedPartitioner);
            });
    }

    for (const auto &res : threadResults)
    {
        allAssignedToRemove.insert(
            allAssignedToRemove.end(),
            res.assignedRemoveQueue.begin(),
            res.assignedRemoveQueue.end());
    }

    if (!allAssignedToRemove.empty())
    {
        std::sort(
            allAssignedToRemove.rbegin(),
            allAssignedToRemove.rend());

        allAssignedToRemove.erase(
            std::unique(
                allAssignedToRemove.begin(),
                allAssignedToRemove.end()),
            allAssignedToRemove.end());

        for (size_t id : allAssignedToRemove)
        {
            if (id >= assigned.posX.size())
            {
                continue;
            }

            eraseHuman(
                *this,
                assigned,
                id);
        }

        allAssignedToRemove.clear();
    }

    for (auto &res : threadResults)
    {
        res.assignedRemoveQueue.clear();
    }
}

void Human::humanMove(World &world, Civilization &civilization, Food &food, Tree &tree, Stone &stone, RendererSFML &renderer)
{
    for (auto &r : threadResults)
    {
        r.clear();
    }
    constexpr uint32_t t = 1000000 + Config::humanCount * 10;
    ticksToDo = t / (foodCollectors.posX.size() + woodCollectors.posX.size() + stoneCollectors.posX.size() + builders.posX.size() + assigned.posX.size());

    actionsToDo = (ticksToDo + ticksLeft) / Config::ticksForNewHumans;
    ticksLeft = (ticksToDo + ticksLeft) % Config::ticksForNewHumans;

    tbb::parallel_invoke(
        [&]
        { processFoodCollectors(world, renderer, civilization); },
        [&]
        { processWoodCollectors(world, renderer, civilization); },
        [&]
        { processStoneCollectors(world, renderer, civilization); },
        [&]
        { processBuilders(world, renderer, civilization); },
        [&]
        { processAssigned(world, renderer, civilization); });

    humanTicks += ticksToDo;
    ///////////////////////////sync

    for (const auto &res : threadResults)
    {
        food.foodsCount -= res.foodCollected;
        civilization.resources.food += res.foodCollected;

        tree.treesCount -= res.woodCollected;
        civilization.resources.wood += res.woodCollected;

        stone.stonesCount -= res.stoneCollected;
        civilization.resources.stone += res.stoneCollected;
        civilization.realWorkers[FARM] += res.farmWorkersDelta;
        civilization.realWorkers[SAWMILL] += res.sawmillWorkersDelta;
        civilization.realWorkers[MINE] += res.mineWorkersDelta;
    }

    for (int i = 0; i < actionsToDo; i++)
    {
        humanRespawn(world, civilization);

        civilization.assignHumansToBuilding(*this, Type::FARM);
        civilization.assignHumansToBuilding(*this, Type::SAWMILL);
        civilization.assignHumansToBuilding(*this, Type::MINE);

        civilization.civilizationDecision(*this, food, stone, tree);
    }
}