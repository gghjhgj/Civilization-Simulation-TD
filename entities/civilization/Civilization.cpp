#include "Civilization.h"
#include "../../Config/Config.h"
#include "../HumansData/Human.h"
#include "../../world/WorldData/World.h"

void Civilization::createCivilization(World &world)
{
    spawnChunk.chunkX = Config::civilization.spawnChunkX;
    spawnChunk.chunkY = Config::civilization.spawnChunkY;

    spawnXY.x = spawnChunk.chunkX * 3;
    spawnXY.y = spawnChunk.chunkY * 3;

    addChunksToPossibleVillage(
        world,
        spawnChunk.chunkX,
        spawnChunk.chunkY,
        1);

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

    if (Config::buildings.spawnWithResourcesFor2Buildings)
    {
        resources.stone += cost[MINE].stone;
        resources.stone += cost[SAWMILL].stone;
        resources.wood += cost[MINE].wood;
        resources.wood += cost[SAWMILL].wood;
    }
}

void Civilization::addWorkers(
    Human &human,
    HumanType sourceType,
    HumanType targetType,
    int count)
{
    if (sourceType == targetType || count <= 0)
        return;

    if (sourceType == HumanType::Builder)
        return;

    dispatchToVector(
        sourceType,
        human,
        [&](auto &srcVec)
        {
            dispatchToVector(
                targetType,
                human,
                [&](auto &destVec)
                {
                    const int available =
                        static_cast<int>(srcVec.posX.size());

                    const int toMove =
                        std::min(count, available);

                    for (int i = 0; i < toMove; i++)
                    {
                        switchProfLast(
                            human,
                            srcVec,
                            destVec,
                            BuildingType::None);
                    }
                });
        });
}

void Civilization::moveWorkersByPriority(
    Human &human,
    HumanType targetType,
    int count)
{
    if (count <= 0)
        return;

    const HumanType sources[] = {
        HumanType::WoodCollector,
        HumanType::FoodCollector,
        HumanType::StoneCollector};

    for (HumanType sourceType : sources)
    {
        if (sourceType == targetType)
            continue;

        int available = 0;

        dispatchToVector(
            sourceType,
            human,
            [&](auto &srcVec)
            {
                available =
                    static_cast<int>(srcVec.posX.size());
            });

        if (available <= 0)
            continue;

        const int toMove =
            std::min(count, available);

        addWorkers(
            human,
            sourceType,
            targetType,
            toMove);

        count -= toMove;

        if (count <= 0)
            return;
    }
}

