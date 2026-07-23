#include "Civilization.h"
#include "HumansData/Human.h"
#include "WorldData/World.h"
#include "Walls.h"
void Civilization::addCivilization(World& world, int index)
{
    /*
    world.grid[index].civilizationPlace += Config::civilizationPlaceHP;
    world.addToDirtyCells(index);
    */
}

void Civilization::createCivilization(World& world)
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
    spawn.chunkX = Config::civSpawnChunkX;
    spawn.chunkY = Config::civSpawnChunkY;
    //
    addChunksToPossibleVillage(world, spawn.chunkX, spawn.chunkY, Config::civilizationPlaceX);

    markCloseAsCivZone(
        world,
        spawn.chunkX,
        spawn.chunkY,
        0);
    initBuildings();
}

void Civilization::initBuildings()
{
    auto& cost = buildingsCost;
    auto& gains = buildingsGains;

    hitsForBuilding[HOUSE] = Config::hitsForBuildingHouse;
    cost[HOUSE].food = 0;
    cost[HOUSE].stone = Config::stoneNeededForHouse;
    cost[HOUSE].wood = Config::woodNeededForHouse;
    maxHumans[HOUSE] = Config::maxHumansInHouse;

    hitsForBuilding[FARM] = Config::hitsForBuildingFarm;
    cost[FARM].food = Config::foodNeededForFarm;
    cost[FARM].stone = 0;
    cost[FARM].wood = Config::woodNeededForFarm;
    maxHumans[FARM] = Config::maxFarmWorkersOnFarm;
    gains[FARM].food = Config::foodPerFarmWorker;

    hitsForBuilding[SAWMILL] = Config::hitsForBuildingSawmill;
    cost[SAWMILL].food = 0;
    cost[SAWMILL].stone = Config::stoneNeededForSawmill;
    cost[SAWMILL].wood = Config::woodNeededForSawmill;
    maxHumans[SAWMILL] = Config::maxSawmillWorkersInSawmill;
    gains[SAWMILL].wood = Config::woodPerSawmillWorker;

    hitsForBuilding[MINE] = Config::hitsForBuildingMine;
    cost[MINE].food = 0;
    cost[MINE].stone = Config::stoneNeededForMine;
    cost[MINE].wood = Config::woodNeededForMine;
    maxHumans[MINE] = Config::maxMineWorkersInMine;
    gains[MINE].stone = Config::stonePerMineWorker;

    /*
    hitsForBuilding[woodWall] = Config::hitsForBuildingWoodWall;
    cost[woodWall].food = 0;
    cost[woodWall].stone = 0;
    cost[woodWall].wood = Config::woodNeededForWoodWall;

    hitsForBuilding[stoneWall] = Config::hitsForBuildingStoneWall;
    cost[stoneWall].food = 0;
    cost[stoneWall].stone = Config::stoneNeededForStoneWall;
    cost[stoneWall].wood = 0;
    */
}

void Civilization::addWorkers(Human& human, HumanType targetType)
{
    int count = Config::partOfHumansChangingJobs;
    int added = 0;
    for (uint32_t i = 0; i < static_cast<uint32_t>(HumanType::COUNT) && added < count; i++)
    {
        HumanType sourceType = static_cast<HumanType>(i);
        if (targetType == sourceType) continue;
        dispatchToVector(sourceType, human, [&](auto& srcVec)
            {
                if (srcVec.posX.empty() || srcVec.posX.size() == 1) return;
                dispatchToVector(targetType, human, [&](auto& destVec)
                    {
                        while (added < count && !srcVec.posX.empty())
                        {
                            switchProfLast(human, srcVec, destVec, BuildingType::None);
                            added++;
                        }
                    });
            });
    }

}


