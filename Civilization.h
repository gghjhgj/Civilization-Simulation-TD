#pragma once



#include "Food.h"
#include "Tree.h"
#include "Stone.h"
class Human;
class World;
class Streets;
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
    enum BuildingsType
    {
        house,
        farm,
        sawmill,
        mine,
        factory,
        woodWall,
        stoneWall
    };
    struct Farm
    {
        int index = 0;
        int workersAssigned = 0;
        int realWorkers = 0;
    };
    struct Sawmill
    {
        int index = 0;
        int workersAssigned = 0;
        int realWorkers = 0;
    };
     struct Mine
    {
        int index = 0;
        int workersAssigned = 0;
        int realWorkers = 0;
    };
    struct Factory
    {
        int index = 0;
        int workersAssigned = 0;
        int realWorkers = 0;
    };
    struct House
    {
        int index = 0;
    };
    struct Construction
    {
        int index = 0;
        BuildingsType result = BuildingsType::house;
        int hitsForCreating = 100;
        std::vector<int> humansIDS;
    };
    int housesDuringConstruction = 0;
    int farmsDuringConstruction = 0;
    int sawmillsDuringConstruction = 0;
    int minesDuringConstruction = 0;
    int factoriesDuringConstruction = 0;

    struct Buildings
    {
        std::vector<House> houses;
        std::vector<Farm> farms;
        std::vector<Sawmill> sawmills;
        std::vector<Mine> mines;
        std::vector<Factory> factories;
        std::vector<Construction> constructions;
    };
    Buildings buildings;
    int emptyFarmsStartingID = 0;
    int farmWorkers = 0;

    int emptyMinesStartingID = 0;
    int mineWorkers = 0;

    int emptySawmillsStartingID = 0;
    int sawmillWorkers = 0;

    int emptyFactoriesStartingID = 0;
    int factoriesWorkers = 0;
    int lastFactoryIndex = -1;

    std::vector<int> bestTilesForBuildingsVillage;

    int mostEastCivZone = -1;
    int mostWestCivZone = -1;
    int mostNorthCivZone = -1;
    int mostSouthCivZone = -1;


    void addCivilization(World &world, int index);
    void createCivilization(World &world);

    void makeEverybodyFoodWorker(Human &human);
    void addMoreFoodWorkers(Human &human);

    void makeEverybodyTreeWorker(Human &human);
    void addMoreTreeWorkers(Human &human);

    void makeEverybodyStoneWorker(Human &human);
    void addMoreStoneWorkers(Human &human);

    void addMoreBuilders(Human &human);

    void civilizationDecision(Human &human, Food &food, Stone &stone, Tree &tree);

    /*
    int getBestTileForBuilding(World &world);
    int getOutskirtsTileForBuilding(World &world);
    */
    void markCloseAsCivZone(World &world, int index, int r);
    void addTilesToPossibleVillage(World &world, int index, int r);
    int getBestTileForBuilingsVillage(World &world);

    void addHouse(World &world, int idx);
    void buildHouse(World &world);
    void assignHumansToHouse(Human &human);

    void addFarm(World &world, int idx);
    void buildFarm(World &world);
    void farmsGains();
    void assignHumansToFarm(Human &human);

    void addSawmill(World &world, int idx);
    void buildSawmill(World &world);
    void sawmillsGains();
    void assignHumansToSawmills(Human &human);

    void addMine(World &world, int idx);
    //int bestNextMinePlace(World &world);
    void buildMine(World &world);
    void minesGains();
    void assignHumansToMine(Human &human);

    /*
    void addFactory(World &world, Streets &streets, Human &human, int idx);
    void buildFactory(World &world);
    void factoriesGains();
    void assignHumansToFactory(Human &human);
    */



    void buildingDecision(World &world, Human &human, Food &food, Stone &stone, Tree &tree);
    void startConstruction(World &world, int idx, BuildingsType type);
    void endConstruction(World &world, Human &human, Streets &streets, int idx, BuildingsType type, int id);
};