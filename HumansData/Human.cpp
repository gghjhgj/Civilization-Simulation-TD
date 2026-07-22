#include "Human.h"

#include "WorldData/World.h"

#include "Civilization.h"

void Human::createHuman(World &world, Civilization &civilization)
{
    size_t threadsCount = tbb::this_task_arena::max_concurrency();

    threadResults.resize(threadsCount);


    foodCollectors.reserve(Config::humanCount + 40000);
    woodCollectors.reserve(Config::humanCount + 40000);
    stoneCollectors.reserve(Config::humanCount + 40000);
    builders.reserve(Config::humanCount + 40000);
    assigned.reserve(Config::humanCount + 40000);
    dead.reserve(10000);
    uint32_t x = civilization.spawn.chunkX * ChunkConfig::CHUNK_SIZE;
    uint32_t y = civilization.spawn.chunkY * ChunkConfig::CHUNK_SIZE;
    uint32_t x2;
    uint32_t y2;

    int maxRange = (Config::sizeX > Config::sizeY) ? Config::sizeX : Config::sizeY;

    for (int i = 0; i < Config::humanCount; i++)
    {
        int r = i + 1;
        if (r > maxRange)
            r = maxRange;

        do
        {
            int dx = (rand() % (2 * r + 1)) - r;
            int dy = (rand() % (2 * r + 1)) - r;

            x2 = x + dx;
            y2 = y + dy;
        } while (!world.isValid(x2, y2) ||
                 world.getCell(x2, y2) != TerrainType::Land);

        addHuman(*this, this->foodCollectors, BuildingType::None, x2, y2);
    }
}

void Human::humanRespawn(World &world, Civilization &civilization)
{
    int newPeople = static_cast<int>(std::cbrt(humansCount));
    uint32_t x = civilization.spawn.chunkX * ChunkConfig::CHUNK_SIZE;
    uint32_t y = civilization.spawn.chunkY * ChunkConfig::CHUNK_SIZE;
    for (int i = 0; i < newPeople; i++)
    {
        addHuman(*this, this->foodCollectors, BuildingType::None, x, y);
    }
}
XY Human::humanFindResource(World &world, uint32_t x, uint32_t y, TerrainType type)
{
    int startX = static_cast<int>(x);
    int startY = static_cast<int>(y);

    for (int dy = -Config::vision; dy <= Config::vision; dy++)
    {
        for (int dx = -Config::vision; dx <= Config::vision; dx++)
        {
            int nx = startX + dx;
            int ny = startY + dy;

            if (!world.isValid(nx, ny))
                continue;

            bool found = false;
            found = world.getCell(nx, ny) == type;

            if (found)
                return {static_cast<uint32_t>(nx), static_cast<uint32_t>(ny)};
        }
    }

    return {UINT32_MAX, UINT32_MAX};
}
XY Human::humanFindFlagChunk(World &world, uint32_t x, uint32_t y, ChunkFlag flag)
{
    auto ref = world.getCellRef(x, y);
    uint8_t vision = 1;
    for (int dy = -vision; dy <= vision; dy++)
    {
        for (int dx = -vision; dx <= vision; dx++)
        {
            int nx = ref.chunkX + dx;
            int ny = ref.chunkY + dy;

            if (!world.isValidChunk(nx, ny))
                continue;

            if (world.hasChunkFlag(nx, ny, ChunkFlag::Construction))
            {
                uint32_t posX = nx * ChunkConfig::CHUNK_SIZE;
                uint32_t posY = ny * ChunkConfig::CHUNK_SIZE;

                return {posX, posY};
            }
        }
    }
    return {UINT32_MAX, UINT32_MAX};
}
XY Human::humanFindWorkingBuildingChunk(World &world, uint32_t x, uint32_t y, BuildingType type)
{
    auto ref = world.getCellRef(x, y);

    uint8_t vision = 1;
    for (int dy = -vision; dy <= vision; dy++)
    {
        for (int dx = -vision; dx <= vision; dx++)
        {
            int nx = ref.chunkX + dx;
            int ny = ref.chunkY + dy;

            if (!world.isValidChunk(nx, ny))
                continue;

            if (world.getBuilding(nx, ny) == type &&
                !(world.hasChunkFlag(nx, ny, ChunkFlag::Construction)))
            {
                uint32_t posX = nx * ChunkConfig::CHUNK_SIZE;
                uint32_t posY = ny * ChunkConfig::CHUNK_SIZE;

                return {posX, posY};
            }
        }
    }
    return {UINT32_MAX, UINT32_MAX};
}
bool Human::gotResource(uint32_t hx, uint32_t hy, uint32_t rx, uint32_t ry)
{
    if (hx == rx && hy == ry)
        return true;
    return false;
}

