#pragma once
#include "Civilization.h"
#include "WorldData/World.h"
#include "HumansData/Human.h"
class Streets
{
    public:
    struct StreetTile
    {
        int id;
        int index;
    };
    struct Street
    {
        int id;
        int dest[2];
        std::vector <StreetTile> streetData;
    };
    std::vector <Street> streetsData;

    void addStreet(World &world, Civilization &civilization, Human &human, int startIDX, int endIDX);
};