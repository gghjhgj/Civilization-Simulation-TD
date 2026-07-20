#pragma once

#include "WorldData/ChunkData/Chunk.h"
#include "HumansData/HumanTypes.h"
#include "Food.h"
#include "Tree.h"
#include "Stone.h"
class Human;
class World;
class Walls;
class Army;

enum Type
{
    HOUSE,
    FARM,
    SAWMILL,
    MINE,

    COUNT
};
inline BuildingType GetBuildingType(Type t) {
    switch(t) {
        case Type::HOUSE:   return BuildingType::House;
        case Type::FARM:    return BuildingType::Farm;
        case Type::SAWMILL: return BuildingType::Sawmill;
        case Type::MINE:    return BuildingType::Mine;
        default:            return BuildingType::None;
    }
}
inline Type GetTypeBuilding(BuildingType b) {
    switch(b) {
        case BuildingType::House:   return Type::HOUSE;
        case BuildingType::Farm:    return Type::FARM;
        case BuildingType::Sawmill: return Type::SAWMILL;
        case BuildingType::Mine:    return Type::MINE;
        default:            return Type::COUNT;
    }
}

class Civilization
{
public:
    std::mt19937 rng{ std::random_device{}() };
    struct ChunkPos
    {
        uint32_t chunkX;
        uint32_t chunkY;
    };
    ChunkPos spawn;
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

    BuildingResources buildingsCost[COUNT]{};
    BuildingResources buildingsGains[COUNT]{};
    int buildingsCount[COUNT]{};
    int maxHumans[COUNT]{};
    int hitsForBuilding[COUNT]{};

    int workersAssigned[COUNT]{};
    int realWorkers[COUNT]{};

    int constructions[COUNT];

    std::vector<ChunkPos> bestChunksForBuildingsVillage;

    int mostEastCivZone = -1;
    int mostWestCivZone = -1;
    int mostNorthCivZone = -1;
    int mostSouthCivZone = -1;


    void addCivilization(World& world, int index);//git
    void createCivilization(World& world);//git
    void initBuildings();//nowe git

    void addWorkers(Human& human, HumanType type);//nowe git

    void civilizationDecision(Human& human, Food& food, Stone& stone, Tree& tree);//git

    void markCloseAsCivZone(World& world, uint32_t chunkX, uint32_t chunkY, int rInChunks);//git
    void addChunksToPossibleVillage(World& world, uint32_t chunkX, uint32_t chunkY, int rInChunks);
    ChunkPos getBestChunkForBuilingsVillage(World& world);//git

    void buildBuilding(World& world, RendererSFML &renderer, Type type);
    void assignHumansToBuilding(Human& human, Type type);
    void getBuildingsGains();

    void buildingDecision(World& world, RendererSFML &renderer, Human& human, Food& food, Stone& stone, Tree& tree);
    void startConstruction(World& world, RendererSFML &renderer, uint32_t chunkX, uint32_t chunkY, Type type);
    void endConstruction(World& world, RendererSFML &renderer, Human& human, uint32_t chunkX, uint32_t chunkY, Type type);
};