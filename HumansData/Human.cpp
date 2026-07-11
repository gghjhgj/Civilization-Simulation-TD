#include "Human.h"

#include "WorldData/World.h"
#include "Civilization.h"



void Human::addHuman(World& world, uint32_t x, uint32_t y, int vecId)
{
    HumanData h;
    h.targetBuilding = BuildingType::None;
    h.targetTerrain = TerrainType::LandWithTree;
    h.pos.x = x;
    h.pos.y = y;
    h.oldPos.x = x;
    h.oldPos.y = y;
    h.id = vecId;
    humans.push_back(h);
    world.addToDirtyCells(x, y);
    humansCount++;
}

void Human::createHuman(World& world, Civilization& civilization)
{
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

        addHuman(world, x2, y2, i);
    }
}

void Human::eraseHuman(World& world, Civilization& civilization, int vecID)
{
    deadHumans.push_back({
     {humans[vecID].oldPos.x, humans[vecID].oldPos.y},
     {humans[vecID].pos.x, humans[vecID].pos.y}
        });
    if (vecID < static_cast<int>(humans.size()) - 1)
    {
        auto& lastHuman = humans.back();
        humans[vecID] = lastHuman;
    }
    humans.pop_back();
}
void Human::humanRespawn(World& world, Civilization& civilization)
{
    int newPeople = static_cast<int>(std::cbrt(humansCount));
    uint32_t x = civilization.spawn.chunkX * ChunkConfig::CHUNK_SIZE;
    uint32_t y = civilization.spawn.chunkY * ChunkConfig::CHUNK_SIZE;
    for (int i = 0; i < newPeople; i++)
    {
        int id = humans.size();
        addHuman(world, x, y, id);
    }
}
Human::XY Human::humanFindResource(World& world, uint32_t x, uint32_t y, TerrainType type)
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
                return { nx, ny };
        }
    }

    return { UINT32_MAX, UINT32_MAX };
}
Human::XY Human::humanFindFlagChunk(World& world, uint32_t x, uint32_t y, ChunkFlag flag)
{
    int chunkX = static_cast<int>(x / ChunkConfig::CHUNK_SIZE);
    int chunkY = static_cast<int>(y / ChunkConfig::CHUNK_SIZE);

    for (int dy = -Config::vision; dy <= Config::vision; dy++)
    {
        for (int dx = -Config::vision; dx <= Config::vision; dx++)
        {
            int nx = chunkX + dx;
            int ny = chunkY + dy;

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
Human::XY Human::humanFindWorkingBuildingChunk(World& world, uint32_t x, uint32_t y, BuildingType type)
{
    int chunkX = static_cast<int>(x / ChunkConfig::CHUNK_SIZE);
    int chunkY = static_cast<int>(y / ChunkConfig::CHUNK_SIZE);

    for (int dy = -Config::vision; dy <= Config::vision; dy++)
    {
        for (int dx = -Config::vision; dx <= Config::vision; dx++)
        {
            int nx = chunkX + dx;
            int ny = chunkY + dy;

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

Human::Dirs Human::humanMoveDecision(uint32_t humanX, uint32_t humanY, uint32_t targetX, uint32_t targetY, int i)
{
    int dx = targetX - humanX;
    int dy = targetY - humanY;

    int8_t dirX = (dx > 0) - (dx < 0);
    int8_t dirY = (dy > 0) - (dy < 0);

    if (targetX != UINT32_MAX && targetY != UINT32_MAX && (rand() % 100 > 15))
    {
        return{ dirX, dirY };
    }

    uint32_t h = humans[i].id + humans[i].points;
    constexpr float PI = 3.14159265358979323846f;
    float angle = (h % 8) * PI / 4.f;
    dirX = std::lround(std::cos(angle));
    dirY = std::lround(std::sin(angle));
    return { dirX, dirY };
}
void Human::humanMove(World& world, Civilization& civilization, Food& food, Tree& tree, Stone& stone, Human& human)
{
    for (int i = 0; i < humans.size(); i++)
    {
        Dirs dir;
        XY newPos;
        bool humanRemoved = false;

        if (!humans[i].isBuilder && humans[i].targetBuilding == BuildingType::None)
        {
            if (humans[i].moves % (Config::vision + 1) == 0)
            {
                humans[i].targetPos = humanFindResource(world, humans[i].pos.x, humans[i].pos.y, humans[i].targetTerrain);
            }
            dir = humanMoveDecision(humans[i].pos.x, humans[i].pos.y,
                humans[i].targetPos.x, humans[i].targetPos.y,
                i);
            newPos.x = humans[i].pos.x + dir.x;
            newPos.y = humans[i].pos.y + dir.y;

            if (humans[i].targetPos.x != UINT32_MAX && humans[i].targetPos.y != UINT32_MAX &&
                gotResource(newPos.x, newPos.y,
                    humans[i].targetPos.x, humans[i].targetPos.y))
            {

                if (world.getCell(newPos.x, newPos.y) == humans[i].targetTerrain)
                {
                    switch (humans[i].targetTerrain)
                    {
                    case TerrainType::LandWithFood:
                    {
                        food.foodsCount--;
                        civilization.resources.food++;
                        world.setCell(newPos.x, newPos.y, TerrainType::Land);
                        world.addToDirtyCells(newPos.x, newPos.y);
                        break;
                    }
                    case TerrainType::LandWithTree:
                    {
                        tree.treesCount--;
                        civilization.resources.wood++;
                        world.setCell(newPos.x, newPos.y, TerrainType::Land);
                        world.addToDirtyCells(newPos.x, newPos.y);
                        break;
                    }
                    case TerrainType::MountainWithStone:
                    {
                        stone.stonesCount--;
                        civilization.resources.stone++;
                        world.setCell(newPos.x, newPos.y, TerrainType::Mountain);
                        world.addToDirtyCells(newPos.x, newPos.y);
                        break;
                    }
                    default: break;
                    }
                }
                else
                {
                    humans[i].targetPos = { UINT32_MAX, UINT32_MAX };
                }
            }
        }
        
        else if (humans[i].isBuilder && humans[i].targetBuilding == BuildingType::None)
        {
            if (humans[i].moves % (Config::vision + 1) == 0)
            {
                humans[i].targetPos = humanFindFlagChunk(world, humans[i].pos.x, humans[i].pos.y, ChunkFlag::Construction);
            }
            dir = humanMoveDecision(humans[i].pos.x, humans[i].pos.y,
                humans[i].targetPos.x, humans[i].targetPos.y,
                i);
            newPos.x = humans[i].pos.x + dir.x;
            newPos.y = humans[i].pos.y + dir.y;

            if (humans[i].targetPos.x != UINT32_MAX && humans[i].targetPos.y != UINT32_MAX &&
                gotResource(newPos.x, newPos.y,
                    humans[i].targetPos.x, humans[i].targetPos.y))
            {
                auto ref = world.getCellRef(newPos.x, newPos.y);
                if (world.getBuilding(ref.chunkX, ref.chunkY) != BuildingType::None &&
                    world.hasChunkFlag(ref.chunkX, ref.chunkY, ChunkFlag::Construction))
                {
                    BuildingType building = world.getBuilding(ref.chunkX, ref.chunkY);
                    Type type = GetTypeBuilding(building);
                    civilization.endConstruction(world, human, ref.chunkX, ref.chunkY, type);
                    world.addChunkToDirtyCells(ref.chunkX, ref.chunkY);
                }
                else
                {
                    humans[i].targetPos = { UINT32_MAX, UINT32_MAX };
                }
            }
        }

        else if (humans[i].targetBuilding != BuildingType::None)
        {
            if (humans[i].moves % (Config::vision + 1) == 0)
            {
                humans[i].targetPos = humanFindWorkingBuildingChunk(world, humans[i].pos.x, humans[i].pos.y, humans[i].targetBuilding);
            }
            dir = humanMoveDecision(humans[i].pos.x, humans[i].pos.y,
                humans[i].targetPos.x, humans[i].targetPos.y,
                i);
            newPos.x = humans[i].pos.x + dir.x;
            newPos.y = humans[i].pos.y + dir.y;

            if (humans[i].targetPos.x != UINT32_MAX && humans[i].targetPos.y != UINT32_MAX &&
                gotResource(newPos.x, newPos.y,
                    humans[i].targetPos.x, humans[i].targetPos.y))
            {
                auto ref = world.getCellRef(newPos.x, newPos.y);
                if (world.getBuilding(ref.chunkX, ref.chunkY) == humans[i].targetBuilding &&
                    !(world.hasChunkFlag(ref.chunkX, ref.chunkY, ChunkFlag::Construction)))
                {
                    switch (humans[i].targetBuilding)
                    {
                    case BuildingType::Farm:
                    {
                        civilization.realWorkers[FARM]++;
                        break;
                    }
                    case BuildingType::Sawmill:
                    {
                        civilization.realWorkers[SAWMILL]++;
                        break;
                    }
                    case BuildingType::Mine:
                    {
                        civilization.realWorkers[MINE]++;
                        break;
                    }
                    default: break;
                    }
                    eraseHuman(world, civilization, i);
                    humanRemoved = true;
                }
                else
                {
                    humans[i].targetPos = { UINT32_MAX, UINT32_MAX };
                }
            }
        }

        if (humanRemoved)
        {
            i--;
            continue;
        }
        if (world.isValid(newPos.x, newPos.y) &&
            world.getCell(newPos.x, newPos.y) != TerrainType::Water)
        {
            humans[i].moves++;
            humans[i].pos = { newPos.x, newPos.y };
        }
        else
        {
            humans[i].points -= (1 + (rand() % 7));
        }

        if (humans[i].points <= 0) humans[i].points += 1000;

        if (rand() % 100 < Config::chanceToChangeDir)
        {
            humans[i].points += (rand() % 2 == 0) ? 1 : -1;
        }
    }
}