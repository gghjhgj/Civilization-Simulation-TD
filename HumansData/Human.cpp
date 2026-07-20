#include "Human.h"

#include "WorldData/World.h"
#include <tbb/parallel_invoke.h>
#include "Civilization.h"

void Human::createHuman(World& world, Civilization& civilization)
{
    foodCollectors.reserve(100000);
    woodCollectors.reserve(100000);
    stoneCollectors.reserve(100000);
    builders.reserve(100000);
    assigned.reserve(100000);
    dead.reserve(10000);
    uint32_t x = civilization.spawn.chunkX * ChunkConfig::CHUNK_SIZE;
    uint32_t y = civilization.spawn.chunkY * ChunkConfig::CHUNK_SIZE;
    uint32_t x2;
    uint32_t y2;

    int maxRange = (Config::sizeX > Config::sizeY) ? Config::sizeX : Config::sizeY;

    for (int i = 0; i < Config::humanCount; i++)
    {
        int r = i + 1;
        if (r > maxRange) r = maxRange;

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

void Human::humanRespawn(World& world, Civilization& civilization)
{
    int newPeople = static_cast<int>(std::cbrt(humansCount));
    uint32_t x = civilization.spawn.chunkX * ChunkConfig::CHUNK_SIZE;
    uint32_t y = civilization.spawn.chunkY * ChunkConfig::CHUNK_SIZE;
    for (int i = 0; i < newPeople; i++)
    {
        addHuman(*this, this->foodCollectors, BuildingType::None, x, y);
    }
}
XY Human::humanFindResource(World& world, uint32_t x, uint32_t y, TerrainType type)
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
                return { static_cast<uint32_t>(nx), static_cast<uint32_t>(ny) };
        }
    }

    return { UINT32_MAX, UINT32_MAX };
}
XY Human::humanFindFlagChunk(World& world, uint32_t x, uint32_t y, ChunkFlag flag)
{
    auto ref = world.getCellRef(x, y);
    uint8_t vision = 1;
    for (int dy = -vision; dy <= vision; dy++)
    {
        for (int dx = -vision; dx <= vision; dx++)
        {
            int nx = ref.chunkX + dx;
            int ny = ref.chunkY + dy;

            if (!world.isValidChunk(nx, ny)) continue;

            if (world.hasChunkFlag(nx, ny, ChunkFlag::Construction))
            {
                uint32_t posX = nx * ChunkConfig::CHUNK_SIZE;
                uint32_t posY = ny * ChunkConfig::CHUNK_SIZE;

                return{ posX, posY };
            }
        }
    }
    return{ UINT32_MAX, UINT32_MAX };
}
XY Human::humanFindWorkingBuildingChunk(World& world, uint32_t x, uint32_t y, BuildingType type)
{
    auto ref = world.getCellRef(x, y);

    uint8_t vision = 1;
    for (int dy = -vision; dy <= vision; dy++)
    {
        for (int dx = -vision; dx <= vision; dx++)
        {
            int nx = ref.chunkX + dx;
            int ny = ref.chunkY + dy;

            if (!world.isValidChunk(nx, ny)) continue;

            if (world.getBuilding(nx, ny) == type &&
                !(world.hasChunkFlag(nx, ny, ChunkFlag::Construction)))
            {
                uint32_t posX = nx * ChunkConfig::CHUNK_SIZE;
                uint32_t posY = ny * ChunkConfig::CHUNK_SIZE;

                return{ posX, posY };
            }
        }
    }
    return{ UINT32_MAX, UINT32_MAX };
}
bool Human::gotResource(uint32_t hx, uint32_t hy, uint32_t rx, uint32_t ry)
{
    if (hx == rx && hy == ry) return true;
    return false;
}

Human::Dirs Human::humanMoveDecision(HumanBase& base)
{
    if (base.targetPos.x == UINT32_MAX || base.targetPos.y == UINT32_MAX)
    {
        uint32_t directionIndex = hash(base.points) & 7;
        static constexpr int lookupX[8] = { 1, 1, 0, -1, -1, -1,  0,  1 };
        static constexpr int lookupY[8] = { 0, 1, 1,  1,  0, -1, -1, -1 };
        return { static_cast<int8_t>(lookupX[directionIndex]), static_cast<int8_t>(lookupY[directionIndex]) };
    }

    int dx = base.targetPos.x - base.pos.x;
    int dy = base.targetPos.y - base.pos.y;
    int8_t dirX = (dx > 0) - (dx < 0);
    int8_t dirY = (dy > 0) - (dy < 0);

    return { dirX, dirY };
}
void Human::humanMove(World& world, Civilization& civilization, Food& food, Tree& tree, Stone& stone, RendererSFML &renderer)
{
    processHumanVector(foodCollectors, world, renderer, [&](auto& h, Dirs& dir, XY& newPos, bool& removed)
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
                food.foodsCount--;
                civilization.resources.food++;

                world.setCell(newPos.x, newPos.y, TerrainType::Land);
                renderer.addToDirtyCells(world, newPos.x, newPos.y, sf::Color::Green);
            }
        }

        return false;
    });


    processHumanVector(woodCollectors, world, renderer, [&](auto& h, Dirs& dir, XY& newPos, bool& removed)
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
                tree.treesCount--;
                civilization.resources.wood++;

                world.setCell(newPos.x, newPos.y, TerrainType::Land);
                renderer.addToDirtyCells(world, newPos.x, newPos.y, sf::Color::Green);
            }
        }

        return false;
    });


    processHumanVector(stoneCollectors, world, renderer, [&](auto& h, Dirs& dir, XY& newPos, bool& removed)
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
                stone.stonesCount--;
                civilization.resources.stone++;

                world.setCell(newPos.x, newPos.y, TerrainType::Mountain);
                renderer.addToDirtyCells(world, newPos.x, newPos.y, sf::Color::White);
            }
        }

        return false;
    });


    processHumanVector(builders, world, renderer, [&](auto& h, Dirs& dir, XY& newPos, bool& removed)
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

                civilization.endConstruction(
                    world,
                    renderer,
                    *this,
                    ref.chunkX,
                    ref.chunkY,
                    GetTypeBuilding(building)
                );
            }
            else
            {
                h.targetPos = { UINT32_MAX, UINT32_MAX };
            }
        }

        return false;
    });


    processHumanVector(assigned, world, renderer, [&](auto& h, Dirs& dir, XY& newPos, bool& removed)
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
                        civilization.realWorkers[FARM]++;
                        break;

                    case BuildingType::Sawmill:
                        civilization.realWorkers[SAWMILL]++;
                        break;

                    case BuildingType::Mine:
                        civilization.realWorkers[MINE]++;
                        break;

                    default:
                        break;
                }

                size_t id = &h - &assigned[0];
                eraseHuman(*this, assigned, static_cast<int>(id));

                return true;
            }
            else
            {
                h.targetPos = { UINT32_MAX, UINT32_MAX };
            }
        }

        return false;
    });
}