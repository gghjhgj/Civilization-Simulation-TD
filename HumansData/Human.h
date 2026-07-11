#pragma once
#include "Tasks.h"
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
    struct XY
    {
        uint32_t x;
        uint32_t y;
    };
    struct Dirs
    {
        int8_t x;
        int8_t y;
    };
    struct HumanData
    {
        XY pos;
        XY oldPos;
        TerrainType targetTerrain;
        int id;
        int HP = Config::humanHP;
        int Stamina = Config::humanStamina;
        int points = 10000;
        int moves = 0;
        XY targetPos = {UINT32_MAX, UINT32_MAX};
    };
    std::vector <HumanData> humans;

    enum ResourceType
    {
        food,
        tree,
        stone,
        construction
    };
    
    int humansCount = 0;
    int humansHavingHouseCount = 0;


    void addHuman(World &world, uint32_t x, uint32_t y, int vecId);//git
    void createHuman(World &world, Civilization &civilization);//git
    void eraseHuman(World &world, Civilization &civilization, int vecID);//nie git na sam koniec
    void humanRespawn(World &world, Civilization &civilization);//git
    XY humanFindResource(World &world, uint32_t x, uint32_t y, TerrainType type);//nie git - to ma dzialac na surowce na budynki nowa funkcja bedzie
    bool gotResource(uint32_t hx, uint32_t hy, uint32_t rx, uint32_t ry);
    Dirs humanMoveDecision(uint32_t humanX, uint32_t humanY, uint32_t targetX, uint32_t targetY, int i);//nie git
    void humanMove(World &world, Civilization &civilization, Food &food, Tree &tree, Stone &stone, Human &human);//nie git
};