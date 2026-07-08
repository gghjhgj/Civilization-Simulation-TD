#pragma once
#include "BuildingsType.h"
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
    struct HumanData
    {
        int index;
        Tasks task = foodFinding;
        int id;
        int HP = Config::humanHP;
        int Stamina = Config::humanStamina;
        int points = 10000;
        int moves = 0;
        int targetIndex = -1;
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


    void addHuman(World &world, int index, int vecId);
    void createHuman(World &world, Civilization &civilization);
    void eraseHuman(World &world, Civilization &civilization, int vecID);
    void humanRespawn(World &world, Civilization &civilization);
    int humanFindResource(World &world, int startIndex, ResourceType type);
    bool gotResource(int h, int f);
    int humanMoveDecision(int fromIndex, int targetIndex, int i);
    void humanMove(World &world, Civilization &civilization, Food &food, Tree &tree, Stone &stone, Human &human);

};