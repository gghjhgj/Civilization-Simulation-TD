#include "Civilization.h"
#include "Human.h"
#include "World.h"
#include "Streets.h"
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
}


void Civilization::makeEverybodyFoodWorker(Human &human)
{
    for(int i = 0; i < human.humans.size(); i ++)
    {
        if(human.humans[i].task != Human::Tasks::goingToBuilding)
        human.humans[i].task = human.Tasks::foodFinding;
    }
}
void Civilization::addMoreFoodWorkers(Human &human)
{
    for(int i = rand()%Config::partOfHumansChangingJobs; i < human.humans.size(); i += Config::partOfHumansChangingJobs)
    {
        if(human.humans[i].targetIndex == -1)
        human.humans[i].task = human.Tasks::foodFinding;
    }
}


void Civilization::makeEverybodyTreeWorker(Human &human)
{
    for(int i = 0; i < human.humans.size(); i ++)
    {
        if(human.humans[i].task != Human::Tasks::goingToBuilding)
        human.humans[i].task = human.Tasks::treeFinding;
    }
}
void Civilization::addMoreTreeWorkers(Human &human)
{
    for(int i = rand()%Config::partOfHumansChangingJobs; i < human.humans.size(); i += Config::partOfHumansChangingJobs)
    {
        if(human.humans[i].targetIndex == -1)
        human.humans[i].task = human.Tasks::treeFinding;
    }
}


void Civilization::makeEverybodyStoneWorker(Human &human)
{
    for(int i = 0; i < human.humans.size(); i ++)
    {
        if(human.humans[i].task != Human::Tasks::goingToBuilding)
        human.humans[i].task = human.Tasks::stoneFinding;
    }
}
void Civilization::addMoreStoneWorkers(Human &human)
{
    for(int i = rand()%Config::partOfHumansChangingJobs; i < human.humans.size(); i += Config::partOfHumansChangingJobs)
    {
        if(human.humans[i].targetIndex == -1)
        human.humans[i].task = human.Tasks::stoneFinding;
    }
}

void Civilization::addMoreBuilders(Human &human)
{
    for(int i = rand()%Config::partOfHumansChangingJobs; i < human.humans.size(); i += Config::partOfHumansChangingJobs)
    {
        if(human.humans[i].targetIndex == -1)
        {
            int id = rand()%buildings.constructions.size();
            human.humans[i].targetIndex = buildings.constructions[id].index;
            human.humans[i].targetBuildingID = id;
            human.humans[i].task = human.Tasks::building;
            buildings.constructions[id].humansIDS.push_back(i);
            human.humans[i].buildingBuildersID = buildings.constructions[id].humansIDS.size() - 1;
        }
    }
}

