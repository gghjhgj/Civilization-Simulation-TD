#pragma once

#include <vector>
#include <cstdint>

enum CellFlags
    {
        Water = 1 << 0,
        Land = 1 << 1,
        Sand = 1 << 2,
        Mountain = 1 << 3
    };

    struct Buildings
    {
        uint8_t House = 0;
        uint8_t Farm = 0;
        uint8_t Sawmill = 0;
        uint8_t Mine = 0;
        uint8_t Factory = 0;
        uint8_t Construction = 0;
    };
    struct WallsData
    {
        int woodWallHP = 0;
        int stoneWallHP = 0;
    };
    /*
    struct ArmyTile
    {
        int armyVecID = -1;
        int armyProf = -1;
    };
    struct Streets
    {
        uint8_t Street = 0;
    };
    */
    struct Cell
    {
        uint8_t flags;

        uint8_t food;
        uint8_t treeHP;
        uint8_t stoneHP;

        uint8_t civilizationPlace;

        int humanIndex = -1;

        int civZone = 0;
        Buildings buildings;
        WallsData walls;

        //int popularity = 0;
        //int indexInBucket = 0;
        //ArmyTile armyTile;
        //Streets streets;
    };