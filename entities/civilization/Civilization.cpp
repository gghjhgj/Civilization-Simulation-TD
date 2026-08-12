#include "Civilization.h"
#include "../../Config/Config.h"
#include "../HumansData/Human.h"
#include "../../world/WorldData/World.h"


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
    spawnChunk.chunkX = Config::civilization.spawnChunkX;
    spawnChunk.chunkY = Config::civilization.spawnChunkY;
    spawnXY.x = spawnChunk.chunkX * 3;
    spawnXY.y = spawnChunk.chunkY * 3;
    addChunksToPossibleVillage(world, spawnChunk.chunkX, spawnChunk.chunkY, 1);


    markCloseAsCivZone(
        world,
        spawnChunk.chunkX,
        spawnChunk.chunkY,
        0);
    initBuildings();
}


void Civilization::initBuildings()
{
    auto &cost = buildingsCost;
    auto &gains = buildingsGains;


    cost[HOUSE].food = 0;
    cost[HOUSE].stone = Config::buildings.house.stoneRequired;
    cost[HOUSE].wood = Config::buildings.house.woodRequired;
    maxHumans[HOUSE] = Config::buildings.house.maxHumans;


    cost[FARM].food = Config::buildings.farm.foodRequired;
    cost[FARM].stone = 0;
    cost[FARM].wood = Config::buildings.farm.woodRequired;
    maxHumans[FARM] = Config::buildings.farm.maxWorkers;
    gains[FARM].food = Config::buildings.farm.foodPerWorker;


    cost[SAWMILL].food = 0;
    cost[SAWMILL].stone = Config::buildings.sawmill.stoneRequired;
    cost[SAWMILL].wood = Config::buildings.sawmill.woodRequired;
    maxHumans[SAWMILL] = Config::buildings.sawmill.maxWorkers;
    gains[SAWMILL].wood = Config::buildings.sawmill.woodPerWorker;


    cost[MINE].food = 0;
    cost[MINE].stone = Config::buildings.mine.stoneRequired;
    cost[MINE].wood = Config::buildings.mine.woodRequired;
    maxHumans[MINE] = Config::buildings.mine.maxWorkers;
    gains[MINE].stone = Config::buildings.mine.stonePerWorker;
}


void Civilization::addWorkers(Human &human, HumanType targetType)
{
    int count = Config::civilization.partOfHumansChangingJobs;
    int added = 0;
    for (uint32_t i = 0; i < static_cast<uint32_t>(HumanType::COUNT) && added < count; i++)
    {
        HumanType sourceType = static_cast<HumanType>(i);
        if (targetType == sourceType)
            continue;
        dispatchToVector(sourceType, human, [&](auto &srcVec)
                         {
                if (srcVec.posX.empty() || srcVec.posX.size() == 1) return;
                dispatchToVector(targetType, human, [&](auto& destVec)
                    {
                        while (added < count && !srcVec.posX.empty())
                        {
                            switchProfLast(human, srcVec, destVec, BuildingType::None);
                            added++;
                        }
                    }); });
    }
}


void Civilization::civilizationDecision(Human &human, Food &food, Stone &stone, Tree &tree)
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
    if (areConstructions && human.builders.posX.size() < human.humansCount / 10)
    {
        addWorkers(human, HumanType::Builder);
    }


    if (resources.food <= Config::buildings.farm.foodRequired)
    {
        addWorkers(human, HumanType::FoodCollector);
    }
    if (resources.wood < Config::buildings.farm.woodRequired ||
        resources.wood < Config::buildings.house.woodRequired ||
        resources.stone < Config::buildings.mine.stoneRequired ||
        food.foodsCount < Config::resources.food.max / 10)
    {
        addWorkers(human, HumanType::WoodCollector);
    }
    if (resources.stone < Config::buildings.mine.stoneRequired ||
        food.foodsCount < Config::resources.food.max / 10)
    {
        addWorkers(human, HumanType::StoneCollector);
    }
}


