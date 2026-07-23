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

    HumanBase foodCollectors;
    HumanBase woodCollectors;
    HumanBase stoneCollectors;
    HumanBase builders;
    HumanBase assigned;

    int humansCount = 0;
    int humansHavingHouseCount = 0;
    inline uint16_t hash(uint16_t x)
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
    XY humanFindResource(World &world, uint16_t x, uint16_t y, TerrainType type);
    XY humanFindFlagChunk(World &world, uint16_t x, uint16_t y, ChunkFlag flag);
    XY humanFindWorkingBuildingChunk(World &world, uint16_t x, uint16_t y, BuildingType type);
    bool gotResource(uint16_t hx, uint16_t hy, uint16_t rx, uint16_t ry);
    Dirs humanMoveDecision(
        uint16_t x, uint16_t y,
        uint16_t targetX, uint16_t targetY,
        uint8_t points
    );
    void humanMove(World &world, Civilization &civilization, Food &food, Tree &tree, Stone &stone, RendererSFML &renderer);

    template<typename T>
    void reserveHumans(T& humans, size_t count)
    {
        humans.posX.reserve(count);
        humans.posY.reserve(count);

        humans.targetX.reserve(count);
        humans.targetY.reserve(count);

        humans.points.reserve(count);
        humans.moves.reserve(count);

        humans.targetBuilding.reserve(count);
    }

private:
    std::vector<ThreadLocalData> threadResults;
    std::vector<size_t> allAssignedToRemove;
    std::vector<DataNeededForEndConstruction> allConstructionsToEnd;

    void processFoodCollectors(
        World &world,
        RendererSFML &renderer
    );

    void processWoodCollectors(
        World &world,
        RendererSFML &renderer
    );

    void processStoneCollectors(
        World &world,
        RendererSFML &renderer
    );

    void processBuilders(
        World &world,
        RendererSFML &renderer
    );

    void processAssigned(
        World &world,
        RendererSFML &renderer
    );

    tbb::affinity_partitioner foodCollectorsPartitioner;
    tbb::affinity_partitioner woodCollectorsPartitioner;
    tbb::affinity_partitioner stoneCollectorsPartitioner;
    tbb::affinity_partitioner buildersPartitioner;
    tbb::affinity_partitioner assignedPartitioner;
};

#include "HumanLogic.hpp"