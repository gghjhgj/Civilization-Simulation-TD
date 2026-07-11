#pragma once

#include "WorldData/World.h"
class Civilization;

class Walls
{
    public:
    Walls();
    static Walls* instance;
    enum WallsTypes
    {
        woodenWall,
        stoneWall
    };
    std::vector<int> wallsIndexes;
    int lastWallIndexMarkedAsCivZone = 0;
    void adddWallTile(World &world, int index, WallsTypes type);
    void buildWalls(World &world, Civilization &civilization, WallsTypes type);
    void markWallsTilesAndCloseAsCivZoneAndAddTilesToPossible(World &world, Civilization &civilization, int r);
};