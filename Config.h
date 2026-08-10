#pragma once
#include <cstdint>
class Config
{
    public:
    static constexpr float FPS = 30.f;
    static constexpr int ticksForCivilizationDecision = 1000;//humanticks
    static constexpr int ticksForBuildingDecision = 10;
    static constexpr int ticksForAssigningDecision = 1000;//humanticks
    static constexpr int ticksForResourcesGainsFromBuildings = 1000;
    static constexpr int ticksForNewHumans = 1000;//humanticks

    static constexpr int sizeX = 63246;
    static constexpr int sizeY = 63246;
    
    static constexpr int WindowSizeX = 1920;
    static constexpr int WindowSizeY = 1080;

    //surface configs
    static constexpr int landPercent = 100;
    static constexpr int waterPercentInLand = 5;
    static constexpr int mountainPercent = 8;
    static constexpr int sandPercent = 7;//sand will also appear on the edge of land

    static constexpr int numberOfLands = 1;
    static constexpr int numberOfLakes = 0;
    static constexpr int numberOfMountains = 15;
    static constexpr int numberOfDesserts = 7;


    //resources configs
    static constexpr int foodCount = 60000000;
    static constexpr int maxFood = 200000000;
    static constexpr int foodRespawn = 20;
    static constexpr int maxFoodSpawnTries = 5;

    static constexpr int treeCount = 200000000;
    static constexpr int forestCount = 100;
    static constexpr int maxTree = 300000000;
    static constexpr int treeRespawn = 10;
    static constexpr int maxTreeSpawnTries = 5;

    static constexpr int stoneCount = 100000;
    static constexpr int maxStone = 200000;
    static constexpr int StoneRespawn = 2;
    static constexpr int StoneHP = 20;
    static constexpr int maxStoneSpawnTries = 10;


    static constexpr uint16_t civSpawnChunkX = 20;
    static constexpr uint16_t civSpawnChunkY = 20;
    static constexpr int partOfHumansChangingJobs = 100; // 0.01
    static constexpr int civilizationPlaceX = 2;
    static constexpr int civilizationPlaceY = 2;

    static constexpr int humanCount = 1000000;
    static constexpr int GRAIN = 128;
    static constexpr int humanHP = 100;
    static constexpr int humanStamina = 1000;
    static constexpr int vision = 1;
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

    static constexpr int maxUnits = 500000;

    static constexpr int MAX_THREAD_COUNT = 12;
};