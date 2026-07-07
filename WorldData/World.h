#pragma once
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <utility>
#include <iostream>
#include <random>
#include <algorithm>
#include <fstream>
#include "Config.h"

#include "Cell.h"

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
    using Cell = ::Cell;

    int allTicksCount = 0;

    std::vector<int> waters;
    std::vector<int> lands;
    std::vector<int> sands;
    std::vector<int> mountains;
    
    std::vector<int> foods;
    std::vector<int> trees;
    std::vector<int> stones;


    inline static Cell grid[Config::sizeX * Config::sizeY];



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

    void makeAllHumansDirty(Human &human);
    
    bool checkIfThereAreNoBuildings(int idx);

    void addToDirtyCells(int index);
};