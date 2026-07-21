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
#include "../RendererSFML.h"
#include "../XY/XY.h"
#include "ChunkData/Chunk.h"
#include "ChunkData/ChunkRegion.h"

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

    constexpr uint32_t CHUNK_REGIONS_X = (CHUNKS_X + ChunkRegionConfig::CHUNK_REGION_SIZE - 1) / ChunkRegionConfig::CHUNK_REGION_SIZE;
    constexpr uint32_t CHUNK_REGIONS_Y = (CHUNKS_Y + ChunkRegionConfig::CHUNK_REGION_SIZE - 1) / ChunkRegionConfig::CHUNK_REGION_SIZE;
    constexpr uint32_t CHUNK_REGIONS_COUNT = CHUNK_REGIONS_X * CHUNK_REGIONS_Y;
}

class World
{
public:
    int allTicksCount = 0;
    ChunkRegion grid[WorldConfig::CHUNK_REGIONS_COUNT];

    struct ChunkRef
    {
        uint32_t chunkRegionX;
        uint32_t chunkRegionY;

        uint32_t chunkRegionIndex;

        uint32_t localChunkX;
        uint32_t localChunkY;

        uint32_t localChunkIndex;
    };
    constexpr ChunkRef getChunkRef(uint32_t chunkX, uint32_t chunkY)
    {
        uint32_t chunkRegionX =
            chunkX / ChunkRegionConfig::CHUNK_REGION_SIZE;

        uint32_t chunkRegionY =
            chunkY / ChunkRegionConfig::CHUNK_REGION_SIZE;

        uint32_t localChunkX =
            chunkX % ChunkRegionConfig::CHUNK_REGION_SIZE;

        uint32_t localChunkY =
            chunkY % ChunkRegionConfig::CHUNK_REGION_SIZE;

        uint32_t chunkRegionIndex =
            chunkRegionY * WorldConfig::CHUNK_REGIONS_X
            + chunkRegionX;

        uint32_t localChunkIndex =
            localChunkY * ChunkRegionConfig::CHUNK_REGION_SIZE
            + localChunkX;

        return
        {
            chunkRegionX,
            chunkRegionY,

            chunkRegionIndex,

            localChunkX,
            localChunkY,

            localChunkIndex
        };
    }

    struct CellRef
    {
        uint32_t chunkX;
        uint32_t chunkY;

        uint32_t chunkRegionIndex;

        uint32_t localChunkIndex;

        uint32_t localCellIndex;
    };
    constexpr CellRef getCellRef(uint32_t worldX, uint32_t worldY)
    {
        uint32_t chunkX = worldX / ChunkConfig::CHUNK_SIZE;
        uint32_t chunkY = worldY / ChunkConfig::CHUNK_SIZE;
        auto chunkRef = getChunkRef(
            worldX / ChunkConfig::CHUNK_SIZE,
            worldY / ChunkConfig::CHUNK_SIZE);

        return
        {
            chunkX,
            chunkY,
            chunkRef.chunkRegionIndex,
            chunkRef.localChunkIndex,
            (worldY % ChunkConfig::CHUNK_SIZE) * ChunkConfig::CHUNK_SIZE +
            (worldX % ChunkConfig::CHUNK_SIZE)
        };
    }


    void setCell(uint32_t x, uint32_t y, TerrainType type)
    {
        auto ref = getCellRef(x, y);

        grid[ref.chunkRegionIndex].chunks[ref.localChunkIndex].setCell(ref.localCellIndex, type);
    }

    TerrainType getCell(uint32_t x, uint32_t y)
    {
        auto ref = getCellRef(x, y);

        return grid[ref.chunkRegionIndex].chunks[ref.localChunkIndex].getCell(ref.localCellIndex);
    }

    void setBuilding(uint32_t chunkX, uint32_t chunkY, BuildingType type)
    {
        auto ref = getChunkRef(chunkX, chunkY);

        grid[ref.chunkRegionIndex]
            .chunks[ref.localChunkIndex]
            .setBuilding(type);
    }

    BuildingType getBuilding(uint32_t chunkX, uint32_t chunkY)
    {
        auto ref = getChunkRef(chunkX, chunkY);

        return grid[ref.chunkRegionIndex]
            .chunks[ref.localChunkIndex]
            .getBuilding();
    }

    void setChunkFlag(uint32_t chunkX, uint32_t chunkY, ChunkFlag flag)
    {
        auto ref = getChunkRef(chunkX, chunkY);

        grid[ref.chunkRegionIndex]
            .chunks[ref.localChunkIndex]
            .setFlag(flag);
    }

    void clearChunkFlag(uint32_t chunkX, uint32_t chunkY, ChunkFlag flag)
    {
        auto ref = getChunkRef(chunkX, chunkY);

        grid[ref.chunkRegionIndex]
            .chunks[ref.localChunkIndex]
            .clearFlag(flag);
    }

    bool hasChunkFlag(uint32_t chunkX, uint32_t chunkY, ChunkFlag flag)
    {
        auto ref = getChunkRef(chunkX, chunkY);

        return grid[ref.chunkRegionIndex]
            .chunks[ref.localChunkIndex]
            .hasFlag(flag);
    }


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

    void markAllDirty(RendererSFML &renderer);

    bool hasBuilding(uint32_t chunkX, uint32_t chunkY);

    XY getCellInChunk(uint32_t chunkX, uint32_t chunkY);
    std::vector<XY> getCellsInChunk(uint32_t chunkX, uint32_t chunkY);
};