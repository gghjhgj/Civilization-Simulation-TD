#pragma once

#include "HumanTypes.h"
#include "XY/XY.h"
#include "Config.h"
#include "Food.h"
#include "Tree.h"
#include "Stone.h"
#include <tbb/parallel_invoke.h>
#include <tbb/parallel_for.h>
#include <vector>
#include <cmath>
class Civilization;
class World;
class Human
{
public:
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
    void createHuman(World& world, Civilization& civilization);//git
    void humanRespawn(World& world, Civilization& civilization);//git
    XY humanFindResource(World& world, uint32_t x, uint32_t y, TerrainType type);
    XY humanFindFlagChunk(World& world, uint32_t x, uint32_t y, ChunkFlag flag);
    XY humanFindWorkingBuildingChunk(World& world, uint32_t x, uint32_t y, BuildingType type);
    bool gotResource(uint32_t hx, uint32_t hy, uint32_t rx, uint32_t ry);
    Dirs humanMoveDecision(HumanBase& base);
   template<typename T, typename Func>
void processHumanVector(std::vector<T>& humans, World& world, RendererSFML &renderer, Func aiLogic)
{
    tbb::parallel_for(
        tbb::blocked_range<size_t>(0, humans.size(), 256),
        [&](const tbb::blocked_range<size_t>& range)
        {
            for (size_t i = range.begin(); i < range.end(); i++)
            {
                auto& h = humans[i];

                Dirs dir;
                XY newPos;
                bool removed = false;

                if (aiLogic(h, dir, newPos, removed))
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
    );
}
    void humanMove(World& world, Civilization& civilization, Food& food, Tree& tree, Stone& stone, RendererSFML &renderer);
};

#include "HumanLogic.hpp"