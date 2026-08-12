#include "Stats.h"

#include <fstream>
#include <filesystem>

#include "../entities/civilization/Civilization.h"
#include "../entities/HumansData/Human.h"

#include "../world/resources/Food.h"
#include "../world/resources/Stone.h"
#include "../world/resources/Tree.h"

void Stats::update(
    int ticks,
    int FPS,
    int humanTicks,
    Civilization& civilization,
    Human& human,
    Stone& stoneResource,
    Food& foodResource,
    Tree& treeResource,
    std::uint64_t totalTicks)
{
    data.ticksPerSecond = ticks;
    data.humanTicksPerSecond = humanTicks;
    data.FPS = FPS;

    data.totalTicks = totalTicks;
    data.totalHumanTicks = human.humanTicks;

    data.foodCount = foodResource.foodsCount;
    data.treeCount = treeResource.treesCount;
    data.stoneCount = stoneResource.stonesCount;

    data.civilizationChancesLeft =
        civilization.civilizationChancesLeft;

    data.humansCount = human.humansCount;

    data.foodCollectors =
        human.foodCollectors.posX.size();

    data.woodCollectors =
        human.woodCollectors.posX.size();

    data.stoneCollectors =
        human.stoneCollectors.posX.size();

    data.builders =
        human.builders.posX.size();

    data.assigned =
        human.assigned.posX.size();

    data.humansOnMap =
        data.foodCollectors +
        data.woodCollectors +
        data.stoneCollectors +
        data.builders +
        data.assigned;

    data.humansHavingHouse =
        human.humansHavingHouseCount;

    data.farmWorkers =
        civilization.realWorkers[FARM];

    data.sawmillWorkers =
        civilization.realWorkers[SAWMILL];

    data.mineWorkers =
        civilization.realWorkers[MINE];

    data.food =
        civilization.resources.food;

    data.wood =
        civilization.resources.wood;

    data.stone =
        civilization.resources.stone;

    data.housesUnderConstruction =
        civilization.constructions[HOUSE];

    data.farmsUnderConstruction =
        civilization.constructions[FARM];

    data.sawmillsUnderConstruction =
        civilization.constructions[SAWMILL];

    data.minesUnderConstruction =
        civilization.constructions[MINE];

    data.houses =
        civilization.buildingsCount[HOUSE];

    data.farms =
        civilization.buildingsCount[FARM];

    data.sawmills =
        civilization.buildingsCount[SAWMILL];

    data.mines =
        civilization.buildingsCount[MINE];
}

void Stats::writeToTxt(const char* filename) const
{
    const std::filesystem::path filePath(filename);

    if (filePath.has_parent_path())
    {
        std::filesystem::create_directories(
            filePath.parent_path()
        );
    }

    std::ofstream statsFile(filePath);

    if (!statsFile.is_open())
    {
        return;
    }

    statsFile << "=== STATYSTYKI SYMULACJI ===\n";

    statsFile << "Ticks per second: "
              << data.ticksPerSecond << "\n";

    statsFile << "Human ticks per second: "
              << data.humanTicksPerSecond << "\n";

    statsFile << "FPS: "
              << data.FPS << "\n";

    statsFile << "Total Ticks: "
              << data.totalTicks << "\n";

    statsFile << "Total human ticks: "
              << data.totalHumanTicks << "\n\n";

    statsFile << "=== ZASOBY SWIATA ===\n";

    statsFile << "Ilosc jedzenia: "
              << data.foodCount << "\n";

    statsFile << "Ilosc drzew: "
              << data.treeCount << "\n";

    statsFile << "Ilosc kamieni: "
              << data.stoneCount << "\n\n";

    statsFile << "=== LUDZIE ===\n";

    statsFile << "Ilosc ludzi: "
              << data.humansCount << "\n";

    statsFile << "Ilosc ludzi na mapie: "
              << data.humansOnMap << "\n";

    statsFile << "Food collectors: "
              << data.foodCollectors << "\n";

    statsFile << "Wood collectors: "
              << data.woodCollectors << "\n";

    statsFile << "Stone collectors: "
              << data.stoneCollectors << "\n";

    statsFile << "Builders: "
              << data.builders << "\n";

    statsFile << "Assigned: "
              << data.assigned << "\n";

    statsFile << "Ilosc ludzi posiadajacych dom: "
              << data.humansHavingHouse << "\n\n";

    statsFile << "=== PRACOWNICY ===\n";

    statsFile << "Ludzie na farmach: "
              << data.farmWorkers << "\n";

    statsFile << "Ludzie w tartakach: "
              << data.sawmillWorkers << "\n";

    statsFile << "Ludzie w kopalniach: "
              << data.mineWorkers << "\n\n";

    statsFile << "=== ZASOBY CYWILIZACJI ===\n";

    statsFile << "Jedzenie: "
              << data.food << "\n";

    statsFile << "Drewno: "
              << data.wood << "\n";

    statsFile << "Kamienie: "
              << data.stone << "\n";

    statsFile << "szanse cywilizacji: "
              << data.civilizationChancesLeft << "\n\n";

    statsFile << "=== BUDYNKI ===\n";

    statsFile << "Domy w budowie: "
              << data.housesUnderConstruction << "\n";

    statsFile << "Farmy w budowie: "
              << data.farmsUnderConstruction << "\n";

    statsFile << "Tartaki w budowie: "
              << data.sawmillsUnderConstruction << "\n";

    statsFile << "Kopalnie w budowie: "
              << data.minesUnderConstruction << "\n";

    statsFile << "Domy: "
              << data.houses << "\n";

    statsFile << "Farmy: "
              << data.farms << "\n";

    statsFile << "Tartaki: "
              << data.sawmills << "\n";

    statsFile << "Kopalnie: "
              << data.mines << "\n";
}