void Civilization::civilizationDecision(Human &human, Food &food, Stone &stone, Tree &tree)
{
    if(resources.food <= human.humansCount)
    {
        makeEverybodyFoodWorker(human);
    }
    if(buildings.constructions.size() > 0)
    {
        addMoreBuilders(human);
        if(buildings.sawmills.size() > 0 && buildings.mines.size() > 0 && buildings.farms.size() > 0)
        return;
    }
    if(resources.food <= Config::foodNeededForFarm)
    {
        addMoreFoodWorkers(human);
    }
    if(resources.wood < Config::woodNeededForFarm || resources.wood < Config::woodNeededForHouse || resources.stone < Config::stoneNeededForMine || food.foodsCount < Config::maxFood/10)
    {
        addMoreTreeWorkers(human);
    }
    if(resources.stone < Config::stoneNeededForMine || food.foodsCount < Config::maxFood/10)
    {
        addMoreStoneWorkers(human);
    }
    
}
/*
int Civilization::getBestTileForBuilding(World &world)
{
    while(world.currentMaxPopularity > 0 &&
          world.popularityRanking[world.currentMaxPopularity].empty())
    {
        world.currentMaxPopularity--;
    }

    if(world.popularityRanking[world.currentMaxPopularity].empty())
    {
        return -1;
    }

    static const int offsets[9] =
    {
        0,
        -Config::sizeX - 1,
        -Config::sizeX,
        -Config::sizeX + 1,
        -1,
        1,
        Config::sizeX - 1,
        Config::sizeX,
        Config::sizeX + 1
    };

    int bestTileIndex =
        world.popularityRanking[world.currentMaxPopularity].back();

    for(int i = 0; i < 9; i++)
    {
        int idx = bestTileIndex + offsets[i];

        if(!world.isValid(idx))
            continue;

        int pop = world.grid[idx].popularity;
        int bucketIdx = world.grid[idx].indexInBucket;

        if(bucketIdx == -1)
            continue;

        auto &bucket = world.popularityRanking[pop];

        if(bucket.empty())
            continue;

        int lastTile = bucket.back();

        bucket[bucketIdx] = lastTile;
        world.grid[lastTile].indexInBucket = bucketIdx;

        bucket.pop_back();

        world.grid[idx].popularity = 0;
        world.grid[idx].indexInBucket = -1;
        world.grid[idx].civZone = 1;
    }

    while(world.currentMaxPopularity > 0 &&
          world.popularityRanking[world.currentMaxPopularity].empty())
    {
        world.currentMaxPopularity--;
    }

    return bestTileIndex;
}
int Civilization::getOutskirtsTileForBuilding(World &world)
{
    int targetPop = world.currentMaxPopularity * Config::outskirts;

    if(targetPop < 1)
        targetPop = 1;

    int chosenPop = -1;

    for(int p = targetPop; p >= 1; p--)
    {
        if(p < world.popularityRanking.size() &&
           !world.popularityRanking[p].empty())
        {
            chosenPop = p;
            break;
        }
    }

    if(chosenPop == -1)
    {
        for(int p = targetPop + 1;
            p <= world.currentMaxPopularity;
            p++)
        {
            if(p < world.popularityRanking.size() &&
               !world.popularityRanking[p].empty())
            {
                chosenPop = p;
                break;
            }
        }
    }

    if(chosenPop == -1)
    {
        if(!world.popularityRanking[0].empty())
            chosenPop = 0;
        else
            return -1;
    }

    int tileIndex =
        world.popularityRanking[chosenPop].back();

    static const int offsets[9] =
    {
        0,
        -Config::sizeX - 1,
        -Config::sizeX,
        -Config::sizeX + 1,
        -1,
        1,
        Config::sizeX - 1,
        Config::sizeX,
        Config::sizeX + 1
    };

    for(int i = 0; i < 9; i++)
    {
        int idx = tileIndex + offsets[i];

        if(!world.isValid(idx))
            continue;

        int pop = world.grid[idx].popularity;
        int bucketIdx = world.grid[idx].indexInBucket;

        if(bucketIdx == -1)
            continue;

        auto &bucket = world.popularityRanking[pop];

        if(bucket.empty())
            continue;

        int lastTile = bucket.back();

        bucket[bucketIdx] = lastTile;
        world.grid[lastTile].indexInBucket = bucketIdx;

        bucket.pop_back();

        world.grid[idx].popularity = 0;
        world.grid[idx].indexInBucket = -1;
        world.grid[idx].civZone = 1;
    }

    while(world.currentMaxPopularity > 0 &&
          world.popularityRanking[world.currentMaxPopularity].empty())
    {
        world.currentMaxPopularity--;
    }

    return tileIndex;
}
*/

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
        int idx = ny * Config::sizeX + nx;
        if (!world.isValid(idx) || 
        world.grid[idx].civilizationPlace > 0 || 
        world.grid[idx].civZone > 0 ||
        world.grid[idx].flags & World::CellFlags::Water)
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
    (world.grid[index].buildings.Construction != 0 ||
    world.grid[index].buildings.House != 0 ||
    world.grid[index].buildings.Farm != 0 ||
    world.grid[index].buildings.Mine != 0 ||
    world.grid[index].buildings.Sawmill != 0 ||
    world.grid[index].buildings.Factory != 0 ||
    world.grid[index].civZone > 0));


    addTilesToPossibleVillage(world, index, 2);
    markCloseAsCivZone(world, index, 1);
    if(bestTilesForBuildingsVillage.empty()) return -1;
    return index;
    }
    else
        return -1;
}


