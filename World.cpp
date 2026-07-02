#include "World.h"
#include "Human.h"
#include "Civilization.h"
#include "Food.h"
#include "Stone.h"
#include "Tree.h"
#include "Streets.h"
#include "Army.h"
#include "Monsters.h"
World::Cell World::grid[Config::sizeX * Config::sizeY];
#include <fstream>



void World::writeStatsToTxt(int ticks, int FPS, Civilization &civilization, Human &human, Stone &stone, Food &food, Tree &tree, Army &army, Monsters &monsters)
{
    std::ofstream statsFile("stats.txt");
    
    if (statsFile.is_open())
    {
        statsFile << "=== STATYSTYKI SYMULACJI ===\n";
        statsFile << "Ticks per second " << ticks << "\n";
        statsFile << "FPS: " << FPS << "\n";
        statsFile << "Total Ticks: " << allTicksCount << "\n\n";

        statsFile << "Ilosc jedzenia: " << food.foodsCount<< "\n";
        statsFile << "Ilosc drzew: " << tree.treesCount<< "\n";
        statsFile << "Ilosc kamieni: " << stone.stonesCount << "\n \n";

        statsFile << "ilosc ludzi: " << human.humansCount << "\n";
        statsFile << "ilosc ludzi na mapie: " << human.humans.size() << "\n";
        statsFile << "ilosc ludzi posiadajacych dom: " << human.humansHavingHouseCount << "\n";
        statsFile << "ilosc ludzi w strukturach: " <<"\n";
        statsFile << "ilosc ludzi na farmach: " << civilization.farmWorkers << "\n";
        statsFile << "ilosc ludzi w tartakach: " << civilization.sawmillWorkers << "\n";
        statsFile << "ilosc ludzi w kopalniach " << civilization.mineWorkers << "\n";
        statsFile << "ilosc ludzi w fabrykach: " << civilization.factoriesWorkers << "\n\n";

        statsFile << "zasoby:\n";
        statsFile << "jedzenie: " << civilization.resources.food << "\n";
        statsFile << "drewno: " << civilization.resources.wood << "\n";
        statsFile << "kamienie: " << civilization.resources.stone << "\n\n";

        statsFile << "budynki:\n";
        statsFile << "budowy: " << civilization.buildings.constructions.size() << "\n poszczególne typy budów:  \n";
        statsFile << "domy w budowie: " << civilization.housesDuringConstruction << "\n";
        statsFile << "farmy w budowie: " << civilization.farmsDuringConstruction << "\n";
        statsFile << "tartaki w budowie: " << civilization.sawmillsDuringConstruction << "\n";
        statsFile << "kopalnie w budowie: " << civilization.minesDuringConstruction << "\n";
        statsFile << "fabryki w budowie: " << civilization.factoriesDuringConstruction << "\n";
        statsFile << "domy " << civilization.buildings.houses.size() << "\n";
        statsFile << "farmy " << civilization.buildings.farms.size() << "\n";
        statsFile << "tartaki " << civilization.buildings.sawmills.size() << "\n";
        statsFile << "kopalnie " << civilization.buildings.mines.size() << "\n";
        statsFile << "fabryki " << civilization.buildings.factories.size() << "\n\n";

        statsFile << "armia:\n";
        statsFile << "ilosc ludzi w armii: " << army.armyRegistry[Army::ArmyProfession::soldier].logicID.size() + 
        army.armyRegistry[Army::ArmyProfession::archer].logicID.size() << "\n";
        statsFile << "ilosc zolnierzy: " << army.armyRegistry[Army::ArmyProfession::soldier].logicID.size() << "\n";
        statsFile << "ilosc lucznikow: " <<  army.armyRegistry[Army::ArmyProfession::archer].logicID.size() << "\n\n";

        statsFile << "potwory:\n";
        statsFile << "ilosc potworow lacznie: " << monsters.monstersRegistry[Monsters::MonstersTypes::normalMonster].logicID.size() + 
        monsters.monstersRegistry[Monsters::MonstersTypes::giantMonster].logicID.size() << "\n";
        statsFile << "ilosc zwyklych potworow: " << monsters.monstersRegistry[Monsters::MonstersTypes::normalMonster].logicID.size() << "\n";
        statsFile << "ilosc gigantow: " << monsters.monstersRegistry[Monsters::MonstersTypes::giantMonster].logicID.size() << "\n";


        
        statsFile.close();
    }
}
void World::init()
{
    popularityRanking.clear();
    popularityRanking.resize(1);
    for (int y = 0; y < Config::sizeY; y++)
    {
        for (int x = 0; x < Config::sizeX; x++)
        {
            Cell &c = grid[index(x, y)];
            c.food = 0;
            c.treeHP = 0;
            c.stoneHP = 0;
            c.flags = 0;
            c.popularity = 0;
            c.indexInBucket = 0;
            c.humanIndex = -1; 
            c.civZone = 0;
            c.buildings = {0, 0, 0, 0, 0, 0};
            c.streets = {0};
            c.walls = {0, 0};
            c.indexInBucket = popularityRanking[0].size();

            popularityRanking[0].push_back(index(x, y));
        }
    }
}
bool World::isValid(int x, int y)
{
    return x >= 0 && y >= 0 &&
           x < Config::sizeX &&
           y < Config::sizeY;
}
bool World::isValid(int id)
{
    return id >= 0 && id < Config::sizeX * Config::sizeY;
}
void World::addPossible(int x, int y, CellFlags flag)
{
    static const int dirs[8][2] =
    {
        {1,0}, {-1,0}, {0,1}, {0,-1},
        {1,1}, {-1,-1}, {-1,1}, {1,-1}
    };

    for (auto &d : dirs)
    {
        int nx = x + d[0];
        int ny = y + d[1];

        if (!isValid(nx, ny))
            continue;

        int id = index(nx, ny);

        if ((grid[id].flags & flag) && !visited[id])
        {
            visited[id] = true;
            possible.push_back({nx, ny});
        }
    }
}
void World::createOcean()
{
    for(int y = 0; y < Config::sizeY; y++)
    {
        for(int x = 0; x < Config::sizeX; x++)
        {
            grid[index(x,y)].flags |= CellFlags::Water;
        }
    }
}
void World::createLand()
{
    int totalCells = Config::sizeX * Config::sizeY;
    int totalLand = (totalCells * Config::landPercent) / 100;
    int landCount = totalLand / Config::numberOfLands;


    for(int i = 0; i < Config::numberOfLands; i++)
    {
        int x;
        int y;
        possible.clear();
        std::fill(std::begin(visited), std::end(visited), false);
        do
        {
            x = rand()%Config::sizeX;
            y = rand()%Config::sizeY;
        } while (!(grid[index(x, y)].flags & CellFlags::Water));
        grid[index(x, y)].flags &= ~CellFlags::Water; 
        grid[index(x, y)].flags |= CellFlags::Land;  
        addPossible(x, y, CellFlags::Water);

        for (int i = 0; i < landCount; i++)
{
    if (possible.empty())
    {
        break;
    }

    int posID = rand() % possible.size();

    auto [x, y] = possible[posID];

    grid[index(x, y)].flags &= ~CellFlags::Water;
    grid[index(x, y)].flags |= CellFlags::Land; 

    addPossible(x, y, CellFlags::Water);

    possible[posID] = possible.back();
    possible.pop_back();
        }
    }
}
bool World::addSand(int x, int y)
{
    bool hasLandNeighbor = false;

static const int dirs[8][2] =
{
    {1,0}, {-1,0}, {0,1}, {0,-1},
    {1,1}, {-1,-1}, {-1,1}, {1,-1}
};

int id = index(x, y);

if (grid[id].flags & CellFlags::Water)
{
    for (auto &d : dirs)
    {
        int nx = x + d[0];
        int ny = y + d[1];

        if (!isValid(nx, ny))
            continue;

        int nid = index(nx, ny);

        if (grid[nid].flags & CellFlags::Land)
        {
            hasLandNeighbor = true;
            break;
        }
    }
}
    return hasLandNeighbor;
}
void World::addSandToLand()
{
    for(int y = 0; y < Config::sizeY; y++)
    {
        for(int x = 0; x < Config::sizeX; x++)
        {
            if(addSand(x, y))
            {
                grid[index(x, y)].flags &= ~CellFlags::Water; 
                grid[index(x, y)].flags |= CellFlags::Sand;  
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
        std::vector<int> sandToWater;
        for (int y = 0; y < Config::sizeY; y++)
        {
            for (int x = 0; x < Config::sizeX; x++)
            {
                int id = index(x, y);
                if (grid[id].flags & CellFlags::Sand)
                {
                    int waterNeighbors = 0;
                    for (auto &d : dirs)
                    {
                        int nx = x + d[0];
                        int ny = y + d[1];

                        if (!isValid(nx, ny))
                        {
                            waterNeighbors++;
                            continue;
                        }
                        int nid = index(nx, ny);
                        if (grid[nid].flags & CellFlags::Water)
                        {
                            waterNeighbors++;
                        }
                    }
                    if (waterNeighbors >= 5)
                    {
                        sandToWater.push_back(id);
                    }
                }
            }
        }
        if (!sandToWater.empty())
        {
            changed = true;
            for (int id : sandToWater)
            {
                grid[id].flags &= ~CellFlags::Sand;
                grid[id].flags |= CellFlags::Water;
            }
        }
    }
}
void World::createStruct(CellFlags flag)
{
    int totalCells = Config::sizeX * Config::sizeY;
    int landCells = (totalCells * Config::landPercent) / 100;

    int numberOfStruct;
    int totalStruct;
    int structCount;

    if (flag == CellFlags::Water)
{
    if(Config::numberOfLakes == 0) return;
    numberOfStruct = Config::numberOfLakes;
    totalStruct = (landCells * Config::waterPercentInLand) / 100;
    structCount = totalStruct / Config::numberOfLakes;
}
else if (flag == CellFlags::Sand)
{
    if(Config::numberOfDesserts == 0) return;
    numberOfStruct = Config::numberOfDesserts;
    totalStruct = (landCells * Config::sandPercent) / 100;
    structCount = totalStruct / Config::numberOfDesserts;
}
else if (flag == CellFlags::Mountain)
{
    if(Config::numberOfMountains == 0) return;
    numberOfStruct = Config::numberOfMountains;
    totalStruct = (landCells * Config::mountainPercent) / 100;
    structCount = totalStruct / Config::numberOfMountains;
}
else
{
    std::cout << "No flag added as " << flag;
    return;
}

     for(int i = 0; i < numberOfStruct; i++)
    {
        int x;
        int y;
        possible.clear();
        std::fill(std::begin(visited), std::end(visited), false);
        do
        {
            x = rand()%Config::sizeX;
            y = rand()%Config::sizeY;
        } while (!(grid[index(x, y)].flags & CellFlags::Land));
        grid[index(x, y)].flags &= ~CellFlags::Land;  
        grid[index(x, y)].flags |= flag;  
        addPossible(x, y, CellFlags::Land);

        for (int j = 0; j < structCount; j++)
{
    if (possible.empty())
    {
        break;
    }

    int posID = rand() % possible.size();

    auto [x, y] = possible[posID];

    grid[index(x, y)].flags &= ~CellFlags::Land;  
    grid[index(x, y)].flags |= flag;   

    addPossible(x, y, CellFlags::Land);

    possible[posID] = possible.back();
    possible.pop_back();
        }
    }
}
void World::surfaceVectorsInit()
{
    lands.clear();
waters.clear();
sands.clear();
mountains.clear();
    for(int y = 0; y < Config::sizeY; y++)
    {
        for(int x = 0; x < Config::sizeX; x++)
        {
            auto flag = grid[index(x, y)].flags;
            if(flag == CellFlags::Land)
            {
                lands.push_back(index(x, y));
                //std::cout << "land added x: " << x << "y: " << y << "index: " << index(x, y) << std::endl;
            }
            else if(flag == CellFlags::Water)
            {
                waters.push_back(index(x, y));
                //std::cout << "water added x: " << x << "y: " << y << "index: " << index(x, y) << std::endl;
            }
            else if(flag == CellFlags::Sand)
            {
                sands.push_back(index(x, y));
                //std::cout << "sand added x: " << x << "y: " << y << "index: " << index(x, y) << std::endl;
            }
            else if(flag == CellFlags::Mountain)
            {
                mountains.push_back(index(x, y));
                //std::cout << "mountain added x: " << x << "y: " << y << "index: " << index(x, y) << std::endl;
            }
        }
    }
    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(lands.begin(), lands.end(), rng);
    std::shuffle(waters.begin(), waters.end(), rng);
    std::shuffle(sands.begin(), sands.end(), rng);
    std::shuffle(mountains.begin(), mountains.end(), rng);
}

bool World::isEmpty(int id)
{
    if (grid[id].food > 0) return false;
    if (grid[id].treeHP > 0) return false;
    if (grid[id].stoneHP > 0) return false;
    return true;
}

bool World::isEmpty(int x, int y)
{
    return isEmpty(index(x, y));
}


void World::markAllDirty()
{
    for(int y = 0; y < Config::sizeY; y++)
    {
        for(int x = 0; x < Config::sizeX; x++)
        {
            addToDirtyCells(index(x, y));
        }
    }
}

void World::updateTilePopularity(int idx)
{
    Cell &c = grid[idx];
    
    int x = idx % Config::sizeX;
    int y = idx / Config::sizeX;

    bool isEdge = (x == 0) | (x == Config::sizeX - 1) | (y == 0) | (y == Config::sizeY - 1);

    if (c.civZone > 0 ||
        c.buildings.House > 0 || 
        c.buildings.Mine > 0 || 
        c.buildings.Farm > 0|| c.buildings.Sawmill > 0|| 
        c.buildings.Construction > 0|| 
        c.civilizationPlace > 0 || 
        c.buildings.Factory > 0 ||
        c.streets.Street > 0 ||
        isEdge) 
        return;

    if (c.indexInBucket == -1)
    {
        if (popularityRanking.empty()) 
        {
            popularityRanking.resize(1);
        }
        c.popularity = 0;
        popularityRanking[0].push_back(idx);
        c.indexInBucket = popularityRanking[0].size() - 1;
    }

    int oldPopularity = c.popularity;
    int newPopularity = oldPopularity + 1;
    c.popularity = newPopularity;

    int pos = c.indexInBucket;
    int lastTileIdx = popularityRanking[oldPopularity].back();

    popularityRanking[oldPopularity][pos] = lastTileIdx;
    grid[lastTileIdx].indexInBucket = pos;

    popularityRanking[oldPopularity].pop_back();

    if(newPopularity >= popularityRanking.size())
    {
        popularityRanking.resize(newPopularity + 1);
    }
    popularityRanking[newPopularity].push_back(idx);
    c.indexInBucket = popularityRanking[newPopularity].size() - 1;

    if(newPopularity > currentMaxPopularity)
    {
        currentMaxPopularity = newPopularity;
    }
}

void World::makeAllHumansDirty(Human &human)
{
    for(int i = 0; i < human.humans.size(); i++)
    {
        addToDirtyCells(human.humans[i].index);
    }
}

bool World::checkIfThereAreNoBuildings(int idx)
{
    if(grid[idx].buildings.Construction > 0 ||
    grid[idx].buildings.House > 0 || 
    grid[idx].buildings.Factory > 0 || 
    grid[idx].buildings.Farm > 0 || 
    grid[idx].buildings.Mine > 0 ||
    grid[idx].buildings.Sawmill > 0)
    return false;
    return true;
}

void World::addToDirtyCells(int index)
{
    /*
    if(index != -1)
    {
        dirtyCells.push_back(index);
    }
    else
    {
        std::cout << "got -1 id " << std::flush;
        __builtin_trap();
        abort();
    }
        */
    dirtyCells.push_back(index);
}