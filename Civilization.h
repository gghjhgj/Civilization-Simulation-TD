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

class Civilization
{
public:
    Food &food;
    Stone &stone;
    Tree &tree;
    Civilization(
        Food &f,
        Stone &s,
        Tree &t)
        :
        food(f),
        stone(s),
        tree(t)
    {}
    std::mt19937 rng{ std::random_device{}() };
    struct ChunkPos
    {
        uint16_t chunkX;
        uint16_t chunkY;
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

    void markCloseAsCivZone(World& world, uint16_t chunkX, uint16_t chunkY, int rInChunks);//git
    void addChunksToPossibleVillage(World& world, uint16_t chunkX, uint16_t chunkY, int rInChunks);
    ChunkPos getBestChunkForBuilingsVillage(World& world);//git

    void buildBuilding(World& world, RendererSFML &renderer, Type type);
    void assignHumansToBuilding(Human& human, Type type);
    void getBuildingsGains();

    void buildingDecision(World& world, RendererSFML &renderer, Human& human, Food& food, Stone& stone, Tree& tree);
    void startConstruction(World& world, RendererSFML &renderer, uint16_t chunkX, uint16_t chunkY, Type type);
    void endConstruction(World& world, RendererSFML &renderer, Human& human, uint16_t chunkX, uint16_t chunkY, Type type);
};