void Civilization::addHouse(World &world, int idx)
{
    world.grid[idx].buildings.House = 1;
    buildings.houses.push_back({idx});
    world.addToDirtyCells(idx);
}
void Civilization::buildHouse(World &world)
{
    if(resources.stone >= Config::stoneNeededForHouse && resources.wood >= Config::woodNeededForHouse)
    {
        int index = getBestTileForBuilingsVillage(world);
        if (index == -1)
        {
            return; 
        }
        startConstruction(world, index, house);
        resources.stone -= Config::stoneNeededForHouse;
        resources.wood -= Config::woodNeededForHouse;
    }
}
void Civilization::assignHumansToHouse(Human &human)
{
    for(int i = human.humansHavingHouseCount; 
        i < human.humansCount 
        && human.humansHavingHouseCount < human.humansCount 
        && human.humansHavingHouseCount < buildings.houses.size()*Config::maxHumansInHouse; 
        i++)
        human.humansHavingHouseCount++;
}



void Civilization::addFarm(World &world, int idx)
{
    world.grid[idx].buildings.Farm = 1;
    world.addToDirtyCells(idx);

    if(!buildings.farms.empty() && (buildings.farms.back().workersAssigned == Config::maxFarmWorkersOnFarm))
    {
        emptyFarmsStartingID = buildings.farms.size();
    }

    buildings.farms.push_back({idx, 0});
}

void Civilization::buildFarm(World &world)
{
    if(resources.food >= Config::foodNeededForFarm && resources.wood >= Config::woodNeededForFarm)
    {
        
            int index = getBestTileForBuilingsVillage(world);
        if (index == -1)
        {
            return; 
        }
        startConstruction(world, index, farm);
        resources.food -= Config::foodNeededForFarm;
        resources.wood -= Config::woodNeededForFarm;
    }
}
void Civilization::farmsGains()
{
    resources.food += farmWorkers * Config::foodPerFarmWorker;
}

void Civilization::assignHumansToFarm(Human &human)
{
    int j = human.humans.size() - 1;
    for(int i = emptyFarmsStartingID; i < buildings.farms.size() && j >= 0; i++)
    for( ;buildings.farms[i].workersAssigned < Config::maxFarmWorkersOnFarm && j >= 0; j--)
    {
        if(human.humans[j].task != Human::Tasks::goingToBuilding)
        {
            if(human.humans[j].task == Human::Tasks::building)
            {
                auto &constr = buildings.constructions[human.humans[j].targetBuildingID];
                if(human.humans[j].buildingBuildersID != constr.humansIDS.size() - 1)
                {
                    human.humans[constr.humansIDS.back()].buildingBuildersID = human.humans[j].buildingBuildersID;
                    constr.humansIDS[human.humans[j].buildingBuildersID] = constr.humansIDS.back();
                }
                constr.humansIDS.pop_back();
            }
            human.humans[j].task = Human::Tasks::goingToBuilding;
            human.humans[j].targetIndex = buildings.farms[i].index;
            human.humans[j].targetBuildingID = i;
            human.humans[j].buildingBuildersID = -1;
            buildings.farms[i].workersAssigned++;
        }
    }
}





void Civilization::addSawmill(World &world, int idx)
{
    world.grid[idx].buildings.Sawmill = 1;
    world.addToDirtyCells(idx);

    if(!buildings.sawmills.empty() && (buildings.sawmills.back().workersAssigned == Config::maxSawmillWorkersInSawmill))
    {
        emptySawmillsStartingID = buildings.sawmills.size();
    }

    buildings.sawmills.push_back({idx, 0});
}
void Civilization::buildSawmill(World &world)
{
    if(resources.wood>= Config::woodNeededForSawmill && resources.stone >= Config::stoneNeededForSawmill)
    {
        int index = getBestTileForBuilingsVillage(world);
        if (index == -1)
        {
            return; 
        }
        startConstruction(world, index, sawmill);
        resources.wood -= Config::woodNeededForSawmill;
        resources.stone -= Config::stoneNeededForSawmill;
    }
}
void Civilization::sawmillsGains()
{
    resources.wood += sawmillWorkers * Config::woodPerSawmillWorker;
}

