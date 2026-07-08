#include "Civilization.h"
#include "HumansData/Human.h"
#include "WorldData/World.h"
#include "Walls.h"
#include "Army.h"
void Civilization::addCivilization(World &world, int index)
{
    world.grid[index].civilizationPlace += Config::civilizationPlaceHP;
    world.addToDirtyCells(index);
}

void Civilization::createCivilization(World &world)
{
    /*
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(0, world.lands.size() - 1);

        int id;
        do
        {
            id = world.lands[dist(rng)]; 
        } while (
        !world.isEmpty(id) ||
        !world.isValid(id + Config::civilizationPlaceX) ||
        !world.isValid(id + Config::civilizationPlaceY * Config::sizeX)
        );
        spawn = id;
        */
       //
       int id;
       id = Config::civSpawnPoint;
       spawn = id;
       //
        addTilesToPossibleVillage(world, spawn, Config::civilizationPlaceX);
        for(int i = 0; i < Config::civilizationPlaceX; i++)
        {
            markCloseAsCivZone(world, spawn, i);
        }
        for(int i = 0; i < Config::civilizationPlaceY; i++)
        {
            for(int j = 0; j < Config::civilizationPlaceX; j++)
            {
                addCivilization(world, id + j + Config::sizeX * i);
            }
        }
        initBuildings();
}

void Civilization::initBuildings()
{
    auto &cost = buildingsCost;
    auto &gains = buildingsGains;

    hitsForBuilding[house] = Config::hitsForBuildingHouse;
    cost[house].food = 0;
    cost[house].stone = Config::stoneNeededForHouse;
    cost[house].wood = Config::woodNeededForHouse;
    maxHumans[house] = Config::maxHumansInHouse;

    hitsForBuilding[farm] = Config::hitsForBuildingFarm;
    cost[farm].food = Config::foodNeededForFarm;
    cost[farm].stone = 0;
    cost[farm].wood = Config::woodNeededForFarm;
    maxHumans[farm] = Config::maxFarmWorkersOnFarm;
    gains[farm].food = Config::foodPerFarmWorker;

    hitsForBuilding[sawmill] = Config::hitsForBuildingSawmill;
    cost[sawmill].food = 0;
    cost[sawmill].stone = Config::stoneNeededForSawmill;
    cost[sawmill].wood = Config::woodNeededForSawmill;
    maxHumans[sawmill] = Config::maxSawmillWorkersInSawmill;
    gains[sawmill].wood = Config::woodPerSawmillWorker;

    hitsForBuilding[mine] = Config::hitsForBuildingMine;
    cost[mine].food = 0;
    cost[mine].stone = Config::stoneNeededForMine;
    cost[mine].wood = Config::woodNeededForMine;
    maxHumans[mine] = Config::maxMineWorkersInMine;
    gains[mine].stone = Config::stonePerMineWorker;

    hitsForBuilding[woodWall] = Config::hitsForBuildingWoodWall;
    cost[woodWall].food = 0;
    cost[woodWall].stone = 0;
    cost[woodWall].wood = Config::woodNeededForWoodWall;

    hitsForBuilding[stoneWall] = Config::hitsForBuildingStoneWall;
    cost[stoneWall].food = 0;
    cost[stoneWall].stone = Config::stoneNeededForStoneWall;
    cost[stoneWall].wood = 0;
}

void Civilization::addWorkers(Human &human, Tasks task)
{
    for(int i = rand()%Config::partOfHumansChangingJobs; i < human.humans.size(); i += Config::partOfHumansChangingJobs)
    {
        if(human.humans[i].targetIndex == -1)
        human.humans[i].task = task;
    }
}


void Civilization::civilizationDecision(Human &human, Food &food, Stone &stone, Tree &tree)
{
    bool areConstructions = false;
    for(int i = 0; i < BuildingsType::BUILDINGS_COUNT; i++)
    {
        if(constructions[i] > 0)
        {
            areConstructions = true;
            continue;
        }
    }
    if(areConstructions)
    {
        addWorkers(human, building);
    }
    if(resources.food <= Config::foodNeededForFarm)
    {
        addWorkers(human, foodFinding);
    }
    if(resources.wood < Config::woodNeededForFarm || resources.wood < Config::woodNeededForHouse || resources.stone < Config::stoneNeededForMine || food.foodsCount < Config::maxFood/10)
    {
        addWorkers(human, treeFinding);
    }
    if(resources.stone < Config::stoneNeededForMine || food.foodsCount < Config::maxFood/10)
    {
        addWorkers(human, stoneFinding);
    }
    
}

void Civilization::markCloseAsCivZone(World &world, int index, int r)
{
    int x = index % Config::sizeX;
    int y = index / Config::sizeX;

    for (int dy = -r; dy <= r; dy++)
    {
        for (int dx = -r; dx <= r; dx++)
        {
            if (!world.isValid(x + dx, y + dy))
                continue;

            int nx = x + dx;
            int ny = y + dy;
            int idx = ny * Config::sizeX + nx;
            if(ny < mostNorthCivZone || mostNorthCivZone == -1)
            {
                mostNorthCivZone = ny;
            }
            if(ny > mostSouthCivZone)
            {
                mostSouthCivZone = ny;
            }
            if(nx < mostWestCivZone || mostWestCivZone == -1)
            {
                mostWestCivZone = nx;
            }
            if(nx > mostEastCivZone)
            {
                mostEastCivZone = nx;
            }
            world.grid[idx].civZone = 1;
        }
    }
}

