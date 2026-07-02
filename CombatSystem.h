#pragma once
#include <iostream>
class Monsters;
class Army;

class CombatSystem
{
    public:
    CombatSystem();
    static CombatSystem* instance;

    Army *army;
    Monsters *monsters;

    int armiesReadyForCombat = 0;
    int armiesNeededForStartingCombat = Army::ArmyProfession::COUNT + Monsters::MonstersTypes::COUNT;



    static int getCoverage(int armyLeftTop, int ArmyRightBot, int monstersLeftTop, int MonstersRightBot);
    static float getCoveragePercent(int coverage, int area);
    static int getDMG(float coveragePercent, int totalDMG);
    static void incrementArmiesReady();
    static bool areArmiesReady();
    static void startCombat();
    static void endCombat();
};