#pragma once

#include <cstdint>

class Civilization;
class Human;
class Stone;
class Food;
class Tree;

class Stats
{
public:
    struct Data
    {
        int ticksPerSecond = 0;
        int humanTicksPerSecond = 0;
        int FPS = 0;

        std::uint64_t totalTicks = 0;
        std::uint64_t totalHumanTicks = 0;

        std::size_t foodCount = 0;
        std::size_t treeCount = 0;
        std::size_t stoneCount = 0;

        std::size_t humansCount = 0;
        std::size_t humansOnMap = 0;

        std::size_t foodCollectors = 0;
        std::size_t woodCollectors = 0;
        std::size_t stoneCollectors = 0;
        std::size_t builders = 0;
        std::size_t assigned = 0;

        std::size_t humansHavingHouse = 0;

        std::size_t farmWorkers = 0;
        std::size_t sawmillWorkers = 0;
        std::size_t mineWorkers = 0;

        std::size_t housesUnderConstruction = 0;
        std::size_t farmsUnderConstruction = 0;
        std::size_t sawmillsUnderConstruction = 0;
        std::size_t minesUnderConstruction = 0;

        std::size_t houses = 0;
        std::size_t farms = 0;
        std::size_t sawmills = 0;
        std::size_t mines = 0;

        std::int64_t food = 0;
        std::int64_t wood = 0;
        std::int64_t stone = 0;

        std::size_t civilizationChancesLeft = 0;
    };

    void update(
        int ticks,
        int FPS,
        int humanTicks,
        Civilization& civilization,
        Human& human,
        Stone& stone,
        Food& food,
        Tree& tree,
        std::uint64_t totalTicks);

    void writeToTxt(
        const char* filename = "stats/stats.txt") const;

    const Data& get() const
    {
        return data;
    }

private:
    Data data;
};