#include "World.h"
#include "HumansData/Human.h"
#include "Civilization.h"
#include "Food.h"
#include "Stone.h"
#include "Tree.h"



void World::writeStatsToTxt(
    int ticks, 
    int FPS, 
    int humanTicks,
    Civilization& civilization, 
    Human& human, 
    Stone& stone, 
    Food& food, 
    Tree& tree
)
{
    std::ofstream statsFile("stats.txt");

    if (statsFile.is_open())
    {
        statsFile << "=== STATYSTYKI SYMULACJI ===\n";
        statsFile << "Ticks per second " << ticks << "\n";
        statsFile << "Human ticks per second " << humanTicks << "\n";
        statsFile << "FPS: " << FPS << "\n";
        statsFile << "Total Ticks: " << allTicksCount << "\n";
        statsFile << "Total human ticks: " << human.humanTicks << "\n\n";

        statsFile << "Ilosc jedzenia: " << food.foodsCount << "\n";
        statsFile << "Ilosc drzew: " << tree.treesCount << "\n";
        statsFile << "Ilosc kamieni: " << stone.stonesCount << "\n \n";

        statsFile << "ilosc ludzi: " << human.humansCount << "\n";
        statsFile << "ilosc ludzi na mapie: " << human.foodCollectors.posX.size() +
                                                human.woodCollectors.posX.size() +
                                                human.stoneCollectors.posX.size() +
                                                human.builders.posX.size() +
                                                human.assigned.posX.size() << "\n";
        statsFile << "food collectors: " << human.foodCollectors.posX.size() << "\n";
        statsFile << "wood collectors: " << human.woodCollectors.posX.size() << "\n";
        statsFile << "stone collectors: " << human.stoneCollectors.posX.size() << "\n";
        statsFile << "builders: " << human.builders.posX.size() << "\n";
        statsFile << "assigned: " << human.assigned.posX.size() << "\n";

        statsFile << "ilosc ludzi posiadajacych dom: " << human.humansHavingHouseCount << "\n";
        statsFile << "ilosc ludzi w strukturach: " << "\n";
        statsFile << "ilosc ludzi na farmach: " << civilization.realWorkers[FARM] << "\n";
        statsFile << "ilosc ludzi w tartakach: " << civilization.realWorkers[SAWMILL] << "\n";
        statsFile << "ilosc ludzi w kopalniach " << civilization.realWorkers[MINE] << "\n\n";

        statsFile << "zasoby:\n";
        statsFile << "jedzenie: " << civilization.resources.food << "\n";
        statsFile << "drewno: " << civilization.resources.wood << "\n";
        statsFile << "kamienie: " << civilization.resources.stone << "\n\n";

        statsFile << "budynki:\n";
        statsFile << "domy w budowie: " << civilization.constructions[HOUSE] << "\n";
        statsFile << "farmy w budowie: " << civilization.constructions[FARM] << "\n";
        statsFile << "tartaki w budowie: " << civilization.constructions[SAWMILL] << "\n";
        statsFile << "kopalnie w budowie: " << civilization.constructions[MINE] << "\n";
        statsFile << "domy " << civilization.buildingsCount[HOUSE] << "\n";
        statsFile << "farmy " << civilization.buildingsCount[FARM] << "\n";
        statsFile << "tartaki " << civilization.buildingsCount[SAWMILL] << "\n";
        statsFile << "kopalnie " << civilization.buildingsCount[MINE] << "\n";

        /*
        statsFile << "armia:\n";
        statsFile << "ilosc ludzi w armii: " << army.armyRegistry[Army::ArmyProfession::soldier].humansCount +
            army.armyRegistry[Army::ArmyProfession::archer].humansCount << "\n";
        statsFile << "ilosc zolnierzy: " << army.armyRegistry[Army::ArmyProfession::soldier].humansCount << "\n";
        statsFile << "ilosc lucznikow: " << army.armyRegistry[Army::ArmyProfession::archer].humansCount << "\n\n";

        statsFile << "potwory:\n";
        statsFile << "ilosc potworow lacznie: " << monsters.monstersRegistry[Monsters::MonstersTypes::normalMonster].monstersCount +
            monsters.monstersRegistry[Monsters::MonstersTypes::giantMonster].monstersCount << "\n";
        statsFile << "ilosc zwyklych potworow: " << monsters.monstersRegistry[Monsters::MonstersTypes::normalMonster].monstersCount << "\n";
        statsFile << "ilosc gigantow: " << monsters.monstersRegistry[Monsters::MonstersTypes::giantMonster].monstersCount << "\n";
        */


        statsFile.close();
    }
}
void World::init()
{
    //popularityRanking.clear();
    //popularityRanking.resize(1);
    for (auto& chunk : grid)
    {
        //chunk.chunks.data = 0;
    }
}
bool World::isValid(int x, int y)
{
    return x >= 0 && y >= 0 &&
        x < Config::sizeX &&
        y < Config::sizeY;
}
bool World::isValidChunk(uint16_t chunkX, uint16_t chunkY)
{
    return 
        chunkX < WorldConfig::CHUNKS_X &&
        chunkY < WorldConfig::CHUNKS_Y;
}
bool World::isChunkLand(uint16_t chunkX, uint16_t chunkY)
{
    for (uint32_t y = 0; y < ChunkConfig::CHUNK_SIZE; y++)
    {
        for (uint32_t x = 0; x < ChunkConfig::CHUNK_SIZE; x++)
        {
            auto ref = getCellInChunk(chunkX, chunkY);

            if (ref.x >= Config::sizeX || ref.y >= Config::sizeY)
                continue;

            if (getCell(ref.x, ref.y) == TerrainType::Water)
                return false;
        }
    }

    return true;
}
void World::addPossible(int x, int y, TerrainType type)
{
    static const int dirs[8][2] =
    {
        {1,0}, {-1,0}, {0,1}, {0,-1},
        {1,1}, {-1,-1}, {-1,1}, {1,-1}
    };

    for (auto& d : dirs)
    {
        int nx = x + d[0];
        int ny = y + d[1];

        if (!isValid(nx, ny))
            continue;

        if (getCell(nx, ny) == type)
        {
            possible.push_back({ nx, ny });
        }
    }
}
void World::createOcean()
{
    for (int y = 0; y < Config::sizeY; y++)
    {
        for (int x = 0; x < Config::sizeX; x++)
        {
            setCell(x, y, TerrainType::Water);
        }
    }
}
void World::createLand()
{
    int totalCells = Config::sizeX * Config::sizeY;
    int totalLand = (totalCells * Config::landPercent) / 100;
    int landCount = totalLand / Config::numberOfLands;
    int landAdded = 0;

    for (int i = 0; i < Config::numberOfLands; i++)
    {
        int x;
        int y;
        possible.clear();
        do
        {
            x = rand() % Config::sizeX;
            y = rand() % Config::sizeY;
        } while (getCell(x, y) != TerrainType::Water);
        landAdded++;
        setCell(x, y, TerrainType::Land);
        addPossible(x, y, TerrainType::Water);

        for (int j = 0; j < landCount; )
        {
            if (possible.empty())
            {
                break;
            }

            int posID = rand() % possible.size();

            auto [x, y] = possible[posID];

            possible[posID] = possible.back();
            possible.pop_back();

            if (getCell(x, y) != TerrainType::Water)
            {
                continue;
            }
            j++;
            landAdded++;
            setCell(x, y, TerrainType::Land);
            addPossible(x, y, TerrainType::Water);
        }
    }
    std::cout << landAdded << std::endl;
}
bool World::addSand(int x, int y)
{
    static const int dirs[8][2] =
    {
        {1,0}, {-1,0}, {0,1}, {0,-1},
        {1,1}, {-1,-1}, {-1,1}, {1,-1}
    };
    if (getCell(x, y) != TerrainType::Water) return false;
    for (auto& d : dirs)
    {
        int nx = x + d[0];
        int ny = y + d[1];
        if (!isValid(nx, ny)) continue;
        if (getCell(nx, ny) == TerrainType::Land) return true;
    }
    return false;
}
void World::addSandToLand()
{
    for (int y = 0; y < Config::sizeY; y++)
    {
        for (int x = 0; x < Config::sizeX; x++)
        {
            if (addSand(x, y))
            {
                setCell(x, y, TerrainType::Desert);
            }
        }
    }
}
void World::smoothShores()
{
    static const int dirs[8][2] =
    {
        {1,0}, {-1,0}, {0,1}, {0,-1},
        {1,1}, {-1,-1}, {-1,1}, {1,-1}
    };
    bool changed = true;
    while (changed)
    {
        changed = false;
        std::vector<XY> sandToWater;
        for (uint16_t y = 0; y < Config::sizeY; y++)
        {
            for (uint16_t x = 0; x < Config::sizeX; x++)
            {
                if (getCell(x, y) == TerrainType::Desert)
                {
                    int waterNeighbors = 0;
                    for (auto& d : dirs)
                    {
                        int nx = x + d[0];
                        int ny = y + d[1];

                        if (!isValid(nx, ny))
                        {
                            waterNeighbors++;
                            continue;
                        }
                        if (getCell(nx, ny) == TerrainType::Water)
                        {
                            waterNeighbors++;
                        }
                    }
                    if (waterNeighbors >= 5)
                    {
                        sandToWater.push_back({ x, y });
                    }
                }
            }
        }
        if (!sandToWater.empty())
        {
            changed = true;
            for (auto& cell : sandToWater)
            {
                setCell(cell.x, cell.y, TerrainType::Water);
            }
        }
    }
}
void World::createStruct(TerrainType type)
{
    int totalCells = Config::sizeX * Config::sizeY;
    int landCells = (totalCells * Config::landPercent) / 100;

    int numberOfStruct;
    int totalStruct;
    int structCount;

    if (type == TerrainType::Water)
    {
        if (Config::numberOfLakes == 0) return;
        numberOfStruct = Config::numberOfLakes;
        totalStruct = (landCells * Config::waterPercentInLand) / 100;
        structCount = totalStruct / Config::numberOfLakes;
    }
    else if (type == TerrainType::Desert)
    {
        if (Config::numberOfDesserts == 0) return;
        numberOfStruct = Config::numberOfDesserts;
        totalStruct = (landCells * Config::sandPercent) / 100;
        structCount = totalStruct / Config::numberOfDesserts;
    }
    else if (type == TerrainType::Mountain)
    {
        if (Config::numberOfMountains == 0) return;
        numberOfStruct = Config::numberOfMountains;
        totalStruct = (landCells * Config::mountainPercent) / 100;
        structCount = totalStruct / Config::numberOfMountains;
    }
    else
    {
        std::cout << "No flag added as ";
        return;
    }

    for (int i = 0; i < numberOfStruct; i++)
    {
        int x;
        int y;
        possible.clear();
        do
        {
            x = rand() % Config::sizeX;
            y = rand() % Config::sizeY;
        } while (getCell(x, y) != TerrainType::Land);
        setCell(x, y, type);
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

            if (getCell(x, y) != TerrainType::Land) continue;

            setCell(x, y, type);
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


void World::markAllDirty(RendererSFML &renderer)
{
    for (uint32_t y = 0; y < Config::sizeY; y++)
    {
        for (uint32_t x = 0; x < Config::sizeX; x++)
        {
            sf::Color color;
            color = renderer.getColor(*this, x, y);
            renderer.addToDirtyCells(*this, x, y, color);
        }
    }
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

            if (worldX >= Config::sizeX || worldY >= Config::sizeY)
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