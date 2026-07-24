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
        uint16_t chunkRegionX;
        uint16_t chunkRegionY;

        uint32_t chunkRegionIndex;

        uint16_t localChunkX;
        uint16_t localChunkY;

        uint16_t localChunkIndex;
    };
    constexpr ChunkRef getChunkRef(uint16_t chunkX, uint16_t chunkY)
    {
        uint16_t chunkRegionX =
            chunkX / ChunkRegionConfig::CHUNK_REGION_SIZE;

        uint16_t chunkRegionY =
            chunkY / ChunkRegionConfig::CHUNK_REGION_SIZE;

        uint16_t localChunkX =
            chunkX % ChunkRegionConfig::CHUNK_REGION_SIZE;

        uint16_t localChunkY =
            chunkY % ChunkRegionConfig::CHUNK_REGION_SIZE;

        uint16_t chunkRegionIndex =
            chunkRegionY * WorldConfig::CHUNK_REGIONS_X + chunkRegionX;

        uint16_t localChunkIndex =
            localChunkY * ChunkRegionConfig::CHUNK_REGION_SIZE + localChunkX;

        return {
            chunkRegionX,
            chunkRegionY,

            chunkRegionIndex,

            localChunkX,
            localChunkY,

            localChunkIndex};
    }

    constexpr uint32_t getChunkRegionIndex(uint16_t chunkX, uint16_t chunkY)
    {
        uint16_t regionX = chunkX / ChunkRegionConfig::CHUNK_REGION_SIZE;
        uint16_t regionY = chunkY / ChunkRegionConfig::CHUNK_REGION_SIZE;

        return regionY * WorldConfig::CHUNK_REGIONS_X + regionX;
    }
    constexpr uint16_t getLocalChunkIndex(uint16_t chunkX, uint16_t chunkY)
    {
        uint16_t localX = chunkX % ChunkRegionConfig::CHUNK_REGION_SIZE;
        uint16_t localY = chunkY % ChunkRegionConfig::CHUNK_REGION_SIZE;

        return localY * ChunkRegionConfig::CHUNK_REGION_SIZE + localX;
    }

    struct CellRef
    {
        uint16_t chunkX;
        uint16_t chunkY;

        uint16_t chunkRegionIndex;

        uint16_t localChunkIndex;

        uint16_t localCellIndex;
    };
    constexpr CellRef getCellRef(uint16_t worldX, uint16_t worldY)
    {
        uint16_t chunkX = worldX / ChunkConfig::CHUNK_SIZE;
        uint16_t chunkY = worldY / ChunkConfig::CHUNK_SIZE;
        auto chunkRef = getChunkRef(
            chunkX,
            chunkY);

        return {
            chunkX,
            chunkY,
            chunkRef.chunkRegionIndex,
            chunkRef.localChunkIndex,
            static_cast<uint16_t>(
                (worldY % ChunkConfig::CHUNK_SIZE) * ChunkConfig::CHUNK_SIZE +
                (worldX % ChunkConfig::CHUNK_SIZE))};
    }

    inline uint32_t getChunkData(
        uint32_t region,
        uint16_t chunk)
    {
        return grid[region].chunks[chunk].data;
    }

    void setCell(uint16_t x, uint16_t y, TerrainType type)
    {
        auto ref = getCellRef(x, y);

        grid[ref.chunkRegionIndex].chunks[ref.localChunkIndex].setCell(ref.localCellIndex, type);
    }

    TerrainType getCell(uint16_t x, uint16_t y)
    {
        auto ref = getCellRef(x, y);

        return grid[ref.chunkRegionIndex].chunks[ref.localChunkIndex].getCell(ref.localCellIndex);
    }
    inline TerrainType getCellFast(
        uint16_t chunkRegionIndex,
        uint16_t localChunkIndex,
        uint16_t localCellIndex)
    {
        return grid[chunkRegionIndex]
            .chunks[localChunkIndex]
            .getCell(localCellIndex);
    }

    void setBuilding(uint16_t chunkX, uint16_t chunkY, BuildingType type)
    {
        auto ref = getChunkRef(chunkX, chunkY);

        grid[ref.chunkRegionIndex]
            .chunks[ref.localChunkIndex]
            .setBuilding(type);
    }

    BuildingType getBuilding(uint16_t chunkX, uint16_t chunkY)
    {
        auto ref = getChunkRef(chunkX, chunkY);

        return grid[ref.chunkRegionIndex]
            .chunks[ref.localChunkIndex]
            .getBuilding();
    }

    void setChunkFlag(uint16_t chunkX, uint16_t chunkY, ChunkFlag flag)
    {
        auto ref = getChunkRef(chunkX, chunkY);

        grid[ref.chunkRegionIndex]
            .chunks[ref.localChunkIndex]
            .setFlag(flag);
    }

    void clearChunkFlag(uint16_t chunkX, uint16_t chunkY, ChunkFlag flag)
    {
        auto ref = getChunkRef(chunkX, chunkY);

        grid[ref.chunkRegionIndex]
            .chunks[ref.localChunkIndex]
            .clearFlag(flag);
    }

    bool hasChunkFlag(uint16_t chunkX, uint16_t chunkY, ChunkFlag flag)
    {
        auto ref = getChunkRef(chunkX, chunkY);

        return grid[ref.chunkRegionIndex]
            .chunks[ref.localChunkIndex]
            .hasFlag(flag);
    }

    void cleanChunkResources(
        Food &food, Tree &tree, Stone &stone,
        uint16_t chunkX,
        uint16_t chunkY,
        Civilization &civ);

    std::vector<std::pair<int, int>> possible;

    void writeStatsToTxt(
        int ticks,
        int FPS,
        int humanTicks,
        Civilization &civilization,
        Human &human,
        Stone &stone,
        Food &food,
        Tree &tree);
    void init();
    bool isValid(int x, int y);
    bool isValidChunk(uint16_t chunkX, uint16_t chunkY);
    bool isChunkLand(uint16_t chunkX, uint16_t chunkY);
    void addPossible(int x, int y, TerrainType type);

    void createOcean();
    void createLand();
    bool addSand(int x, int y);
    void addSandToLand(); // on edges
    void smoothShores();

    void createStruct(TerrainType type);

    bool isEmpty(uint16_t x, uint16_t y);

    void markAllDirty(RendererSFML &renderer);

    bool hasBuilding(uint16_t chunkX, uint16_t chunkY);

    XY getCellInChunk(uint16_t chunkX, uint16_t chunkY);
    std::vector<XY> getCellsInChunk(uint16_t chunkX, uint16_t chunkY);
};