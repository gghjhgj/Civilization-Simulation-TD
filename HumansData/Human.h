#pragma once

#include "HumanTypes.h"
#include "XY.h"

#include "Config.h"
#include "Food.h"
#include "Tree.h"
#include "Stone.h"

#include <vector>
#include <cmath>
class Civilization;
class World;
class Human
{
public:
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

    std::vector<CollectorHuman> foodCollectors;
    std::vector<CollectorHuman> woodCollectors;
    std::vector<CollectorHuman> stoneCollectors;
    std::vector<HumanBuilder> builders;
    std::vector<HumanAssigned> assigned;
    std::vector<DeadHuman> dead;




    int humansCount = 0;
    int humansHavingHouseCount = 0;

    void createHuman(World& world, Civilization& civilization);//git
    void humanRespawn(World& world, Civilization& civilization);//git
    XY humanFindResource(World& world, uint32_t x, uint32_t y, TerrainType type);
    XY humanFindFlagChunk(World& world, uint32_t x, uint32_t y, ChunkFlag flag);
    XY humanFindWorkingBuildingChunk(World& world, uint32_t x, uint32_t y, BuildingType type);
    bool gotResource(uint32_t hx, uint32_t hy, uint32_t rx, uint32_t ry);
    Dirs humanMoveDecision(HumanBase& base);
    template <typename T, typename Func>
    void processHumanVector(std::vector<T>& humans, World& world, Func aiLogic)
    {
        for (int i = 0; i < humans.size();)
        {
            auto& h = humans[i];
            Dirs dir;
            XY newPos;
            bool removed = false;

            if (aiLogic(humans[i], dir, newPos, removed))
            {
                continue;
            }


            if (world.isValid(newPos.x, newPos.y) && world.getCell(newPos.x, newPos.y) != TerrainType::Water) {
                h.moves++;
                h.pos = { newPos.x, newPos.y };
            }
            else {
                h.points -= (1 + (rand() % 7));
            }
            if (h.points <= 0) h.points += 1000;
            i++;
        }
    }
    void humanMove(World& world, Civilization& civilization, Food& food, Tree& tree, Stone& stone, Human& human);
};

#include "HumanLogic.hpp"