#pragma once

class Monsters;
class Army;

class CombatSystem
{
    public:

    static int getCoverage(int armyLeftTop, int ArmyRightBot, int monstersLeftTop, int MonstersRightBot);
    static float getCoveragePercent(int coverage, int area);
    static int getDMG(float coveragePercent, int totalDMG);
};