#pragma once

#include "BuildingsType.h"
#include "Tasks.h"

#include "Food.h"
#include "Tree.h"
#include "Stone.h"
class Human;
class World;
class Walls;
class Army;
class Civilization
{
    public:
    std::mt19937 rng{std::random_device{}()};
    int spawn = -1;
    struct Resources
    {
        long long int food = 0;
        long long int wood = 0;
        long long int stone = 0;
    };
    Resources resources;

    struct BuildingResources
    {
        float food = 0.0;
        float wood = 0.0;
        float stone = 0.0;
    };
    BuildingResources buildingsCost[BuildingsType::COUNT]{};
    BuildingResources buildingsGains[BuildingsType::COUNT]{};
    int maxHumans[BuildingsType::COUNT]{};
    int hitsForBuilding[BuildingsType::COUNT]{};

    std::vector<int> buildingsIndexes[BuildingsType::COUNT]{};
    int workersAssigned[BuildingsType::COUNT]{};
    int realWorkers[BuildingsType::COUNT]{};

    struct Construction
    {
        int index;
        int HP;
    };
    std::vector<Construction> constructions[BuildingsType::COUNT];

    std::vector<int> bestTilesForBuildingsVillage;

    int mostEastCivZone = -1;
    int mostWestCivZone = -1;
    int mostNorthCivZone = -1;
    int mostSouthCivZone = -1;


    void addCivilization(World &world, int index);//git
    void createCivilization(World &world);//git
    void initBuildings();//nowe git

    void addWorkers(Human &human, Tasks task);//nowe git

    void civilizationDecision(Human &human, Food &food, Stone &stone, Tree &tree);//git

    void markCloseAsCivZone(World &world, int index, int r);//git
    void addTilesToPossibleVillage(World &world, int index, int r);//git
    int getBestTileForBuilingsVillage(World &world);//git

    void addBuilding(World &world, int idx, BuildingsType type);
    void buildBuilding(World &world, BuildingsType type);
    void assignHumansToBuilding(Human &human, BuildingsType type);
    void getBuildingsGains();

    void buildingDecision(World &world, Human &human, Food &food, Stone &stone, Tree &tree);
    void startConstruction(World &world, int idx, BuildingsType type);
    void endConstruction(World &world, Human &human, int idx, BuildingsType type, int id);
};