void Civilization::addTilesToPossibleVillage(World &world, int index, int r)
{
    int x = index % Config::sizeX;
    int y = index / Config::sizeX;
    
    for (int dy = -r; dy <= r; dy++)
    {
        for (int dx = -r; dx <= r; dx++)
        {
            int nx = x + dx;
            int ny = y + dy;
            
            if (nx < 0 || nx >= Config::sizeX || ny < 0 || ny >= Config::sizeY)
                continue;

            int idx = ny * Config::sizeX + nx;

            if (world.grid[idx].civilizationPlace > 0 || 
                world.grid[idx].civZone > 0 ||
                world.grid[idx].flags & Water)
                continue;

            bestTilesForBuildingsVillage.push_back(idx);
        }
    }
}

int Civilization::getBestTileForBuilingsVillage(World &world)
{
    int id;
    int index;
    if (!bestTilesForBuildingsVillage.empty())
    {
    do
    {
        std::uniform_int_distribution<int> dist(0, bestTilesForBuildingsVillage.size() - 1);
        id = dist(rng);
        index = bestTilesForBuildingsVillage[id];
        std::swap(bestTilesForBuildingsVillage[id], bestTilesForBuildingsVillage.back());
        bestTilesForBuildingsVillage.pop_back();
    } while (!bestTilesForBuildingsVillage.empty() && 
    (world.grid[index].building != BUILDINGS_COUNT ||
    world.grid[index].civZone > 0 ||
    world.grid[index].construction.hitsNeeded > 0));


    addTilesToPossibleVillage(world, index, 2);
    markCloseAsCivZone(world, index, 1);
    if(bestTilesForBuildingsVillage.empty()) return -1;
    return index;
    }
    else
        return -1;
}

void Civilization::addBuilding(World &world, int idx, BuildingsType type)
{
    world.grid[idx].building = type;
    buildingsIndexes[type].push_back(idx);
    world.addToDirtyCells(idx);
}

void Civilization::buildBuilding(World &world, BuildingsType type)
{
    if(resources.food >= buildingsCost[type].food
    && resources.stone >= buildingsCost[type].stone
    && resources.wood >= buildingsCost[type].wood)
    {
        int index = getBestTileForBuilingsVillage(world);
        if(index == -1) return;
        startConstruction(world, index, type);
        
        resources.food -= buildingsCost[type].food;
        resources.wood -= buildingsCost[type].wood;
        resources.stone -= buildingsCost[type].stone;
    }
}


void Civilization::assignHumansToBuilding(Human &human, BuildingsType type)
{
    if(type == house)
    {
    int humansWithoutHouse = human.humansCount - human.humansHavingHouseCount; 
    int emptyPlaces = (buildingsIndexes[house].size() * Config::maxHumansInHouse) - human.humansHavingHouseCount;

    human.humansHavingHouseCount += std::min(humansWithoutHouse, emptyPlaces);
    return;
    }


        if(buildingsIndexes[type].empty()) return;
        for(Human::HumanData& h : human.humans)
        {
            if(h.task == goingToBuilding) continue;
            if(buildingsIndexes[type].size() * maxHumans[type] <= workersAssigned[type]) break;
            
            int id = rand()%buildingsIndexes[type].size();
            h.task = goingToBuilding;
            h.targetIndex = buildingsIndexes[type][id];
            workersAssigned[type]++;
    }
}

void Civilization::getBuildingsGains()
{
    for(int i = 0; i < BuildingsType::BUILDINGS_COUNT; i++)
    {
        resources.food += buildingsGains[i].food * realWorkers[i];
        resources.wood += buildingsGains[i].wood * realWorkers[i];
        resources.stone += buildingsGains[i].stone * realWorkers[i];
    }
}




void Civilization::buildingDecision(World &world, Human &human, Food &food, Stone &stone, Tree &tree)
{
    if((buildingsIndexes[farm].size() + constructions[farm]) * 50 < human.humansCount )
    {
        buildBuilding(world, farm);
    }
    else if((buildingsIndexes[sawmill].size() + constructions[sawmill]) * 260 < human.humansCount)
    {
        buildBuilding(world, sawmill);
    }
    else if((buildingsIndexes[mine].size() + constructions[mine]) * 2600 < human.humansCount)
    {
        buildBuilding(world, mine);
    }
    else if(buildingsIndexes[house].size() + constructions[house] < human.humansCount/5)
    {
        buildBuilding(world, house);
    }
}

void Civilization::startConstruction(World &world, int idx, BuildingsType type)
{
    world.grid[idx].construction.hitsNeeded = hitsForBuilding[type];
    world.grid[idx].construction.result = type;
    world.addToDirtyCells(idx);

    constructions[type]++;
}

void Civilization::endConstruction(World &world, Human &human, int idx, BuildingsType type)
{
    world.grid[idx].construction.result = BUILDINGS_COUNT;
    world.addToDirtyCells(idx);
    addBuilding(world, idx, type);
    constructions[type]--;
}