void Civilization::markCloseAsCivZone(
    World &world,
    uint16_t chunkX,
    uint16_t chunkY,
    int rInChunks)
{
    for (int dy = -rInChunks; dy <= rInChunks; dy++)
    {
        for (int dx = -rInChunks; dx <= rInChunks; dx++)
        {
            int nx = chunkX + dx;
            int ny = chunkY + dy;


            if (!world.isValidChunk(nx, ny))
                continue;


            if (ny < civZoneChunks.mostNorthCivZone ||
                civZoneChunks.mostNorthCivZone == -1)
            {
                civZoneChunks.mostNorthCivZone = ny;


                civZoneTiles.mostNorthCivZone =
                    ny * ChunkConfig::CHUNK_SIZE;


                ranges.mostNorthCivZone =
                    std::max(
                        0,
                        civZoneTiles.mostNorthCivZone -
                            static_cast<int>(Config::humans.range));
            }


            if (ny > civZoneChunks.mostSouthCivZone)
            {
                civZoneChunks.mostSouthCivZone = ny;


                civZoneTiles.mostSouthCivZone =
                    (ny + 1) * ChunkConfig::CHUNK_SIZE - 1;


                ranges.mostSouthCivZone =
                    std::min(
                        static_cast<int>(ConfigConstexpr::sizeY) - 1,
                        civZoneTiles.mostSouthCivZone +
                            static_cast<int>(Config::humans.range));
            }


            if (nx < civZoneChunks.mostWestCivZone ||
                civZoneChunks.mostWestCivZone == -1)
            {
                civZoneChunks.mostWestCivZone = nx;


                civZoneTiles.mostWestCivZone =
                    nx * ChunkConfig::CHUNK_SIZE;


                ranges.mostWestCivZone =
                    std::max(
                        0,
                        civZoneTiles.mostWestCivZone -
                            static_cast<int>(Config::humans.range));
            }


            if (nx > civZoneChunks.mostEastCivZone)
            {
                civZoneChunks.mostEastCivZone = nx;


                civZoneTiles.mostEastCivZone =
                    (nx + 1) * ChunkConfig::CHUNK_SIZE - 1;


                ranges.mostEastCivZone =
                    std::min(
                        static_cast<int>(ConfigConstexpr::sizeX) - 1,
                        civZoneTiles.mostEastCivZone +
                            static_cast<int>(Config::humans.range));
            }


            world.setChunkFlag(
                nx,
                ny,
                ChunkFlag::CivZone);
        }
    }
}


void Civilization::addChunksToPossibleVillage(World &world, uint16_t chunkX, uint16_t chunkY, int rInChunks)
{
    for (int dy = -rInChunks; dy <= rInChunks; dy++)
    {
        for (int dx = -rInChunks; dx <= rInChunks; dx++)
        {
            int nx = chunkX + dx;
            int ny = chunkY + dy;
            if (!(world.isValidChunk(nx, ny)))
                continue;
            if (world.hasChunkFlag(nx, ny, ChunkFlag::CivZone))
                continue;


            bestChunksForBuildingsVillage.push_back(
                {static_cast<uint16_t>(nx),
                 static_cast<uint16_t>(ny)});
        }
    }
}


Civilization::ChunkPos Civilization::getBestChunkForBuilingsVillage(World &world)
{
    int id;
    if (bestChunksForBuildingsVillage.empty())
        return {UINT16_MAX, UINT16_MAX};
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
        world.hasChunkFlag(pos.chunkX, pos.chunkY, ChunkFlag::CivZone))
    {
        return {UINT16_MAX, UINT16_MAX};
    }
    addChunksToPossibleVillage(
        world,
        pos.chunkX,
        pos.chunkY,
        2);
    markCloseAsCivZone(
        world,
        pos.chunkX,
        pos.chunkY,
        1);
    return pos;
}