void Civilization::civilizationDecision(Human& human, Food& food, Stone& stone, Tree& tree)
{
    bool areConstructions = false;
    for (int i = 0; i < COUNT; i++)
    {
        if (constructions[i] > 0)
        {
            areConstructions = true;
            continue;
        }
    }
    if (areConstructions && human.builders.posX.size() < human.humansCount/10)
    {
        addWorkers(human, HumanType::Builder);
    }

    if (resources.food <= Config::foodNeededForFarm)
    {
        addWorkers(human, HumanType::FoodCollector);
    }
    if (resources.wood < Config::woodNeededForFarm || resources.wood < Config::woodNeededForHouse || resources.stone < Config::stoneNeededForMine || food.foodsCount < Config::maxFood / 10)
    {
        addWorkers(human, HumanType::WoodCollector);
    }
    if (resources.stone < Config::stoneNeededForMine || food.foodsCount < Config::maxFood / 10)
    {
        addWorkers(human, HumanType::StoneCollector);
    }

}

void Civilization::markCloseAsCivZone(World& world, uint32_t chunkX, uint32_t chunkY, int rInChunks)
{
    for (int dy = -rInChunks; dy <= rInChunks; dy++)
    {
        for (int dx = -rInChunks; dx <= rInChunks; dx++)
        {
            int nx = chunkX + dx;
            int ny = chunkY + dy;
            if (!(world.isValidChunk(nx, ny))) continue;

            if (ny < mostNorthCivZone || mostNorthCivZone == -1)
            {
                mostNorthCivZone = ny;
            }
            if (ny > mostSouthCivZone)
            {
                mostSouthCivZone = ny;
            }
            if (nx < mostWestCivZone || mostWestCivZone == -1)
            {
                mostWestCivZone = nx;
            }
            if (nx > mostEastCivZone)
            {
                mostEastCivZone = nx;
            }

            world.setChunkFlag(
                nx,
                ny,
                ChunkFlag::CivZone
            );
        }
    }
}

void Civilization::addChunksToPossibleVillage(World& world, uint32_t chunkX, uint32_t chunkY, int rInChunks)
{
    for (int dy = -rInChunks; dy <= rInChunks; dy++)
    {
        for (int dx = -rInChunks; dx <= rInChunks; dx++)
        {
            int nx = chunkX + dx;
            int ny = chunkY + dy;
            if (!(world.isValidChunk(nx, ny))) continue;
            if (world.hasChunkFlag(nx, ny, ChunkFlag::CivZone)) continue;
            if (!(world.isChunkLand(nx, ny))) continue;

            bestChunksForBuildingsVillage.push_back(
                { static_cast<uint32_t>(nx),
                  static_cast<uint32_t>(ny) }
            );
        }
    }
}


Civilization::ChunkPos Civilization::getBestChunkForBuilingsVillage(World& world)
{
    int id;
    if (bestChunksForBuildingsVillage.empty()) return { UINT32_MAX, UINT32_MAX };
    ChunkPos pos;
    do
    {
        std::uniform_int_distribution<int> dist(0, bestChunksForBuildingsVillage.size() - 1);
        id = dist(rng);
        pos = bestChunksForBuildingsVillage[id];
        bestChunksForBuildingsVillage[id] = bestChunksForBuildingsVillage.back();
        bestChunksForBuildingsVillage.pop_back();
    } while (!bestChunksForBuildingsVillage.empty() &&
        (world.getBuilding(pos.chunkX, pos.chunkY) != BuildingType::None ||
            world.hasChunkFlag(pos.chunkX, pos.chunkY, ChunkFlag::CivZone)));
    if (
        world.getBuilding(pos.chunkX, pos.chunkY) != BuildingType::None ||
        world.hasChunkFlag(pos.chunkX, pos.chunkY, ChunkFlag::CivZone)
        )
    {
        return { UINT32_MAX, UINT32_MAX };
    }
    addChunksToPossibleVillage(
        world,
        pos.chunkX,
        pos.chunkY,
        2
    );
    markCloseAsCivZone(
        world,
        pos.chunkX,
        pos.chunkY,
        1
    );
    return pos;

}