void Civilization::assignHumansToSawmills(Human &human)
{
    int j = human.humans.size() - 1;
    for(int i = emptySawmillsStartingID; i < buildings.sawmills.size() && j >= 0; i++)
    for( ;buildings.sawmills[i].workersAssigned < Config::maxSawmillWorkersInSawmill && j >= 0; j--)
    {
        if(human.humans[j].task != Human::Tasks::goingToBuilding)
        {
             if(human.humans[j].task == Human::Tasks::building)
            {
                auto &constr = buildings.constructions[human.humans[j].targetBuildingID];
                if(human.humans[j].buildingBuildersID != constr.humansIDS.size() - 1)
                {
                    human.humans[constr.humansIDS.back()].buildingBuildersID = human.humans[j].buildingBuildersID;
                    constr.humansIDS[human.humans[j].buildingBuildersID] = constr.humansIDS.back();
                }
                constr.humansIDS.pop_back();
            }
            human.humans[j].task = Human::Tasks::goingToBuilding;
            human.humans[j].targetIndex = buildings.sawmills[i].index;
            human.humans[j].targetBuildingID = i;
            buildings.sawmills[i].workersAssigned++;
        }
    }
}






void Civilization::addMine(World &world, int idx)
{
    world.grid[idx].buildings.Mine = 1;
    world.addToDirtyCells(idx);
    if(!buildings.mines.empty() && (buildings.mines.back().workersAssigned == Config::maxMineWorkersInMine))
    {
        emptyMinesStartingID = buildings.mines.size();
    }
    buildings.mines.push_back({idx, 0});
}
/*
int Civilization::bestNextMinePlace(World &world)
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(0, world.lands.size() - 1);
    int bestIndex = 0;
    int tries = 0;
    int maxTries = Config::maxMineSpawnTries;
    do
    {
        bestIndex = world.lands[dist(rng)]; 
        tries++;
    } while (world.grid[bestIndex].flags != World::CellFlags::Mountain && tries < maxTries);
    if(tries >= maxTries) 
    {
        return getOutskirtsTileForBuilding(world);
    }
    int pop = world.grid[bestIndex].popularity;
    int bucketIdx = world.grid[bestIndex].indexInBucket;
    if (bucketIdx != -1 && pop >= 0 && pop < world.popularityRanking.size())
    {
        auto &bucket = world.popularityRanking[pop];
        
        if (!bucket.empty() && bucketIdx < bucket.size())
        {
            int lastTileIndex = bucket.back();
            bucket[bucketIdx] = lastTileIndex;
            world.grid[lastTileIndex].indexInBucket = bucketIdx;
            bucket.pop_back();
        }
    }
    world.grid[bestIndex].popularity = 0;
    world.grid[bestIndex].indexInBucket = -1;

    return bestIndex;
}
*/
void Civilization::buildMine(World &world)
{
    if(resources.wood >= Config::woodNeededForMine && resources.stone >= Config::stoneNeededForMine)
    {
        int index = getBestTileForBuilingsVillage(world);
        if (index == -1)
        {
            return; 
        }
        startConstruction(world, index, mine);
        resources.stone -= Config::stoneNeededForMine;
        resources.wood -= Config::woodNeededForMine;
    }
}
void Civilization::minesGains()
{
    resources.stone +=  mineWorkers * Config::stonePerMineWorker;
}