void Civilization::buildBuilding(World &world, RendererSFML &renderer, Type type)
{
    if (resources.food >= buildingsCost[type].food &&
        resources.stone >= buildingsCost[type].stone &&
        resources.wood >= buildingsCost[type].wood)
    {
        auto pos = getBestChunkForBuilingsVillage(world);
        if (pos.chunkX == UINT16_MAX || pos.chunkY == UINT16_MAX)
            return;


        startConstruction(world, renderer, pos.chunkX, pos.chunkY, type);


        resources.food -= buildingsCost[type].food;
        resources.wood -= buildingsCost[type].wood;
        resources.stone -= buildingsCost[type].stone;
    }
}


void Civilization::assignHumansToBuilding(Human &human, Type type)
{
    if (type == HOUSE)
    {
        int humansWithoutHouse = human.humansCount - human.humansHavingHouseCount;
        int emptyPlaces = (buildingsCount[HOUSE] * Config::buildings.house.maxHumans) - human.humansHavingHouseCount;


        human.humansHavingHouseCount += std::min(humansWithoutHouse, emptyPlaces);
        return;
    }


    if (buildingsCount[type] == 0)
        return;


    int maxAssigned = buildingsCount[type] * maxHumans[type];


    for (uint32_t i = 0; i < static_cast<uint32_t>(HumanType::COUNT) &&
                         workersAssigned[type] < maxAssigned;
         i++)
    {
        HumanType sourceType = static_cast<HumanType>(i);


        dispatchToVector(sourceType, human, [&](auto &srcVec)
                         {
            if (srcVec.posX.empty()) return;


            while (workersAssigned[type] < maxAssigned && !srcVec.posX.empty())
            {
                switchProfLast(human, srcVec, human.assigned, GetBuildingType(type));
                workersAssigned[type]++;
            } });
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


void Civilization::buildingDecision(World &world, RendererSFML &renderer, Human &human, Food &food, Stone &stone, Tree &tree)
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

void Civilization::startConstruction(World &world, RendererSFML &renderer, uint16_t chunkX, uint16_t chunkY, Type type)
{
    world.cleanChunkResources(food, tree, stone, chunkX, chunkY, *this);
    world.setChunkFlag(chunkX, chunkY, ChunkFlag::Construction);
    world.setBuilding(chunkX, chunkY, GetBuildingType(type));
    uint16_t x = chunkX * ChunkConfig::CHUNK_SIZE;
    uint16_t y = chunkY * ChunkConfig::CHUNK_SIZE;
    sf::Color color = renderer.getColor(world, x, y);
    renderer.addChunkToDirtyCells(world, chunkX, chunkY, color);


    constructions[type]++;
}


void Civilization::endConstruction(World &world, RendererSFML &renderer, Human &human, uint16_t chunkX, uint16_t chunkY, Type type)
{
    if (!world.hasChunkFlag(chunkX, chunkY, ChunkFlag::Construction))
        return;


    world.clearChunkFlag(chunkX, chunkY, ChunkFlag::Construction);
    uint16_t x = chunkX * ChunkConfig::CHUNK_SIZE;
    uint16_t y = chunkY * ChunkConfig::CHUNK_SIZE;
    sf::Color color = renderer.getColor(world, x, y);
    renderer.addChunkToDirtyCells(world, chunkX, chunkY, color);
    constructions[type]--;
    buildingsCount[type]++;
}

void Civilization::updateHunger(Human& human)
{
    const int humansWithHouse =
        human.humansHavingHouseCount;

    const int humansWithoutHouse =
        human.humansCount - humansWithHouse;

    const int foodNeededWithoutHouse =
        Config::hunger.foodNeededForHumansToEat;

    const int foodNeededWithHouse =
        std::max(
            0,
            foodNeededWithoutHouse -
            Config::hunger.foodReductionForHumansWithHouse
        );

    const int foodAte =
        humansWithoutHouse * foodNeededWithoutHouse +
        humansWithHouse * foodNeededWithHouse;

    if (foodAte <= resources.food)
    {
        resources.food -= foodAte;
    }
    else
    {
        civilizationChancesLeft--;
    }
}