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

#include "ChunkData/Chunk.h"

class Food;
class Tree;
class Stone;
class Human;
class Civilization;
class Army;
class Monsters;

namespace WorldConfig
{
    constexpr uint32_t CHUNKS_X = (Config::sizeX + ChunkConfig::CHUNK_SIZE - 1) / ChunkConfig::CHUNK_SIZE;
    constexpr uint32_t CHUNKS_Y = (Config::sizeY + ChunkConfig::CHUNK_SIZE - 1) / ChunkConfig::CHUNK_SIZE;
    constexpr uint32_t CHUNKS_COUNT = CHUNKS_X * CHUNKS_Y;
}

class World
{
public:
    int allTicksCount = 0;
    struct XY
    {
        uint32_t x;
        uint32_t y;
    };


    Chunk grid[WorldConfig::CHUNKS_COUNT];
    struct CellRef
    {
        uint32_t chunkX;
        uint32_t chunkY;
        uint32_t chunkIndex;
        uint32_t localIndex;
    };
    constexpr CellRef getCellRef(uint32_t worldX, uint32_t worldY)
    {
        uint32_t chunkX = worldX / ChunkConfig::CHUNK_SIZE;
        uint32_t chunkY = worldY / ChunkConfig::CHUNK_SIZE;

        uint32_t localX = worldX % ChunkConfig::CHUNK_SIZE;
        uint32_t localY = worldY % ChunkConfig::CHUNK_SIZE;

        uint32_t chunkIDX = (chunkY * WorldConfig::CHUNKS_X) + chunkX;
        uint32_t localIDX = (localY * ChunkConfig::CHUNK_SIZE) + localX;

        return 
        { 
            chunkX,
            chunkY,
            chunkIDX, 
            localIDX 
        };
    }
    void setCell(uint32_t x, uint32_t y, TerrainType type)
    {
        auto ref = getCellRef(x, y);

        grid[ref.chunkIndex].setCell(ref.localIndex, type);
    }

    TerrainType getCell(uint32_t x, uint32_t y)
    {
        auto ref = getCellRef(x, y);

        return grid[ref.chunkIndex].getCell(ref.localIndex);
    }

    void setBuilding(uint32_t chunkX, uint32_t chunkY, BuildingType type)
    {
        uint32_t index = chunkY * WorldConfig::CHUNKS_X + chunkX;
        grid[index].setBuilding(type);
    }

    BuildingType getBuilding(uint32_t chunkX, uint32_t chunkY)
    {
        uint32_t index = chunkY * WorldConfig::CHUNKS_X + chunkX;

        return grid[index].getBuilding();
    }

    void setChunkFlag(uint32_t chunkX, uint32_t chunkY, ChunkFlag flag)
    {
        uint32_t index = chunkY * WorldConfig::CHUNKS_X + chunkX;

        grid[index].setFlag(flag);
    }
    void clearChunkFlag(uint32_t chunkX, uint32_t chunkY, ChunkFlag flag)
    {
        uint32_t index = chunkY * WorldConfig::CHUNKS_X + chunkX;

        grid[index].clearFlag(flag);
    }
    bool hasChunkFlag(uint32_t chunkX, uint32_t chunkY, ChunkFlag flag)
    {
        uint32_t index = chunkY * WorldConfig::CHUNKS_X + chunkX;

        return grid[index].hasFlag(flag);
    }


    std::vector <XY> dirtyCells;
    std::vector <std::pair<int, int>> possible;

    void writeStatsToTxt(int ticks, int FPS, Civilization& civilization, Human& human, Stone& stone, Food& food, Tree& tree, Army& army, Monsters& monsters);
    void init();
    bool isValid(int x, int y);
    bool isValidChunk(uint32_t chunkX, uint32_t chunkY);
    bool isChunkLand(uint32_t chunkX, uint32_t chunkY);
    void addPossible(int x, int y, TerrainType type);

    void createOcean();
    void createLand();
    bool addSand(int x, int y);
    void addSandToLand();//on edges
    void smoothShores();

    void createStruct(TerrainType type);

    bool isEmpty(uint32_t x, uint32_t y);

    void markAllDirty();

    bool hasBuilding(uint32_t chunkX, uint32_t chunkY);

    std::vector<XY> getCellsInChunk(uint32_t chunkX, uint32_t chunkY);
    void addChunkToDirtyCells(uint32_t chunkX, uint32_t chunkY);
    void addToDirtyCells(uint32_t x, uint32_t y);
};