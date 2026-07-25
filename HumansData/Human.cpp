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

    uint16_t x = static_cast<uint16_t>(civilization.spawn.chunkX * ChunkConfig::CHUNK_SIZE);
    uint16_t y = static_cast<uint16_t>(civilization.spawn.chunkY * ChunkConfig::CHUNK_SIZE);

    uint16_t x2;
    uint16_t y2;

    int maxRange = (Config::sizeX > Config::sizeY) ? Config::sizeX : Config::sizeY;

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
        } while (!world.isValid(x2, y2) ||
                 world.getCell(x2, y2) != TerrainType::Land);

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
    uint16_t x = static_cast<uint16_t>(civilization.spawn.chunkX * ChunkConfig::CHUNK_SIZE);
    uint16_t y = static_cast<uint16_t>(civilization.spawn.chunkY * ChunkConfig::CHUNK_SIZE);
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

    auto ref = world.getCellRef(x, y);

    uint16_t cell =
        world.grid[ref.chunkRegionIndex]
            .chunks[ref.localChunkIndex]
            .whereType(type);

    if (cell != ChunkConfig::CELL_COUNT)
    {
        int localX = cell % 3;
        int localY = cell / 3;

        return {
            static_cast<uint16_t>(ref.chunkX * 3 + localX),
            static_cast<uint16_t>(ref.chunkY * 3 + localY)};
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

Human::Dirs Human::humanMoveDecision(
    uint16_t x, uint16_t y,
    uint16_t targetX, uint16_t targetY,
    uint8_t points)
{
    int a;
    if (targetX == UINT16_MAX || targetY == UINT16_MAX)
    {
        uint8_t directionIndex = points % 8;
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
    RendererSFML &renderer)
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
                    int threadID =
                        tbb::this_task_arena::current_thread_index();

                    if (humanTicks % 3 == 0)
                    {
                        for (size_t i = range.begin(); i < range.end(); i++)
                        {
                            auto &h = foodCollectors;
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
                    for (size_t i = range.begin();
                         i < range.end();
                         i++)
                    {
                        auto &h = foodCollectors;

                        Dirs dir;

                        dir = humanMoveDecision(
                            h.posX[i],
                            h.posY[i],
                            h.targetX[i],
                            h.targetY[i],
                            i + h.points[i]);

                        uint16_t newX = h.posX[i] + dir.x;
                        uint16_t newY = h.posY[i] + dir.y;

                        if (h.targetX[i] != UINT16_MAX &&
                            h.targetY[i] != UINT16_MAX &&
                            gotResource(
                                newX,
                                newY,
                                h.targetX[i],
                                h.targetY[i]))
                        {
                            h.targetX[i] = UINT16_MAX;
                            h.targetY[i] = UINT16_MAX;
                            if (world.getCell(newX, newY) == TerrainType::LandWithFood)
                            {
                                h.points[i] += h.points[i] & 3;
                                threadResults[threadID].foodCollected++;

                                world.setCell(
                                    newX,
                                    newY,
                                    TerrainType::Land);

                                renderer.addToDirtyBuffer(
                                    world,
                                    newX,
                                    newY,
                                    sf::Color::Green,
                                    threadID);
                            }
                        }

                        if (world.isValid(newX, newY) &&
                            world.getCell(newX, newY) != TerrainType::Water)
                        {
                            h.posX[i] = newX;
                            h.posY[i] = newY;
                        }
                        else
                        {
                            h.points[i] -= 4;
                        }
                    }
                },
                foodCollectorsPartitioner);
        });
}

void Human::processWoodCollectors(
    World &world,
    RendererSFML &renderer)
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
                    int threadID =
                        tbb::this_task_arena::current_thread_index();

                    if (humanTicks % 3 == 0)
                    {
                        for (size_t i = range.begin(); i < range.end(); i++)
                        {
                            auto &h = woodCollectors;

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
                    for (size_t i = range.begin();
                         i < range.end();
                         i++)
                    {
                        auto &h = woodCollectors;

                        Dirs dir;

                        dir = humanMoveDecision(
                            h.posX[i],
                            h.posY[i],
                            h.targetX[i],
                            h.targetY[i],
                            i + h.points[i]);

                        uint16_t newX = h.posX[i] + dir.x;
                        uint16_t newY = h.posY[i] + dir.y;

                        if (h.targetX[i] != UINT16_MAX &&
                            h.targetY[i] != UINT16_MAX &&
                            gotResource(
                                newX,
                                newY,
                                h.targetX[i],
                                h.targetY[i]))
                        {
                            h.targetX[i] = UINT16_MAX;
                            h.targetY[i] = UINT16_MAX;

                            if (world.getCell(newX, newY) == TerrainType::LandWithTree)
                            {
                                h.points[i] += h.points[i] & 3;

                                threadResults[threadID]
                                    .woodCollected++;

                                world.setCell(
                                    newX,
                                    newY,
                                    TerrainType::Land);

                                renderer.addToDirtyBuffer(
                                    world,
                                    newX,
                                    newY,
                                    sf::Color::Green,
                                    threadID);
                            }
                        }

                        if (world.isValid(newX, newY) &&
                            world.getCell(newX, newY) != TerrainType::Water)
                        {

                            h.posX[i] = newX;
                            h.posY[i] = newY;
                        }
                        else
                        {
                            h.points[i] -= 4;
                        }
                    }
                },
                woodCollectorsPartitioner);
        });
}

