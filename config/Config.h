#pragma once

#include <cstdint>
#include <string>

struct SimulationConfig
{
int ticksForBuildingDecision;
int ticksForResourcesGainsFromBuildings;
int ticksForNewHumans;
};

struct RenderingConfig
{
float fps;
int windowSizeX;
int windowSizeY;
};

struct SurfaceConfig
{
int landPercent;
int mountainPercent;
int sandPercent;
int numberOfDeserts;
};

struct FoodConfig
{
int count;
int max;
int respawn;
int maxSpawnTries;
};

struct TreeConfig
{
int count;
int forestCount;
int max;
int respawn;
int maxSpawnTries;
};

struct StoneConfig
{
int count;
int max;
int respawn;
int maxSpawnTries;
};

struct ResourcesConfig
{
FoodConfig food;
TreeConfig trees;
StoneConfig stone;
};

struct CivilizationConfig
{
uint16_t spawnChunkX;
uint16_t spawnChunkY;
};

struct HumansConfig
{
int count;
int grain;
int range;
int maxSpawnRange;
bool respawn;
std::string humanRespawnType;
float humanRespawnRoot;
int humanRespawnDivisor;
};

struct HouseConfig
{
int stoneRequired;
int woodRequired;
int maxHumans;
};

struct FarmConfig
{
int foodRequired;
int woodRequired;
int maxSpawnTries;
int maxWorkers;
float foodPerWorker;
};

struct SawmillConfig
{
int stoneRequired;
int woodRequired;
int maxSpawnTries;
int maxWorkers;
float woodPerWorker;
};

struct MineConfig
{
int stoneRequired;
int woodRequired;
int maxSpawnTries;
int maxWorkers;
float stonePerWorker;
};

struct BuildingsConfig
{
HouseConfig house;
FarmConfig farm;
SawmillConfig sawmill;
MineConfig mine;
bool buildBuildings;
bool spawnWithResourcesFor2Buildings;
};

struct ThreadsConfig
{
int cores;
int threads;

int coresForSimLoop;
int threadsForSimLoop;

int coresForHumanLoop;
int threadsForHumanLoop;

bool readDeviceThreadCount;

};

struct HungerConfig
{
    int firstTickHumansEat;
    int ticksForHumansToEat;
    int foodNeededForHumansToEat;
    int foodReductionForHumansWithHouse;
    int chancesForCivilizationWhenNoFood;
};

struct SystemConfig
{
    bool crashHandler;
};
class Config
{
public:
static void load(const std::string& path);

static SimulationConfig simulation;
static RenderingConfig rendering;
static SurfaceConfig surface;
static ResourcesConfig resources;
static CivilizationConfig civilization;
static HumansConfig humans;
static BuildingsConfig buildings;
static ThreadsConfig threads;
static HungerConfig hunger;
static SystemConfig system;
};
