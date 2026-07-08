/*
#include "Streets.h"

void Streets::addStreet(World &world, Civilization &civilization, Human &human, int startIDX, int endIDX)
{
    int sx = startIDX % Config::sizeX;
    int sy = startIDX / Config::sizeX;
    int ex = endIDX % Config::sizeX;
    int ey = endIDX / Config::sizeX;
    int maxTr = 10000;
    Street street;
    street.id = streetsData.size();
    street.dest[0] = endIDX;
    street.dest[1] = startIDX;

    while (sx != ex || sy != ey)
{
    if (sx < ex && world.checkIfThereAreNoBuildings(world.index(sx + 1, sy)))
    {
        sx++;
    }
    else if (sx > ex && world.checkIfThereAreNoBuildings(world.index(sx - 1, sy)))
    {
        sx--;
    }
    else if (sy < ey && world.checkIfThereAreNoBuildings(world.index(sx, sy + 1)))
    {
        sy++;
    }
    else if (sy > ey && world.checkIfThereAreNoBuildings(world.index(sx, sy - 1)))
    {
        sy--;
    }
    else
    {
        break;
    }

    int index = world.index(sx, sy);
    street.streetData.push_back({street.id, index});
    world.addToDirtyCells(index);
    world.grid[index].streets.Street = 1;
    
    int pop = world.grid[index].popularity;
    int bucketIdx = world.grid[index].indexInBucket;

    if (bucketIdx != -1 &&
        pop >= 0 &&
        pop < world.popularityRanking.size())
    {
        auto& bucket = world.popularityRanking[pop];

        if (bucketIdx < bucket.size())
        {
            int lastTileIndex = bucket.back();
            bucket[bucketIdx] = lastTileIndex;
            world.grid[lastTileIndex].indexInBucket = bucketIdx;
            bucket.pop_back();
        }
    }

    world.grid[index].popularity = 0;
    world.grid[index].indexInBucket = -1;
}
    streetsData.push_back(std::move(street));
}
*/