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
#include "ThreadPool.hpp"

#include <vector>
#include <cmath>
class Civilization;
class World;
class Human
{
public:
    Human(ThreadPool &pool)
        :threadPool(pool)
        {}
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

        bool operator==(const DataNeededForEndConstruction&) const = default;
    };
    struct ThreadLocalData
    {
        int foodCollected;
        int woodCollected;
        int stoneCollected;
        int housesBuilt;
        int farmsBuilt;
        int sawmillsBuilt;
        int minesBuilt;
        int farmWorkersDelta;
        int sawmillWorkersDelta;
        int mineWorkersDelta;
        std::vector<DataNeededForEndConstruction> constr;
        std::vector<size_t> assignedRemoveQueue;
    };
    void createHuman(World& world, Civilization& civilization);//git
    void humanRespawn(World& world, Civilization& civilization);//git
    XY humanFindResource(World& world, uint32_t x, uint32_t y, TerrainType type);
    XY humanFindFlagChunk(World& world, uint32_t x, uint32_t y, ChunkFlag flag);
    XY humanFindWorkingBuildingChunk(World& world, uint32_t x, uint32_t y, BuildingType type);
    bool gotResource(uint32_t hx, uint32_t hy, uint32_t rx, uint32_t ry);
    Dirs humanMoveDecision(HumanBase& base);
    template<typename T, typename Func>
    void processHumanRange(
        std::vector<T>& humans,
        size_t begin,
        size_t end,
        World& world,
        RendererSFML& renderer,
        Func aiLogic,
        int threadID
    )
    {
        for (size_t i = begin; i < end; i++)
        {
            auto& h = humans[i];

            Dirs dir;
            XY newPos;
            bool removed = false;


            if (aiLogic(h, dir, newPos, removed, threadID))
            {
                continue;
            }


            if (world.isValid(newPos.x, newPos.y) &&
                world.getCell(newPos.x, newPos.y) != TerrainType::Water)
            {
                h.moves++;
                h.pos = { newPos.x, newPos.y };
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
    }

    template<typename T, typename Func>
    void addHumanTasks(
        std::vector<std::function<void(int)>>& tasks,
        std::vector<T>& humans,
        World& world,
        RendererSFML& renderer,
        Func aiLogic
    )
    {
        constexpr size_t GRAIN = 128;

        for (size_t i = 0; i < humans.size(); i += GRAIN)
        {
            size_t begin = i;

            size_t end = std::min(i + GRAIN, humans.size());

            tasks.push_back(
                [&, begin, end, aiLogic](int threadID)
                {
                    processHumanRange(
                        humans,
                        begin,
                        end,
                        world,
                        renderer,
                        aiLogic,
                        threadID
                    );
                }
            );
        }
    }


    void humanMove(World& world, Civilization& civilization, Food& food, Tree& tree, Stone& stone, RendererSFML& renderer);


    private:

    ThreadPool &threadPool;
};

#include "HumanLogic.hpp"