void Civilization::civilizationDecision(
    Human &human,
    Food &food,
    Stone &stone,
    Tree &tree)
{
    const int population =
        static_cast<int>(human.foodCollectors.posX.size()) +
        static_cast<int>(human.woodCollectors.posX.size()) +
        static_cast<int>(human.stoneCollectors.posX.size()) +
        static_cast<int>(human.builders.posX.size()) +
        static_cast<int>(human.assigned.posX.size());

    const int assigned =
        static_cast<int>(human.assigned.posX.size());

    const int initialDesiredBuilders =
        population / 10;

    const bool hasFarm =
        buildingsCount[FARM] > 0 ||
        constructions[FARM] > 0;

    const int sawmillCount =
        buildingsCount[SAWMILL] +
        constructions[SAWMILL];

    const int mineCount =
        buildingsCount[MINE] +
        constructions[MINE];

    int desiredFoodCollectors = 0;

    if (!hasFarm)
        desiredFoodCollectors = population / 2;

    int availablePopulation =
        population -
        assigned -
        initialDesiredBuilders -
        desiredFoodCollectors;

    int desiredStoneCollectors = 0;
    int desiredWoodCollectors = 0;

    if (mineCount < 100)
    {
        desiredStoneCollectors =
            std::min(
                availablePopulation / 10,
                1000);

        availablePopulation -=
            desiredStoneCollectors;
    }

    if (sawmillCount < 100)
    {
        desiredWoodCollectors =
            availablePopulation;

        availablePopulation = 0;
    }

    int desiredBuilders =
        initialDesiredBuilders +
        availablePopulation;

    const int currentBuilders =
        static_cast<int>(
            human.builders.posX.size());

    const int currentFoodCollectors =
        static_cast<int>(
            human.foodCollectors.posX.size());

    const int currentWoodCollectors =
        static_cast<int>(
            human.woodCollectors.posX.size());

    const int currentStoneCollectors =
        static_cast<int>(
            human.stoneCollectors.posX.size());

    if (currentStoneCollectors > desiredStoneCollectors)
    {
        addWorkers(
            human,
            HumanType::StoneCollector,
            HumanType::WoodCollector,
            currentStoneCollectors -
                desiredStoneCollectors);
    }

    if (currentWoodCollectors > desiredWoodCollectors)
    {
        addWorkers(
            human,
            HumanType::WoodCollector,
            HumanType::StoneCollector,
            currentWoodCollectors -
                desiredWoodCollectors);
    }

    if (currentBuilders < desiredBuilders)
    {
        moveWorkersByPriority(
            human,
            HumanType::Builder,
            desiredBuilders -
                currentBuilders);
    }

    if (currentFoodCollectors < desiredFoodCollectors)
    {
        moveWorkersByPriority(
            human,
            HumanType::FoodCollector,
            desiredFoodCollectors -
                currentFoodCollectors);
    }

    if (currentFoodCollectors > desiredFoodCollectors)
    {
        addWorkers(
            human,
            HumanType::FoodCollector,
            HumanType::WoodCollector,
            currentFoodCollectors -
                desiredFoodCollectors);
    }

    const int updatedWoodCollectors =
        static_cast<int>(
            human.woodCollectors.posX.size());

    if (updatedWoodCollectors < desiredWoodCollectors)
    {
        moveWorkersByPriority(
            human,
            HumanType::WoodCollector,
            desiredWoodCollectors -
                updatedWoodCollectors);
    }

    const int updatedStoneCollectors =
        static_cast<int>(
            human.stoneCollectors.posX.size());

    if (updatedStoneCollectors < desiredStoneCollectors)
    {
        moveWorkersByPriority(
            human,
            HumanType::StoneCollector,
            desiredStoneCollectors -
                updatedStoneCollectors);
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
                            static_cast<int>(
                                Config::humans.range));
            }

            if (ny > civZoneChunks.mostSouthCivZone)
            {
                civZoneChunks.mostSouthCivZone = ny;
                civZoneTiles.mostSouthCivZone =
                    (ny + 1) *
                        ChunkConfig::CHUNK_SIZE -
                    1;

                ranges.mostSouthCivZone =
                    std::min(
                        static_cast<int>(
                            ConfigConstexpr::sizeY) -
                            1,
                        civZoneTiles.mostSouthCivZone +
                            static_cast<int>(
                                Config::humans.range));
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
                            static_cast<int>(
                                Config::humans.range));
            }

            if (nx > civZoneChunks.mostEastCivZone)
            {
                civZoneChunks.mostEastCivZone = nx;
                civZoneTiles.mostEastCivZone =
                    (nx + 1) *
                        ChunkConfig::CHUNK_SIZE -
                    1;

                ranges.mostEastCivZone =
                    std::min(
                        static_cast<int>(
                            ConfigConstexpr::sizeX) -
                            1,
                        civZoneTiles.mostEastCivZone +
                            static_cast<int>(
                                Config::humans.range));
            }

            world.setChunkFlag(
                nx,
                ny,
                ChunkFlag::CivZone);
        }
    }
}

void Civilization::addChunksToPossibleVillage(
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

            if (world.hasChunkFlag(
                    nx,
                    ny,
                    ChunkFlag::CivZone))
                continue;

            bestChunksForBuildingsVillage.push_back(
                {
                    static_cast<uint16_t>(nx),
                    static_cast<uint16_t>(ny)});
        }
    }
}