void Human::processStoneCollectors(
    World &world,
    RendererSFML &renderer)
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
                    int threadID =
                        tbb::this_task_arena::current_thread_index();

                    if (humanTicks % 3 == 0)
                    {
                        for (size_t i = range.begin(); i < range.end(); i++)
                        {
                            auto &h = stoneCollectors;

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
                    for (size_t i = range.begin();
                         i < range.end();
                         i++)
                    {
                        auto &h = stoneCollectors;

                        Dirs dir;

                        dir = humanMoveDecision(
                            h.posX[i],
                            h.posY[i],
                            h.targetX[i],
                            h.targetY[i],
                            i + h.points[i]);

                        uint16_t newX = h.posX[i] + dir.x;
                        uint16_t newY = h.posY[i] + dir.y;

                        if (h.targetX[i] != UINT16_MAX &&
                            h.targetY[i] != UINT16_MAX &&
                            gotResource(
                                newX,
                                newY,
                                h.targetX[i],
                                h.targetY[i]))
                        {
                            h.targetX[i] = UINT16_MAX;
                            h.targetY[i] = UINT16_MAX;

                            if (world.getCell(newX, newY) == TerrainType::MountainWithStone)
                            {
                                h.points[i] += h.points[i] & 3;

                                threadResults[threadID]
                                    .stoneCollected++;

                                world.setCell(
                                    newX,
                                    newY,
                                    TerrainType::Mountain);

                                renderer.addToDirtyBuffer(
                                    world,
                                    newX,
                                    newY,
                                    sf::Color(120, 120, 120),
                                    threadID);
                            }
                        }

                        if (world.isValid(newX, newY) &&
                            world.getCell(newX, newY) != TerrainType::Water)
                        {

                            h.posX[i] = newX;
                            h.posY[i] = newY;
                        }
                        else
                        {
                            h.points[i] -= 4;
                        }
                    }
                },
                stoneCollectorsPartitioner);
        });
}

void Human::processBuilders(
    World &world,
    RendererSFML &renderer)
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
                    int threadID =
                        tbb::this_task_arena::current_thread_index();

                    auto &h = builders;

                    if (humanTicks % 13 == 0)
                    {
                        for (size_t i = range.begin();
                             i < range.end();
                             i++)
                        {
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

                    for (size_t i = range.begin();
                         i < range.end();
                         i++)
                    {
                        Dirs dir =
                            humanMoveDecision(
                                h.posX[i],
                                h.posY[i],
                                h.targetX[i],
                                h.targetY[i],
                                i + h.points[i]);

                        uint16_t newX = h.posX[i] + dir.x;
                        uint16_t newY = h.posY[i] + dir.y;

                        if (h.targetX[i] != UINT16_MAX &&
                            h.targetY[i] != UINT16_MAX &&
                            gotResource(
                                newX,
                                newY,
                                h.targetX[i],
                                h.targetY[i]))
                        {
                            h.points[i]++;

                            auto ref =
                                world.getCellRef(
                                    newX,
                                    newY);

                            if (world.getBuilding(
                                    ref.chunkX,
                                    ref.chunkY) != BuildingType::None &&
                                world.hasChunkFlag(
                                    ref.chunkX,
                                    ref.chunkY,
                                    ChunkFlag::Construction))
                            {
                                BuildingType building =
                                    world.getBuilding(
                                        ref.chunkX,
                                        ref.chunkY);

                                Type type =
                                    GetTypeBuilding(building);

                                threadResults[threadID]
                                    .constr.push_back(
                                        {ref.chunkX,
                                         ref.chunkY,
                                         type});
                            }
                            else
                            {
                                h.targetX[i] = UINT16_MAX;
                                h.targetY[i] = UINT16_MAX;
                            }
                        }

                        if (world.isValid(newX, newY) &&
                            world.getCell(newX, newY) != TerrainType::Water)
                        {
                            h.posX[i] = newX;
                            h.posY[i] = newY;
                        }
                        else
                        {
                            h.points[i] -= 4;
                        }
                    }
                },
                buildersPartitioner);
        });
}