void Civilization::assignHumansToMine(Human &human)
{
    int j = human.humans.size() - 1;
    for(int i = emptyMinesStartingID; i < buildings.mines.size() && j >= 0; i++)
    for( ;buildings.mines[i].workersAssigned < Config::maxMineWorkersInMine && j >= 0; j--)
    {
        if(human.humans[j].task != Human::Tasks::goingToBuilding)
        {
             if(human.humans[j].task == Human::Tasks::building)
            {
                auto &constr = buildings.constructions[human.humans[j].targetBuildingID];
                if(human.humans[j].buildingBuildersID != constr.humansIDS.size() - 1)
                {
                    human.humans[constr.humansIDS.back()].buildingBuildersID = human.humans[j].buildingBuildersID;
                    constr.humansIDS[human.humans[j].buildingBuildersID] = constr.humansIDS.back();
                }
                constr.humansIDS.pop_back();
            }
            human.humans[j].task = Human::Tasks::goingToBuilding;
            human.humans[j].targetIndex = buildings.mines[i].index;
            human.humans[j].targetBuildingID = i;
            buildings.mines[i].workersAssigned++;
        }
    }
}
/*
void Civilization::addFactory(World &world, Streets &streets, Human &human, int idx)
{
    world.grid[idx].buildings.Factory = 1;
    world.addToDirtyCells(idx);

    if(!buildings.factories.empty() && (buildings.factories.back().workersAssigned == Config::maxFactoryWorkersInFactory))
    {
        emptyFactoriesStartingID = buildings.factories.size();
    }

    buildings.factories.push_back({idx, 0});
    if(buildings.factories.size() > 1)
        {
            streets.addStreet(world, *this, human, buildings.factories[buildings.factories.size() - 2].index, idx);
        }
}
void Civilization::buildFactory(World &world)
{
    if(resources.wood >= Config::woodNeededForFactory &&
       resources.stone >= Config::stoneNeededForFactory)
    {
        int baseIndex;

        if(lastFactoryIndex != -1)
        {
            baseIndex = lastFactoryIndex;
        }
        else
        {
            baseIndex = getOutskirtsTileForBuilding(world);
        }

        if(baseIndex == -1)
            return;

        int x = baseIndex % Config::sizeX;
        int y = baseIndex / Config::sizeX;

        int newIndex = -1;
        int maxTries = Config::factoryMaxSpawnTries;

        do
        {
            int dx = (rand() % 2 ? 1 : -1) *
         (Config::factoriesMinDistance +
          rand() % (Config::factoriesMaxDistance - Config::factoriesMinDistance + 1));

        int dy = (rand() % 2 ? 1 : -1) *
         (Config::factoriesMinDistance +
          rand() % (Config::factoriesMaxDistance - Config::factoriesMinDistance + 1));

            int nx = x + dx;
            int ny = y + dy;

            newIndex = world.index(nx, ny);

            if(--maxTries <= 0)
                return;

        } while(!world.isValid(newIndex) ||
                world.grid[newIndex].civZone > 0 ||
                world.grid[newIndex].flags & World::Water);

        startConstruction(world, newIndex, factory);
        lastFactoryIndex = newIndex;
        resources.wood -= Config::woodNeededForFactory;
        resources.stone -= Config::stoneNeededForFactory;
    }
}

void Civilization::factoriesGains()
{
    resources.wood += sawmillWorkers * Config::woodPerSawmillWorker;
}

void Civilization::assignHumansToFactory(Human &human)
{
    int j = human.humans.size() - 1;
    for(int i = emptyFactoriesStartingID; i < buildings.factories.size() && j >= 0; i++)
    for( ;buildings.factories[i].workersAssigned < Config::maxFactoryWorkersInFactory && j >= 0; j--)
    {
        if(human.humans[j].task != Human::Tasks::goingToBuilding)
        {
             if(human.humans[j].task == Human::Tasks::building)
            {
                auto &constr = buildings.constructions[human.humans[j].targetBuildingID];
                if(human.humans[j].buildingBuildersID != constr.humansIDS.size() - 1)
                {
                    human.humans[constr.humansIDS.back()].buildingBuildersID = human.humans[j].buildingBuildersID;
                    constr.humansIDS[human.humans[j].buildingBuildersID] = constr.humansIDS.back();
                }
                constr.humansIDS.pop_back();
            }
            human.humans[j].task = Human::Tasks::goingToBuilding;
            human.humans[j].targetIndex = buildings.factories[i].index;
            human.humans[j].targetBuildingID = i;
            buildings.sawmills[i].workersAssigned++;
        }
    }
}
*/