Civilization::ChunkPos
Civilization::getBestChunkForBuilingsVillage(
    World &world)
{
    int id;

    if (bestChunksForBuildingsVillage.empty())
        return {UINT16_MAX, UINT16_MAX};

    ChunkPos pos;

    do
    {
        std::uniform_int_distribution<int> dist(
            0,
            static_cast<int>(
                bestChunksForBuildingsVillage.size()) -
                1);

        id = dist(rng);
        pos = bestChunksForBuildingsVillage[id];

        bestChunksForBuildingsVillage[id] =
            bestChunksForBuildingsVillage.back();

        bestChunksForBuildingsVillage.pop_back();

    } while (
        !bestChunksForBuildingsVillage.empty() &&
        (
            world.getBuilding(
                pos.chunkX,
                pos.chunkY) !=
                BuildingType::None ||
            world.hasChunkFlag(
                pos.chunkX,
                pos.chunkY,
                ChunkFlag::CivZone)));

    if (
        world.getBuilding(
            pos.chunkX,
            pos.chunkY) !=
            BuildingType::None ||
        world.hasChunkFlag(
            pos.chunkX,
            pos.chunkY,
            ChunkFlag::CivZone))
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

bool Civilization::buildBuilding(
    World &world,
    RendererSFML &renderer,
    Type type)
{
    if (
        resources.food < buildingsCost[type].food ||
        resources.stone < buildingsCost[type].stone ||
        resources.wood < buildingsCost[type].wood)
    {
        return false;
    }

    auto pos =
        getBestChunkForBuilingsVillage(world);

    if (
        pos.chunkX == UINT16_MAX ||
        pos.chunkY == UINT16_MAX)
    {
        return false;
    }

    startConstruction(
        world,
        renderer,
        pos.chunkX,
        pos.chunkY,
        type);

    resources.food -=
        buildingsCost[type].food;

    resources.wood -=
        buildingsCost[type].wood;

    resources.stone -=
        buildingsCost[type].stone;

    return true;
}

void Civilization::assignHumansToBuilding(
    Human &human,
    Type type)
{
    if (type == HOUSE)
    {
        int humansWithoutHouse =
            human.humansCount -
            human.humansHavingHouseCount;

        int emptyPlaces =
            buildingsCount[HOUSE] *
                Config::buildings.house.maxHumans -
            human.humansHavingHouseCount;

        human.humansHavingHouseCount +=
            std::min(
                humansWithoutHouse,
                emptyPlaces);

        return;
    }

    if (buildingsCount[type] == 0)
        return;

    int maxAssigned =
        buildingsCount[type] *
        maxHumans[type];

    const HumanType sources[] = {
        HumanType::WoodCollector,
        HumanType::FoodCollector,
        HumanType::StoneCollector};

    for (HumanType sourceType : sources)
    {
        if (workersAssigned[type] >= maxAssigned)
            break;

        dispatchToVector(
            sourceType,
            human,
            [&](auto &srcVec)
            {
                while (
                    workersAssigned[type] <
                        maxAssigned &&
                    !srcVec.posX.empty())
                {
                    switchProfLast(
                        human,
                        srcVec,
                        human.assigned,
                        GetBuildingType(type));

                    workersAssigned[type]++;
                }
            });
    }
}

void Civilization::getBuildingsGains()
{
    for (int i = 0; i < COUNT; i++)
    {
        resources.food +=
            buildingsGains[i].food *
            realWorkers[i];

        resources.wood +=
            buildingsGains[i].wood *
            realWorkers[i];

        resources.stone +=
            buildingsGains[i].stone *
            realWorkers[i];
    }
}

void Civilization::buildingDecision(
    World &world,
    RendererSFML &renderer,
    Human &human,
    Food &food,
    Stone &stone,
    Tree &tree)
{
    if (!Config::buildings.buildBuildings)
        return;

    while (true)
    {
        const uint64_t foodAte =
            getFoodAte(human);

        const uint32_t collectors =
            human.foodCollectors.posX.size() +
            human.woodCollectors.posX.size() +
            human.stoneCollectors.posX.size();

        const uint32_t sawmillCount =
            buildingsCount[SAWMILL] +
            constructions[SAWMILL];

        const uint32_t houseCount =
            buildingsCount[HOUSE] +
            constructions[HOUSE];

        const uint32_t mineCount =
            buildingsCount[MINE] +
            constructions[MINE];

        if (sawmillCount == 0)
        {
            if (!buildBuilding(
                    world,
                    renderer,
                    Type::SAWMILL))
                return;

            continue;
        }

        if (mineCount == 0)
        {
            if (!buildBuilding(
                    world,
                    renderer,
                    Type::MINE))
                return;

            continue;
        }

        if (
            2 * foodAte > resources.food &&
            collectors >=
                Config::buildings.farm.maxWorkers)
        {
            if (!buildBuilding(
                    world,
                    renderer,
                    Type::FARM))
            {
                if (
                    human.humansCount >
                    houseCount *
                        Config::buildings.house.maxHumans)
                {
                    if (!buildBuilding(
                            world,
                            renderer,
                            Type::HOUSE))
                        return;
                }
            }

            continue;
        }

        if (
            collectors >=
                Config::buildings.mine.maxWorkers ||
            collectors >=
                Config::buildings.sawmill.maxWorkers)
        {
            if (mineCount <= sawmillCount)
            {
                if (!buildBuilding(
                        world,
                        renderer,
                        Type::MINE))
                    return;
            }
            else
            {
                if (!buildBuilding(
                        world,
                        renderer,
                        Type::SAWMILL))
                    return;
            }

            continue;
        }

        if (
            human.humansCount >
            houseCount *
                Config::buildings.house.maxHumans)
        {
            if (!buildBuilding(
                    world,
                    renderer,
                    Type::HOUSE))
                return;

            continue;
        }

        break;
    }
}

void Civilization::startConstruction(
    World &world,
    RendererSFML &renderer,
    uint16_t chunkX,
    uint16_t chunkY,
    Type type)
{
    world.cleanChunkResources(
        food,
        tree,
        stone,
        chunkX,
        chunkY,
        *this);

    world.setChunkFlag(
        chunkX,
        chunkY,
        ChunkFlag::Construction);

    world.setBuilding(
        chunkX,
        chunkY,
        GetBuildingType(type));

    uint16_t x =
        chunkX *
        ChunkConfig::CHUNK_SIZE;

    uint16_t y =
        chunkY *
        ChunkConfig::CHUNK_SIZE;

    sf::Color color =
        renderer.getColor(
            world,
            x,
            y);

    renderer.addChunkToDirtyCells(
        world,
        chunkX,
        chunkY,
        color);

    constructions[type]++;
}

void Civilization::endConstruction(
    World &world,
    RendererSFML &renderer,
    Human &human,
    uint16_t chunkX,
    uint16_t chunkY,
    Type type)
{
    if (!world.hasChunkFlag(
            chunkX,
            chunkY,
            ChunkFlag::Construction))
        return;

    world.clearChunkFlag(
        chunkX,
        chunkY,
        ChunkFlag::Construction);

    uint16_t x =
        chunkX *
        ChunkConfig::CHUNK_SIZE;

    uint16_t y =
        chunkY *
        ChunkConfig::CHUNK_SIZE;

    sf::Color color =
        renderer.getColor(
            world,
            x,
            y);

    renderer.addChunkToDirtyCells(
        world,
        chunkX,
        chunkY,
        color);

    if (constructions[type] > 0)
        --constructions[type];

    ++buildingsCount[type];
}

int64_t Civilization::getFoodAte(
    Human &human)
{
    const int64_t humansWithHouse =
        human.humansHavingHouseCount;

    const int64_t humansWithoutHouse =
        human.humansCount -
        humansWithHouse;

    const int64_t foodNeededWithoutHouse =
        Config::hunger.foodNeededForHumansToEat;

    const int64_t foodNeededWithHouse =
        std::max(
            int64_t{0},
            foodNeededWithoutHouse -
                Config::hunger
                    .foodReductionForHumansWithHouse);

    const uint64_t foodAte =
        humansWithoutHouse *
            foodNeededWithoutHouse +
        humansWithHouse *
            foodNeededWithHouse;

    return foodAte;
}

void Civilization::updateHunger(
    Human &human)
{
    uint64_t foodAte =
        getFoodAte(human);

    if (foodAte <= resources.food)
    {
        resources.food -= foodAte;
    }
    else
    {
        civilizationChancesLeft--;
    }
}

HumanType Civilization::respawnDecision(
    Human &human)
{
    const int population =
        static_cast<int>(
            human.foodCollectors.posX.size()) +
        static_cast<int>(
            human.woodCollectors.posX.size()) +
        static_cast<int>(
            human.stoneCollectors.posX.size()) +
        static_cast<int>(
            human.builders.posX.size()) +
        static_cast<int>(
            human.assigned.posX.size());

    const int assigned =
        static_cast<int>(
            human.assigned.posX.size());

    const int initialDesiredBuilders =
        population / 10;

    const bool hasFarm =
        buildingsCount[FARM] > 0 ||
        constructions[FARM] > 0;

    const int sawmillCount =
        buildingsCount[SAWMILL] +
        constructions[SAWMILL];

    const int mineCount =
        buildingsCount[MINE] +
        constructions[MINE];

    int desiredFoodCollectors = 0;

    if (!hasFarm)
        desiredFoodCollectors =
            population / 2;

    int availablePopulation =
        population -
        assigned -
        initialDesiredBuilders -
        desiredFoodCollectors;

    int desiredStoneCollectors = 0;
    int desiredWoodCollectors = 0;

    if (mineCount < 100)
    {
        desiredStoneCollectors =
            std::min(
                availablePopulation / 10,
                1000);

        availablePopulation -=
            desiredStoneCollectors;
    }

    if (sawmillCount < 100)
    {
        desiredWoodCollectors =
            availablePopulation;

        availablePopulation = 0;
    }

    const int desiredBuilders =
        initialDesiredBuilders +
        availablePopulation;

    const int builders =
        static_cast<int>(
            human.builders.posX.size());

    const int foodCollectors =
        static_cast<int>(
            human.foodCollectors.posX.size());

    const int woodCollectors =
        static_cast<int>(
            human.woodCollectors.posX.size());

    const int stoneCollectors =
        static_cast<int>(
            human.stoneCollectors.posX.size());

    if (builders < desiredBuilders)
        return HumanType::Builder;

    if (foodCollectors < desiredFoodCollectors)
        return HumanType::FoodCollector;

    if (woodCollectors < desiredWoodCollectors)
        return HumanType::WoodCollector;

    if (stoneCollectors < desiredStoneCollectors)
        return HumanType::StoneCollector;

    return HumanType::Builder;
}