void Human::processAssigned(
    World &world,
    RendererSFML &renderer)
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
                    int threadID =
                        tbb::this_task_arena::current_thread_index();

                    auto &h = assigned;

                    if (humanTicks % 13 == 0)
                    {
                        for (size_t i = range.begin();
                             i < range.end();
                             i++)
                        {
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

                    for (size_t i = range.begin();
                         i < range.end();
                         i++)
                    {
                        Dirs dir =
                            humanMoveDecision(
                                h.posX[i],
                                h.posY[i],
                                h.targetX[i],
                                h.targetY[i],
                                i + h.points[i]);

                        uint16_t newX = h.posX[i] + dir.x;
                        uint16_t newY = h.posY[i] + dir.y;

                        if (h.targetX[i] != UINT16_MAX &&
                            h.targetY[i] != UINT16_MAX &&
                            gotResource(
                                newX,
                                newY,
                                h.targetX[i],
                                h.targetY[i]))
                        {
                            h.points[i]++;

                            auto ref =
                                world.getCellRef(
                                    newX,
                                    newY);

                            if (world.getBuilding(
                                    ref.chunkX,
                                    ref.chunkY) == h.targetBuilding[i] &&
                                !world.hasChunkFlag(
                                    ref.chunkX,
                                    ref.chunkY,
                                    ChunkFlag::Construction))
                            {
                                switch (h.targetBuilding[i])
                                {
                                case BuildingType::Farm:
                                    threadResults[threadID]
                                        .farmWorkersDelta++;
                                    break;

                                case BuildingType::Sawmill:
                                    threadResults[threadID]
                                        .sawmillWorkersDelta++;
                                    break;

                                case BuildingType::Mine:
                                    threadResults[threadID]
                                        .mineWorkersDelta++;
                                    break;

                                default:
                                    break;
                                }

                                threadResults[threadID]
                                    .assignedRemoveQueue
                                    .push_back(i);

                                continue;
                            }
                            else
                            {
                                h.targetX[i] = UINT16_MAX;
                                h.targetY[i] = UINT16_MAX;
                            }
                        }

                        if (world.isValid(newX, newY) &&
                            world.getCell(newX, newY) != TerrainType::Water)
                        {
                            h.posX[i] = newX;
                            h.posY[i] = newY;
                        }
                        else
                        {
                            h.points[i] -= 4;
                        }
                    }
                },
                assignedPartitioner);
        });
}

void Human::humanMove(World &world, Civilization &civilization, Food &food, Tree &tree, Stone &stone, RendererSFML &renderer)
{
    for (auto &r : threadResults)
    {
        r.clear();
    }

    // a)
    /*
    processFoodCollectors(world, renderer);
    processWoodCollectors(world, renderer);
    processStoneCollectors(world, renderer);
    processBuilders(world, renderer);
    processAssigned(world, renderer);
    humanTicks++;
    */

    // b)
    
        tbb::parallel_invoke(
            [&]
            { processFoodCollectors(world, renderer); },
            [&]
            { processWoodCollectors(world, renderer); },
            [&]
            { processStoneCollectors(world, renderer); },
            [&]
            { processBuilders(world, renderer); },
            [&]
            { processAssigned(world, renderer); });

        humanTicks++;
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

        allAssignedToRemove.insert(
            allAssignedToRemove.end(),
            res.assignedRemoveQueue.begin(),
            res.assignedRemoveQueue.end());

        allConstructionsToEnd.insert(
            allConstructionsToEnd.end(),
            res.constr.begin(),
            res.constr.end());
    }
    if (!allAssignedToRemove.empty())
    {
        std::sort(allAssignedToRemove.rbegin(), allAssignedToRemove.rend());
        allAssignedToRemove.erase(std::unique(allAssignedToRemove.begin(), allAssignedToRemove.end()), allAssignedToRemove.end());

        for (size_t id : allAssignedToRemove)
        {
            if (id >= assigned.posX.size())
            {
                continue;
            }
            eraseHuman(*this, assigned, id);
        }
        allAssignedToRemove.clear();
    }
    
    if (!allConstructionsToEnd.empty())
    {
        std::sort(allConstructionsToEnd.begin(), allConstructionsToEnd.end());
        allConstructionsToEnd.erase(std::unique(allConstructionsToEnd.begin(), allConstructionsToEnd.end()), allConstructionsToEnd.end());

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

    if (humanTicks % Config::ticksForNewHumans == 0)
    {
        humanRespawn(world, civilization);
    }
    if (humanTicks % Config::ticksForAssigningDecision == 0)
    {
        civilization.assignHumansToBuilding(*this, Type::FARM);
        civilization.assignHumansToBuilding(*this, Type::SAWMILL);
        civilization.assignHumansToBuilding(*this, Type::MINE);
    }
    if (humanTicks % Config::ticksForCivilizationDecision == 0)
    {
        civilization.civilizationDecision(*this, food, stone, tree);
    }
}