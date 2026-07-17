#pragma once
#include <cstdint>
class Config
{
    public:
    static constexpr int ticksForRender = 1500;//nie ma juz
    static constexpr float FPS = 30.f;
    static constexpr int ticksForCivilizationDecision = 1000;
    static constexpr int ticksForBuildingDecision = 10;
    static constexpr int ticksForAssigningDecision = 20;
    static constexpr int ticksForResourcesGainsFromBuildings = 1000;
    static constexpr int ticksForNewHumans = 3000;

    static constexpr int sizeX = 1800;
    static constexpr int sizeY = 999;
    
    static constexpr int WindowSizeX = sizeX;
    static constexpr int WindowSizeY = sizeY;

    //surface configs
    static constexpr int landPercent = 100;
    static constexpr int waterPercentInLand = 5;
    static constexpr int mountainPercent = 3;
    static constexpr int sandPercent = 7;//sand will also appear on the edge of land

    static constexpr int numberOfLands = 1;
    static constexpr int numberOfLakes = 0;
    static constexpr int numberOfMountains = 2;
    static constexpr int numberOfDesserts = 6;


    //resources configs
    static constexpr int foodCount = 30000;
    static constexpr int maxFood = 100000;
    static constexpr int foodRespawn = 5;
    static constexpr int maxFoodSpawnTries = 5;

    static constexpr int treeCount = 100000;
    static constexpr int forestCount = 10;//est
    static constexpr int maxTree = 150000;
    static constexpr int treeRespawn = 1;
    static constexpr int treeHP = 10;
    static constexpr int maxTreeSpawnTries = 5;

    static constexpr int stoneCount = 2500;
    static constexpr int maxStone = 5000;
    static constexpr int StoneRespawn = 1;
    static constexpr int StoneHP = 20;
    static constexpr int maxStoneSpawnTries = 10;


    static constexpr uint32_t civSpawnChunkX = 20;
    static constexpr uint32_t civSpawnChunkY = 20;
    static constexpr int partOfHumansChangingJobs = 100; // 0.01
    static constexpr int civilizationCount = 1;
    static constexpr int civilizationPlaceX = 2;
    static constexpr int civilizationPlaceY = 2;
    static constexpr int civilizationPlaceHP = 30;
    static constexpr int humanCount = 1000000;
    static constexpr int humanHP = 100;
    static constexpr int humanStamina = 1000;
    static constexpr int vision = 2;
    static constexpr int chanceToChangeDir = 10; //%
    static constexpr int chanceForRandomMoveHavingTarget = 30; // %

    static constexpr int stoneNeededForHouse = 10;
    static constexpr int woodNeededForHouse = 100;
    static constexpr int maxHumansInHouse = 5;

    static constexpr float outskirts = 0.75;

    static constexpr int foodNeededForFarm = 100;
    static constexpr int woodNeededForFarm = 50;
    static constexpr int maxFarmSpawnTries = 5;
    static constexpr int maxFarmWorkersOnFarm = 10;
    static constexpr float foodPerFarmWorker = 0.1;

    static constexpr int stoneNeededForSawmill = 100;
    static constexpr int woodNeededForSawmill = 1000;
    static constexpr int maxSawmillSpawnTries = 5;
    static constexpr int maxSawmillWorkersInSawmill = 100;
    static constexpr float woodPerSawmillWorker = 0.1;

    static constexpr int stoneNeededForMine = 10000;
    static constexpr int woodNeededForMine = 100000;
    static constexpr int maxMineSpawnTries = 5;
    static constexpr int maxMineWorkersInMine  = 1000;
    static constexpr float stonePerMineWorker = 0.01;

    static constexpr int stoneNeededForFactory = 100;
    static constexpr int woodNeededForFactory = 500;
    static constexpr int maxFactoryWorkersInFactory = 10000;
    static constexpr int factoryMaxSpawnTries = 10;
    static constexpr int factoriesMinDistance = 3;
    static constexpr int factoriesMaxDistance = 10;


    static constexpr int hitsForBuildingHouse = 2;
    static constexpr int hitsForBuildingFarm = 1;
    static constexpr int hitsForBuildingSawmill = 5;
    static constexpr int hitsForBuildingMine = 10;
    static constexpr int hitsForBuildingFactory = 100;
    static constexpr int hitsForBuildingWoodWall = 2;
    static constexpr int hitsForBuildingStoneWall = 5;

    static constexpr int woodForCar = 100;
    static constexpr int maxCarCargo = 1000;
    static constexpr int foodCargo = 1;
    static constexpr int woodCargo = 2;
    static constexpr int stoneCargo = 5;


    //////////walls
    static constexpr int woodWallHP = 1000;
    static constexpr int stoneWallHP = 5000;
    static constexpr int woodNeededForWoodWall = 5;
    static constexpr int stoneNeededForStoneWall = 10;
    static constexpr int ticksForBuildingWall = 300000;



    /////////army
    static constexpr int humanInArmyHP = 1000;
    static constexpr int humanInArmyDamage = 5;
    static constexpr int maxHumanCountAddedToArmy = 1;
    static constexpr int ticksForAddingHumansToArmy = 500;
    static constexpr int maxHumansInArmy = 1000000;

    static constexpr int countOfTroopsInOneLine = 20;



    ///////monsters

    static constexpr int normalMonsterHP = 5;
    static constexpr int normalMonsterDamage = 1;

    static constexpr int normalMonstersCreated = 500000;


    static constexpr int giantMonsterHP = 200;
    static constexpr int giantMonsterDamage = 5;
    
    static constexpr int giantMonstersCreated = 20;


    static constexpr int maxUnits = 500000;
};