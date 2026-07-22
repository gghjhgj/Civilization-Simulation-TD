#pragma once

#include "HumanTypes.h"
#include "XY/XY.h"
#include "Config.h"
#include "Food.h"
#include "Tree.h"
#include "Stone.h"

#include <tbb/parallel_for.h>
#include <tbb/parallel_invoke.h>
#include <tbb/task_arena.h>
#include <tbb/task_group.h>
#include <tbb/enumerable_thread_specific.h>

#include "../ThreadController.hpp"
#include "../TBBPinObserver.hpp"
#include <memory>
#include <vector>
#include <cmath>
class Civilization;
class World;
class Human
{
public:
    tbb::task_arena aiArena{10};
    std::unique_ptr<TBBPinObserver> aiObserver;

    Human()
    {
        aiObserver =
            std::make_unique<TBBPinObserver>(
                aiArena);
        aiArena.initialize();
    }

    inline uint32_t random10()
    {
        thread_local uint32_t state =
            std::chrono::high_resolution_clock::now()
                .time_since_epoch()
                .count();

        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;

        return (state % 10) + 1;
    }
    uint64_t humanTicks = 0;
    struct Dirs
    {
        int8_t x;
        int8_t y;
    };
    enum ResourceType
    {
        food,
        tree,
        stone,
        construction
    };

    std::vector<HumanBase> foodCollectors;
    std::vector<HumanBase> woodCollectors;
    std::vector<HumanBase> stoneCollectors;
    std::vector<HumanBase> builders;
    std::vector<HumanBase> assigned;
    std::vector<DeadHuman> dead;

    int humansCount = 0;
    int humansHavingHouseCount = 0;
    inline uint32_t hash(uint32_t x)
    {
        x ^= x >> 16;
        x *= 0x7feb352d;
        x ^= x >> 15;
        x *= 0x846ca68b;
        x ^= x >> 16;
        return x;
    }
    struct DataNeededForEndConstruction
    {
        uint32_t chunkX;
        uint32_t chunkY;
        Type type;

        bool operator==(const DataNeededForEndConstruction &) const = default;
    };
    struct ThreadLocalData
    {
        int foodCollected = 0;
        int woodCollected = 0;
        int stoneCollected = 0;
        int housesBuilt = 0;
        int farmsBuilt = 0;
        int sawmillsBuilt = 0;
        int minesBuilt = 0;
        int farmWorkersDelta = 0;
        int sawmillWorkersDelta = 0;
        int mineWorkersDelta = 0;
        std::vector<DataNeededForEndConstruction> constr;
        std::vector<size_t> assignedRemoveQueue;

        void clear()
        {
            foodCollected = 0;
            woodCollected = 0;
            stoneCollected = 0;

            housesBuilt = 0;
            farmsBuilt = 0;
            sawmillsBuilt = 0;
            minesBuilt = 0;

            farmWorkersDelta = 0;
            sawmillWorkersDelta = 0;
            mineWorkersDelta = 0;

            constr.clear();
            assignedRemoveQueue.clear();
        }
    };
    void createHuman(World &world, Civilization &civilization);  // git
    void humanRespawn(World &world, Civilization &civilization); // git
    XY humanFindResource(World &world, uint32_t x, uint32_t y, TerrainType type);
    XY humanFindFlagChunk(World &world, uint32_t x, uint32_t y, ChunkFlag flag);
    XY humanFindWorkingBuildingChunk(World &world, uint32_t x, uint32_t y, BuildingType type);
    bool gotResource(uint32_t hx, uint32_t hy, uint32_t rx, uint32_t ry);
    Dirs humanMoveDecision(HumanBase &base);
    template <typename T, typename Func>
    void processHumanVector(
        std::vector<T> &humans,
        World &world,
        RendererSFML &renderer,
        Func aiLogic)
    {

        aiArena.execute(
            [&]()
            {
                tbb::parallel_for(
                    tbb::blocked_range<size_t>(
                        0,
                        humans.size(),
                        256),

                    [&](const tbb::blocked_range<size_t> &range)
                    {
                        int threadID =
                            tbb::this_task_arena::current_thread_index();

                        for (size_t i = range.begin();
                             i < range.end();
                             i++)
                        {
                            auto &h = humans[i];

                            Dirs dir;
                            XY newPos;
                            bool removed = false;

                            if (aiLogic(
                                    h,
                                    dir,
                                    newPos,
                                    removed,
                                    threadID))
                            {
                                continue;
                            }

                            if (world.isValid(newPos.x, newPos.y) &&
                                world.getCell(newPos.x, newPos.y) != TerrainType::Water)
                            {

                                h.moves++;
                                h.pos =
                                    {
                                        newPos.x,
                                        newPos.y};
                            }
                            else
                            {
                                h.points -= random10();
                            }

                            if (h.points <= 0)
                                h.points += random10() * random10();

                            if (random10() % 4 == 0)
                                h.points -= random10();
                        }
                    });
            });
    }

    void humanMove(World &world, Civilization &civilization, Food &food, Tree &tree, Stone &stone, RendererSFML &renderer);

private:
    std::vector<ThreadLocalData> threadResults;
    std::vector<size_t> allAssignedToRemove;
    std::vector<DataNeededForEndConstruction> allConstructionsToEnd;
};

#include "HumanLogic.hpp"