void Civilization::buildBuilding(World& world, RendererSFML &renderer, Type type)
{
    if (resources.food >= buildingsCost[type].food
        && resources.stone >= buildingsCost[type].stone
        && resources.wood >= buildingsCost[type].wood)
    {
        auto pos = getBestChunkForBuilingsVillage(world);
        if (pos.chunkX == UINT32_MAX || pos.chunkY == UINT32_MAX) return;

        startConstruction(world, renderer, pos.chunkX, pos.chunkY, type);

        resources.food -= buildingsCost[type].food;
        resources.wood -= buildingsCost[type].wood;
        resources.stone -= buildingsCost[type].stone;
    }
}


void Civilization::assignHumansToBuilding(Human& human, Type type)
{
    if (type == HOUSE)
    {
        int humansWithoutHouse = human.humansCount - human.humansHavingHouseCount;
        int emptyPlaces = (buildingsCount[HOUSE] * Config::maxHumansInHouse) - human.humansHavingHouseCount;

        human.humansHavingHouseCount += std::min(humansWithoutHouse, emptyPlaces);
        return;
    }

    if (buildingsCount[type] == 0) return;

    int maxAssigned = buildingsCount[type] * maxHumans[type];

    for (uint32_t i = 0; i < static_cast<uint32_t>(HumanType::COUNT) &&
        workersAssigned[type] < maxAssigned; i++)
    {
        HumanType sourceType = static_cast<HumanType>(i);

        dispatchToVector(sourceType, human, [&](auto& srcVec)
        {
            if (srcVec.posX.empty()) return;

            while (workersAssigned[type] < maxAssigned && !srcVec.posX.empty())
            {
                switchProfLast(human, srcVec, human.assigned, GetBuildingType(type));
                workersAssigned[type]++;
            }
        });
    }
}
void Civilization::getBuildingsGains()
{
    for (int i = 0; i < COUNT; i++)
    {
        resources.food += buildingsGains[i].food * realWorkers[i];
        resources.wood += buildingsGains[i].wood * realWorkers[i];
        resources.stone += buildingsGains[i].stone * realWorkers[i];
    }
}




void Civilization::buildingDecision(World& world, RendererSFML &renderer, Human& human, Food& food, Stone& stone, Tree& tree)
{
    if ((buildingsCount[FARM] + constructions[FARM]) * 50 < human.humansCount)
    {
        buildBuilding(world, renderer, FARM);
    }

    if ((buildingsCount[SAWMILL] + constructions[SAWMILL]) * 260 < human.humansCount)
    {
        buildBuilding(world, renderer, SAWMILL);
    }

    if ((buildingsCount[MINE] + constructions[MINE]) * 2600 < human.humansCount)
    {
        buildBuilding(world, renderer, MINE);
    }

    if (buildingsCount[HOUSE] + constructions[HOUSE] < human.humansCount / 5)
    {
        buildBuilding(world, renderer, HOUSE);
    }
}
void Civilization::startConstruction(World& world, RendererSFML &renderer, uint32_t chunkX, uint32_t chunkY, Type type)
{
    world.setChunkFlag(chunkX, chunkY, ChunkFlag::Construction);
    world.setBuilding(chunkX, chunkY, GetBuildingType(type));
    sf::Color color;
    renderer.addChunkToDirtyCells(world, chunkX, chunkY, sf::Color(255, 128, 0));

    constructions[type]++;
}

void Civilization::endConstruction(World& world, RendererSFML &renderer, Human& human, uint32_t chunkX, uint32_t chunkY, Type type)
{
    world.clearChunkFlag(chunkX, chunkY, ChunkFlag::Construction);
    sf::Color color;
    switch(type)
    {
        {
        case Type::HOUSE:
            {
                color = sf::Color::Red;
                break;
            }

        case Type::FARM:
        {
            color = sf::Color(255, 255, 150);
            break;
        }
        case Type::SAWMILL:
        {
            color = sf::Color(165, 42, 42);
            break;
        }

        case Type::MINE:
        {
            color = sf::Color(191, 0, 255);
            break;
        }
        default:
            break;
        }
    }
    renderer.addChunkToDirtyCells(world, chunkX, chunkY, color);
    constructions[type]--;
    buildingsCount[type]++;
}