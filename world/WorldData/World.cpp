#include "World.h"
#include "../../entities/HumansData/Human.h"
#include "../../entities/civilization/Civilization.h"
#include "../resources/Food.h"
#include "../resources/Stone.h"
#include "../resources/Tree.h"

World::World()
    : grid(
          std::make_unique<ChunkRegion[]>(
              WorldConfig::CHUNK_REGIONS_COUNT))
{
}

bool World::isValid(int x, int y)
{
    return x >= 0 && y >= 0 &&
           x < ConfigConstexpr::sizeX &&
           y < ConfigConstexpr::sizeY;
}
bool World::isValidChunk(uint16_t chunkX, uint16_t chunkY)
{
    return chunkX < WorldConfig::CHUNKS_X &&
           chunkY < WorldConfig::CHUNKS_Y;
}

void World::addPossible(int x, int y, TerrainType type)
{
    static const int dirs[8][2] =
        {
            {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {-1, 1}, {1, -1}};

    for (auto &d : dirs)
    {
        int nx = x + d[0];
        int ny = y + d[1];

        if (!isValid(nx, ny))
            continue;

        if (getCell(nx, ny) == type)
        {
            possible.push_back({nx, ny});
        }
    }
}

void World::createLand()
{
    for (int y = 0; y < ConfigConstexpr::sizeY; y++)
    {
        for (int x = 0; x < ConfigConstexpr::sizeX; x++)
        {
            setCell(x, y, TerrainType::Land);
        }
    }
}
bool World::addSand(int x, int y)
{
    static const int dirs[8][2] =
        {
            {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {-1, 1}, {1, -1}};

    for (auto &d : dirs)
    {
        int nx = x + d[0];
        int ny = y + d[1];
        if (!isValid(nx, ny))
            continue;
        if (getCell(nx, ny) == TerrainType::Land)
            return true;
    }
    return false;
}
void World::updateMountainRange(
    int mountainID,
    uint16_t x,
    uint16_t y)
{
    auto &range = mountainsRanges[mountainID];

    if (x > range.maxX)
        range.maxX = x;

    if (x < range.minX)
        range.minX = x;

    if (y > range.maxY)
        range.maxY = y;

    if (y < range.minY)
        range.minY = y;
}
void World::createStruct(TerrainType type)
{
    uint32_t totalCells = ConfigConstexpr::sizeX * ConfigConstexpr::sizeY;
    uint32_t landCells = (totalCells * Config::surface.landPercent) / 100;

    int numberOfStruct;
    int totalStruct;
    int structCount;

    if (type == TerrainType::Desert)
    {
        if (Config::surface.numberOfDeserts == 0)
            return;
        numberOfStruct = Config::surface.numberOfDeserts;
        totalStruct = (landCells * Config::surface.sandPercent) / 100;
        structCount = totalStruct / Config::surface.numberOfDeserts;
    }
    else if (type == TerrainType::Mountain)
    {
        if (ConfigConstexpr::numberOfMountains == 0)
            return;
        numberOfStruct = ConfigConstexpr::numberOfMountains;
        totalStruct = (landCells * Config::surface.mountainPercent) / 100;
        structCount = totalStruct / ConfigConstexpr::numberOfMountains;
    }
    else
    {
        std::cout << "No flag added as ";
        return;
    }

    for (int i = 0; i < numberOfStruct; i++)
    {
        if (type == TerrainType::Mountain)
        {
            mountainsRanges[i].minX = UINT16_MAX;
            mountainsRanges[i].maxX = 0;
            mountainsRanges[i].minY = UINT16_MAX;
            mountainsRanges[i].maxY = 0;
        }

        int x;
        int y;

        possible.clear();

        do
        {
            x = rand() % ConfigConstexpr::sizeX;
            y = rand() % ConfigConstexpr::sizeY;
        } while (getCell(x, y) != TerrainType::Land);

        setCell(x, y, type);

        if (type == TerrainType::Mountain)
        {
            updateMountainRange(
                i,
                static_cast<uint16_t>(x),
                static_cast<uint16_t>(y));
        }

        addPossible(x, y, TerrainType::Land);

        for (int j = 0; j < structCount; j++)
        {
            if (possible.empty())
            {
                break;
            }

            int posID = rand() % possible.size();

            auto [x, y] = possible[posID];

            possible[posID] = possible.back();
            possible.pop_back();

            if (getCell(x, y) != TerrainType::Land)
                continue;

            setCell(x, y, type);

            if (type == TerrainType::Mountain)
            {
                updateMountainRange(
                    i,
                    static_cast<uint16_t>(x),
                    static_cast<uint16_t>(y));
            }

            addPossible(x, y, TerrainType::Land);
        }
    }
}

bool World::isEmpty(uint16_t x, uint16_t y)
{
    TerrainType type = getCell(x, y);
    return type != TerrainType::MountainWithStone &&
           type != TerrainType::LandWithFood &&
           type != TerrainType::LandWithTree;
}

bool World::hasBuilding(uint16_t chunkX, uint16_t chunkY)
{
    return getBuilding(chunkX, chunkY) != BuildingType::None;
}

XY World::getCellInChunk(uint16_t chunkX, uint16_t chunkY)
{
    uint16_t x = chunkX * ChunkConfig::CHUNK_SIZE;
    uint16_t y = chunkY * ChunkConfig::CHUNK_SIZE;
    return {x, y};
}

std::vector<XY> World::getCellsInChunk(uint16_t chunkX, uint16_t chunkY)
{
    std::vector<XY> cells;
    cells.reserve(ChunkConfig::CELL_COUNT);

    auto ref = getCellInChunk(chunkX, chunkY);

    for (uint16_t y = 0; y < ChunkConfig::CHUNK_SIZE; y++)
    {
        for (uint16_t x = 0; x < ChunkConfig::CHUNK_SIZE; x++)
        {
            uint16_t worldX = ref.x + x;
            uint16_t worldY = ref.y + y;

            if (worldX >= ConfigConstexpr::sizeX || worldY >= ConfigConstexpr::sizeY)
                continue;

            cells.push_back({worldX, worldY});
        }
    }

    return cells;
}

void World::cleanChunkResources(
    Food &food, Tree &tree, Stone &stone,
    uint16_t chunkX,
    uint16_t chunkY,
    Civilization &civ)
{
    auto ref = getChunkRef(chunkX, chunkY);

    auto &chunk =
        grid[ref.chunkRegionIndex]
            .chunks[ref.localChunkIndex];

    for (uint16_t i = 0; i < ChunkConfig::CELL_COUNT; i++)
    {
        TerrainType type = chunk.getCell(i);

        switch (type)
        {
        case TerrainType::LandWithFood:
        {
            civ.resources.food++;
            food.foodsCount--;

            chunk.setCell(i, TerrainType::Land);
            break;
        }

        case TerrainType::LandWithTree:
        {
            civ.resources.wood++;
            tree.treesCount--;

            chunk.setCell(i, TerrainType::Land);
            break;
        }

        case TerrainType::MountainWithStone:
        {
            civ.resources.stone++;
            stone.stonesCount--;

            chunk.setCell(i, TerrainType::Mountain);
            break;
        }

        default:
            break;
        }
    }
}