#include "Monsters.h"
#include "Army.h"
#include "CombatSystem.h"

int CombatSystem::getCoverage(int armyLeftTop, int armyRightBot, int monstersLeftTop, int monstersRightBot)
{
    int armyFirstX = armyLeftTop % Config::sizeX;
    int armyLastX = armyRightBot % Config::sizeX;
    int armyMaxX = std::max(armyFirstX, armyLastX);
    int armyMinX = std::min(armyFirstX, armyLastX);

    int armyFirstY = armyLeftTop / Config::sizeX;
    int armyLastY = armyRightBot / Config::sizeX;
    int armyMaxY = std::max(armyFirstY, armyLastY);
    int armyMinY = std::min(armyFirstY, armyLastY);

    int monstersFirstX = monstersLeftTop % Config::sizeX;
    int monstersLastX = monstersRightBot % Config::sizeX;
    int monstersMaxX = std::max(monstersFirstX, monstersLastX);
    int monstersMinX = std::min(monstersFirstX, monstersLastX);

    int monstersFirstY = monstersLeftTop / Config::sizeX;
    int monstersLastY = monstersRightBot / Config::sizeX;
    int monstersMaxY = std::max(monstersFirstY, monstersLastY);
    int monstersMinY = std::min(monstersFirstY, monstersLastY);


    int startX = std::max(armyMinX, monstersMinX);
    int endX = std::min(armyMaxX, monstersMaxX);

    int startY = std::max(armyMinY, monstersMinY);
    int endY = std::min(armyMaxY, monstersMaxY);

    int coverageX = std::max(0, endX - startX);
    int coverageY = std::max(0, endY - startY);

    return coverageX * coverageY;
}

float CombatSystem::getCoveragePercent(int coverage, int area)
{
    return (float)coverage/(float)area;
}

int CombatSystem::getDMG(float coveragePercent, int totalDMG)
{
    return totalDMG * coveragePercent;
}