void Civilization::buildingDecision(World &world, Human &human, Food &food, Stone &stone, Tree &tree)
{
    if((buildings.farms.size() + farmsDuringConstruction) * 50 < human.humansCount )
    {
        buildFarm(world);
    }
    else if((buildings.sawmills.size() + sawmillsDuringConstruction) * 260 < human.humansCount)
    {
        buildSawmill(world);
    }
    else if((buildings.mines.size() + minesDuringConstruction) * 2600 < human.humansCount)
    {
        buildMine(world);
    }
    else if(buildings.houses.size() + housesDuringConstruction < human.humansCount/5)
    {
        buildHouse(world);
    }
    /*
    else if(buildings.factories.size() + factoriesDuringConstruction < human.humansCount)
    {
        buildFactory(world);
    }
    */
}

void Civilization::startConstruction(World &world, int idx, BuildingsType type)
{
    world.grid[idx].buildings.Construction = 1;
    world.addToDirtyCells(idx);
    int HP = 0;
    if(type == house)
    {
        HP = Config::hitsForBuildingHouse;
        housesDuringConstruction++;
    }
    else if(type == farm)
    {
        HP = Config::hitsForBuildingFarm;
        farmsDuringConstruction++;
    }
    else if(type == sawmill)
    {
        HP = Config::hitsForBuildingSawmill;
        sawmillsDuringConstruction++;
    }
    else if(type == mine)
    {
        HP = Config::hitsForBuildingMine;
        minesDuringConstruction++;
    }
    else if(type == factory)
    {
        HP = Config::hitsForBuildingFactory;
        factoriesDuringConstruction++;
    }
     else if(type == woodWall)
    {
        HP = Config::hitsForBuildingWoodWall;
    }
    else if(type == stoneWall)
    {
        HP = Config::hitsForBuildingStoneWall;
    }
    buildings.constructions.push_back({idx, type, HP});
}

void Civilization::endConstruction(World &world, Human &human, Streets &streets, int idx, BuildingsType type, int id)
{
    for(size_t i = 0; i < buildings.constructions[id].humansIDS.size(); i++)
    {
        int workerID = buildings.constructions[id].humansIDS[i];
        if (workerID >= 0 && workerID < human.humans.size()) 
        {
            human.humans[workerID].targetBuildingID = -1;
            human.humans[workerID].targetIndex = -1;
            human.humans[workerID].task = Human::Tasks::foodFinding; 
        }
    }
    world.grid[idx].buildings.Construction = 0;
    
    if (type == house) 
    {
        addHouse(world, idx);
        housesDuringConstruction--;
    }
    else if (type == farm)
    {
        addFarm(world, idx);
        farmsDuringConstruction--;
    }
    else if (type == sawmill)
    {
        addSawmill(world, idx);
        sawmillsDuringConstruction--;
    }
    else if (type == mine) 
    {
        addMine(world, idx);
        minesDuringConstruction--;
    }
     else if (type == woodWall)
    {
        if(Walls::instance != nullptr)
        {
            Walls::instance->adddWallTile(world, idx, Walls::WallsTypes::woodenWall);
        }
    }
    else if (type == stoneWall)
    {
        if(Walls::instance != nullptr)
        {
            Walls::instance->adddWallTile(world, idx, Walls::WallsTypes::stoneWall);
        }
    }
    /*
    else if(type == factory)
    {
        addFactory(world, streets, human, idx);
        factoriesDuringConstruction--;
    }
    */

    int lastIdx = buildings.constructions.size() - 1;
    if (id != lastIdx)
    {
        for(size_t i = 0; i < buildings.constructions.back().humansIDS.size(); i++)
        {
            int workerID = buildings.constructions.back().humansIDS[i];
            if (workerID >= 0 && workerID < human.humans.size()) 
            {
                human.humans[workerID].targetBuildingID = id;
            }
        }
        buildings.constructions[id] = buildings.constructions.back();
    }
    buildings.constructions.pop_back();
}