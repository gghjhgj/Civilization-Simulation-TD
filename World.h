#pragma once
#include <glad/glad.h>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <utility>
#include <iostream>
#include <random>
#include <algorithm>
#include <fstream>
#include "Config.h"

class Streets;
class Food;
class Tree;
class Stone;
class Human;
class Civilization;
class Army;
class Monsters;

class World
{
    public:
    int allTicksCount = 0;
    enum CellFlags
    {
        Water = 1 << 0,
        Land = 1 << 1,
        Sand = 1 << 2,
        Mountain = 1 << 3
    };
    std::vector<int> waters;
    std::vector<int> lands;
    std::vector<int> sands;
    std::vector<int> mountains;

    struct Buildings
    {
        uint8_t House = 0;
        uint8_t Farm = 0;
        uint8_t Sawmill = 0;
        uint8_t Mine = 0;
        uint8_t Factory = 0;
        uint8_t Construction = 0;
    };
    struct Streets
    {
        uint8_t Street = 0;
    };
    struct Walls
    {
        int woodWallHP = 0;
        int stoneWallHP = 0;
    };
    struct ArmyTile
    {
        int armyVecID = -1;
        int armyProf = -1;
    };
    
    struct Cell
    {
        uint8_t food;
        uint8_t treeHP;
        uint8_t stoneHP;

        uint8_t civilizationPlace;

        int humanIndex = -1;

        int civZone = 0;
        Buildings buildings;
        Walls walls;

        uint8_t flags;

        //int popularity = 0;
        //int indexInBucket = 0;
        //ArmyTile armyTile;
        //Streets streets;
    };
    std::vector<int> foods;
    std::vector<int> trees;
    std::vector<int> stones;


    static Cell grid[Config::sizeX * Config::sizeY];
    static unsigned int GPU_World_Buffer_ID;



    std::vector<std::vector<int>> popularityRanking;
    int currentMaxPopularity = 0;

    inline int index(int x, int y) const
    {
        return y * Config::sizeX + x;
    }

    std::vector <std::pair<int, int>> possible;

    std::vector <int> dirtyCells;

    bool visited[Config::sizeX * Config::sizeY] = {};
    
    void writeStatsToTxt(int ticks, int FPS, Civilization &civilization, Human &human, Stone &stone, Food &food, Tree &tree, Army &army, Monsters &monsters);
    void init();
    bool isValid(int x, int y);
    bool isValid(int id);
    void addPossible(int x, int y, CellFlags flag);

    void createOcean();
    void createLand();
    bool addSand(int x, int y);
    void addSandToLand();//on edges
    void smoothShores();

    void createStruct(CellFlags flag);
    void surfaceVectorsInit();

    bool isEmpty(int id);
    bool isEmpty(int x, int y);

    void markAllDirty();
    

    void updateTilePopularity(int idx);



    void makeAllHumansDirty(Human &human);
    

    bool checkIfThereAreNoBuildings(int idx);

    void addToDirtyCells(int index);
};