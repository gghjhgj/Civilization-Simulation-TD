#include "Walls.h"
#include "Civilization.h"

Walls* Walls::instance = nullptr;

Walls::Walls() {
    instance = this;
}


/*
void Walls::adddWallTile(World &world, int index, WallsTypes type)
{
    switch(type)
    {
        case woodenWall:
        {
            world.grid[index].walls.woodWallHP = Config::woodWallHP;
            world.addToDirtyCells(index);
            break;
        }
        case stoneWall:
        {
            world.grid[index].walls.stoneWallHP = Config::stoneWallHP;
            world.addToDirtyCells(index);
            break;
        }
    }
}
    */

void Walls::buildWalls(World &world, Civilization &civilization, WallsTypes type)
{
    if (civilization.mostWestCivZone == -1 || civilization.mostEastCivZone == -1 ||
        civilization.mostNorthCivZone == -1 || civilization.mostSouthCivZone == -1)
    {
        return;
    }

    for(int i = civilization.mostWestCivZone; i <= civilization.mostEastCivZone; i++)
    {
        int indexTop = world.index(i, civilization.mostNorthCivZone);
        bool buildTop = false;
        if(world.isValid(indexTop))
        {
            if(!(world.grid[indexTop].flags & Water))
            {
                wallsIndexes.push_back(indexTop);
                buildTop = true;
            }
        }

        int indexBottom = world.index(i, civilization.mostSouthCivZone);
        bool buildBot = false;
        if(world.isValid(indexBottom))
        {
            if(!(world.grid[indexBottom].flags & Water))
            {
                wallsIndexes.push_back(indexBottom);
                buildBot = true;
            }
        }

        if(type == woodenWall)
        {
            if(buildTop)
            {
                civilization.resources.wood -= Config::woodNeededForWoodWall;
                civilization.startConstruction(world, indexTop, BuildingsType::woodWall);
            }
            if(buildBot)
            {
                civilization.resources.wood -= Config::woodNeededForWoodWall;
                civilization.startConstruction(world, indexBottom, BuildingsType::woodWall);
            }
        }
        else if(type == stoneWall)
        {
            if(buildTop)
            {
                civilization.resources.stone -= Config::stoneNeededForStoneWall;
                civilization.startConstruction(world, indexTop, BuildingsType::stoneWall);
            }
            if(buildBot)
            {
                civilization.resources.stone -= Config::stoneNeededForStoneWall;
                civilization.startConstruction(world, indexBottom, BuildingsType::stoneWall);
            }
        }
    }

    for(int i = civilization.mostNorthCivZone; i <= civilization.mostSouthCivZone; i++)
    {
        int indexLeft = world.index(civilization.mostWestCivZone, i);
        bool buildLeft = false;
        if(world.isValid(indexLeft))
        {
            if(!(world.grid[indexLeft].flags & Water))
            {
                wallsIndexes.push_back(indexLeft);
                buildLeft = true;
            }
        }

        int indexRight = world.index(civilization.mostEastCivZone, i);
        bool buildRight = false;
        if(world.isValid(indexRight))
        {
            if(!(world.grid[indexRight].flags & Water))
            {
                wallsIndexes.push_back(indexRight);
                buildRight = true;
            }
        }

        if(type == woodenWall)
        {
            if(buildLeft)
            {
                civilization.resources.wood -= Config::woodNeededForWoodWall;
                civilization.startConstruction(world, indexLeft, BuildingsType::woodWall);
            }
            if(buildRight)
            {
                civilization.resources.wood -= Config::woodNeededForWoodWall;
                civilization.startConstruction(world, indexRight, BuildingsType::woodWall);
            }
        }
        else if(type == stoneWall)
        {
            if(buildLeft)
            {
                civilization.resources.stone -= Config::stoneNeededForStoneWall;
                civilization.startConstruction(world, indexLeft, BuildingsType::stoneWall);
            }
            if(buildRight)
            {
                civilization.resources.stone -= Config::stoneNeededForStoneWall;
                civilization.startConstruction(world, indexRight, BuildingsType::stoneWall);
            }
        }
    }
    
    markWallsTilesAndCloseAsCivZoneAndAddTilesToPossible(world, civilization, 1);
}




void Walls::markWallsTilesAndCloseAsCivZoneAndAddTilesToPossible(World &world, Civilization &civilization, int r)
{
    for(int i = lastWallIndexMarkedAsCivZone; i < wallsIndexes.size(); i++)
    {
        civilization.markCloseAsCivZone(world, wallsIndexes[i], r);
        civilization.addTilesToPossibleVillage(world, wallsIndexes[i], r+1);
    }
    lastWallIndexMarkedAsCivZone = wallsIndexes.size() - 1;
}