Human::Dirs Human::humanMoveDecision(HumanBase &base)
{
    if (base.targetPos.x == UINT32_MAX || base.targetPos.y == UINT32_MAX)
    {
        uint32_t directionIndex = hash(base.points) & 7;
        static constexpr int lookupX[8] = {1, 1, 0, -1, -1, -1, 0, 1};
        static constexpr int lookupY[8] = {0, 1, 1, 1, 0, -1, -1, -1};
        return {static_cast<int8_t>(lookupX[directionIndex]), static_cast<int8_t>(lookupY[directionIndex])};
    }

    int dx = base.targetPos.x - base.pos.x;
    int dy = base.targetPos.y - base.pos.y;
    int8_t dirX = (dx > 0) - (dx < 0);
    int8_t dirY = (dy > 0) - (dy < 0);

    return {dirX, dirY};
}
void Human::humanMove(World &world, Civilization &civilization, Food &food, Tree &tree, Stone &stone, RendererSFML &renderer)
{
    for (auto &r : threadResults)
        r.clear();

    processHumanVector(foodCollectors, world, renderer, [&](auto &h, Dirs &dir, XY &newPos, bool &removed, int threadID)
                       {
            if (h.moves % (Config::vision + 1) == 0)
                h.targetPos = humanFindResource(world, h.pos.x, h.pos.y, TerrainType::LandWithFood);

            dir = humanMoveDecision(h);
            newPos = { h.pos.x + dir.x, h.pos.y + dir.y };

            if (h.targetPos.x != UINT32_MAX && h.targetPos.y != UINT32_MAX &&
                gotResource(newPos.x, newPos.y, h.targetPos.x, h.targetPos.y))
            {
                if (world.getCell(newPos.x, newPos.y) == TerrainType::LandWithFood)
                {
                    threadResults[threadID].foodCollected++;

                    world.setCell(newPos.x, newPos.y, TerrainType::Land);
                    renderer.addToDirtyBuffer(world, newPos.x, newPos.y, sf::Color::Green, threadID);
                }
            }

            return false; });

    processHumanVector(woodCollectors, world, renderer, [&](auto &h, Dirs &dir, XY &newPos, bool &removed, int threadID)
                       {
            if (h.moves % (Config::vision + 1) == 0)
                h.targetPos = humanFindResource(world, h.pos.x, h.pos.y, TerrainType::LandWithTree);

            dir = humanMoveDecision(h);
            newPos = { h.pos.x + dir.x, h.pos.y + dir.y };

            if (h.targetPos.x != UINT32_MAX && h.targetPos.y != UINT32_MAX &&
                gotResource(newPos.x, newPos.y, h.targetPos.x, h.targetPos.y))
            {
                if (world.getCell(newPos.x, newPos.y) == TerrainType::LandWithTree)
                {
                    threadResults[threadID].woodCollected++;

                    world.setCell(newPos.x, newPos.y, TerrainType::Land);
                    renderer.addToDirtyBuffer(world, newPos.x, newPos.y, sf::Color::Green, threadID);
                }
            }

            return false; });

    processHumanVector(stoneCollectors, world, renderer, [&](auto &h, Dirs &dir, XY &newPos, bool &removed, int threadID)
                       {
            if (h.moves % (Config::vision + 1) == 0)
                h.targetPos = humanFindResource(world, h.pos.x, h.pos.y, TerrainType::MountainWithStone);

            dir = humanMoveDecision(h);
            newPos = { h.pos.x + dir.x, h.pos.y + dir.y };

            if (h.targetPos.x != UINT32_MAX && h.targetPos.y != UINT32_MAX &&
                gotResource(newPos.x, newPos.y, h.targetPos.x, h.targetPos.y))
            {
                if (world.getCell(newPos.x, newPos.y) == TerrainType::MountainWithStone)
                {
                    threadResults[threadID].stoneCollected++;

                    world.setCell(newPos.x, newPos.y, TerrainType::Mountain);
                    renderer.addToDirtyBuffer(world, newPos.x, newPos.y, sf::Color::White, threadID);
                }
            }

            return false; });

    processHumanVector(builders, world, renderer, [&](auto &h, Dirs &dir, XY &newPos, bool &removed, int threadID)
                       {
            if (h.moves % (Config::vision + 1) == 0)
                h.targetPos = humanFindFlagChunk(world, h.pos.x, h.pos.y, ChunkFlag::Construction);

            dir = humanMoveDecision(h);
            newPos = { h.pos.x + dir.x, h.pos.y + dir.y };

            if (h.targetPos.x != UINT32_MAX && h.targetPos.y != UINT32_MAX &&
                gotResource(newPos.x, newPos.y, h.targetPos.x, h.targetPos.y))
            {
                auto ref = world.getCellRef(newPos.x, newPos.y);

                if (world.getBuilding(ref.chunkX, ref.chunkY) != BuildingType::None &&
                    world.hasChunkFlag(ref.chunkX, ref.chunkY, ChunkFlag::Construction))
                {
                    BuildingType building = world.getBuilding(ref.chunkX, ref.chunkY);
                    Type type = GetTypeBuilding(building);
                    threadResults[threadID].constr.push_back({ ref.chunkX, ref.chunkY, type });
                }
                else
                {
                    h.targetPos = { UINT32_MAX, UINT32_MAX };
                }
            }

            return false; });

    processHumanVector(assigned, world, renderer, [&](auto &h, Dirs &dir, XY &newPos, bool &removed, int threadID)
                       {
            if (h.moves % (Config::vision + 1) == 0)
            {
                h.targetPos =
                    humanFindWorkingBuildingChunk(world, h.pos.x, h.pos.y, h.targetBuilding);
            }

            dir = humanMoveDecision(h);

            newPos.x = h.pos.x + dir.x;
            newPos.y = h.pos.y + dir.y;

            if (h.targetPos.x != UINT32_MAX &&
                h.targetPos.y != UINT32_MAX &&
                gotResource(newPos.x, newPos.y,
                    h.targetPos.x,
                    h.targetPos.y))
            {
                auto ref = world.getCellRef(newPos.x, newPos.y);

                if (world.getBuilding(ref.chunkX, ref.chunkY) == h.targetBuilding &&
                    !world.hasChunkFlag(ref.chunkX, ref.chunkY, ChunkFlag::Construction))
                {
                    switch (h.targetBuilding)
                    {
                    case BuildingType::Farm:
                        threadResults[threadID].farmWorkersDelta++;
                        break;

                    case BuildingType::Sawmill:
                        threadResults[threadID].sawmillWorkersDelta++;
                        break;

                    case BuildingType::Mine:
                        threadResults[threadID].mineWorkersDelta++;
                        break;

                    default:
                        break;
                    }

                    size_t id = &h - &assigned[0];
                    threadResults[threadID].assignedRemoveQueue.push_back(id);

                    return true;
                }
                else
                {
                    h.targetPos = { UINT32_MAX, UINT32_MAX };
                }
            }

        return false; });

    ///////////////////////////sync

    for (const auto &res : threadResults)
    {
        food.foodsCount -= res.foodCollected;
        civilization.resources.food += res.foodCollected;

        tree.treesCount -= res.woodCollected;
        civilization.resources.wood += res.woodCollected;

        stone.stonesCount -= res.stoneCollected;
        civilization.resources.stone += res.stoneCollected;

        civilization.realWorkers[FARM] += res.farmWorkersDelta;
        civilization.realWorkers[SAWMILL] += res.sawmillWorkersDelta;
        civilization.realWorkers[MINE] += res.mineWorkersDelta;

        allAssignedToRemove.insert(
            allAssignedToRemove.end(),
            res.assignedRemoveQueue.begin(),
            res.assignedRemoveQueue.end());

        allConstructionsToEnd.insert(
            allConstructionsToEnd.end(),
            res.constr.begin(),
            res.constr.end());
    }

    if (!allAssignedToRemove.empty())
    {
        std::sort(allAssignedToRemove.rbegin(), allAssignedToRemove.rend());
        allAssignedToRemove.erase(std::unique(allAssignedToRemove.begin(), allAssignedToRemove.end()), allAssignedToRemove.end());

        for (size_t id : allAssignedToRemove)
        {
            if (id < assigned.size())
            {
                eraseHuman(*this, assigned, id);
            }
        }
        allAssignedToRemove.clear();
    }

    if (!allConstructionsToEnd.empty())
    {
        allConstructionsToEnd.erase(std::unique(allConstructionsToEnd.begin(), allConstructionsToEnd.end()), allConstructionsToEnd.end());

        for (auto constr : allConstructionsToEnd)
        {
            civilization.endConstruction(
                world,
                renderer,
                *this,
                constr.chunkX,
                constr.chunkY,
                constr.type);
        }
        allConstructionsToEnd.clear();
    }

    humanTicks++;

    if (humanTicks % Config::ticksForNewHumans == 0)
    {
        humanRespawn(world, civilization);
    }
    if (world.allTicksCount % Config::ticksForAssigningDecision == 0)
    {
        civilization.assignHumansToBuilding(*this, Type::FARM);
        civilization.assignHumansToBuilding(*this, Type::SAWMILL);
        civilization.assignHumansToBuilding(*this, Type::MINE);
    }
    if(humanTicks % Config::ticksForCivilizationDecision == 0)
    {
        civilization.civilizationDecision(*this, food, stone, tree);
    }
}