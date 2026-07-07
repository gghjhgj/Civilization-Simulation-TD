#pragma once
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
    enum Tasks
    {
        foodFinding,
        treeFinding,
        stoneFinding,
        goingToBuilding,
        building,
    };
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
        int targetBuildingID = 0;
        int buildingBuildersID = -1;
    };
    std::vector <HumanData> humans;

    
    int humansCount = 0;
    int humansHavingHouseCount = 0;
    /*
    struct Jobs
    {
    std::vector <int> foodWorkers;
    std::vector <int> treeWorkers;
    std::vector <int> stoneWorkers;
    std::vector <int> buildWorkers;
    std::vector <int> farmWorkers;
    //std::vector <int> pathWorkers;
    };
    */
    void addHuman(World &world, int index, int vecId);
    void createHuman(World &world, Civilization &civilization);
    void eraseHuman(World &world, Civilization &civilization, int vecID);
    void humanRespawn(World &world, Civilization &civilization);
    int humanFindFood(World &world, int startIndex);
    int humanFindTree(World &world, int startIndex);
    int humanFindStone(World &world, int startIndex);
    bool gotResource(int h, int f);
    int humanMoveDecision(int fromIndex, int targetIndex, int i);
    void humanMove(World &world, Civilization &civilization, Food &food, Tree &tree, Stone &stone, Human &human);

};