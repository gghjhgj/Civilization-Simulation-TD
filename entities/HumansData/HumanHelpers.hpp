#pragma once

#include "HumanTypes.h"
#include "../civilization/Civilization.h"
#include "../../world/WorldData/World.h"

class Human;

inline uint32_t fastRandom(uint32_t x)
{
    x ^= x >> 16;
    x *= 0x45d9f3b;
    x ^= x >> 16;
    x *= 0x45d9f3b;
    x ^= x >> 16;
    return x;
}

inline bool isInCivZone(
    uint16_t x,
    uint16_t y,
    const Civilization &civ)
{
    return x >= civ.civZoneTiles.mostWestCivZone &&
           x <= civ.civZoneTiles.mostEastCivZone &&
           y >= civ.civZoneTiles.mostNorthCivZone &&
           y <= civ.civZoneTiles.mostSouthCivZone;
}
inline void setWorkerTargetAsCivSpawn(
    Civilization &civ,
    HumanBase &h,
    size_t i)
{
    if (h.posX[i] < civ.civZoneTiles.mostWestCivZone ||
        h.posX[i] > civ.civZoneTiles.mostEastCivZone ||
        h.posY[i] < civ.civZoneTiles.mostNorthCivZone ||
        h.posY[i] > civ.civZoneTiles.mostSouthCivZone)
    {
        h.targetX[i] = civ.spawnXY.x;
        h.targetY[i] = civ.spawnXY.y;
    }
}

inline bool isInCivRange(
    uint16_t x,
    uint16_t y,
    const Civilization& civilization)
{
    return x >= civilization.ranges.mostWestCivZone &&
           x <= civilization.ranges.mostEastCivZone &&
           y >= civilization.ranges.mostNorthCivZone &&
           y <= civilization.ranges.mostSouthCivZone;
}

inline void setWorkerTargetAsCivRangeBoundary(
    Civilization &civ,
    HumanBase &h,
    size_t i)
{
    uint16_t x = h.posX[i];
    uint16_t y = h.posY[i];

    if (isInCivRange(x, y, civ))
        return;

    int dx = 0;
    int dy = 0;

    if (x < civ.ranges.mostWestCivZone)
        dx = civ.ranges.mostWestCivZone - x;
    else if (x > civ.ranges.mostEastCivZone)
        dx = x - civ.ranges.mostEastCivZone;

    if (y < civ.ranges.mostNorthCivZone)
        dy = civ.ranges.mostNorthCivZone - y;
    else if (y > civ.ranges.mostSouthCivZone)
        dy = y - civ.ranges.mostSouthCivZone;

    if (dx > dy)
    {
        h.targetX[i] =
            x < civ.ranges.mostWestCivZone
                ? civ.ranges.mostWestCivZone
                : civ.ranges.mostEastCivZone;

        h.targetY[i] = y;
    }
    else
    {
        h.targetX[i] = x;

        h.targetY[i] =
            y < civ.ranges.mostNorthCivZone
                ? civ.ranges.mostNorthCivZone
                : civ.ranges.mostSouthCivZone;
    }
}

inline bool isOnMountain(
    World &world,
    uint16_t x,
    uint16_t y)
{
    TerrainType type = world.getCell(x, y);

    return type == TerrainType::Mountain ||
           type == TerrainType::MountainWithStone;
}

inline void setStoneCollectorMountainTarget(
    World &world,
    HumanBase &h,
    size_t i)
{
    if (world.getCell(h.posX[i], h.posY[i]) ==
        TerrainType::MountainWithStone)
    {
        return;
    }

    uint32_t seed =
        static_cast<uint32_t>(i) ^
        (static_cast<uint32_t>(h.points[i]) * 0x9E3779B9u);

    uint32_t mountainIndex =
        fastRandom(seed) % ConfigConstexpr::numberOfMountains;

    const auto &mountain =
        world.mountainsRanges[mountainIndex];

    h.targetX[i] =
        mountain.minX +
        (mountain.maxX - mountain.minX) / 2;

    h.targetY[i] =
        mountain.minY +
        (mountain.maxY - mountain.minY) / 2;
}