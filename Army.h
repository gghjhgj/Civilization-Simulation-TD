
#pragma once
#include <array>
#include <vector>
#include <algorithm>
#include <cmath>
#include <windows.h>
#include "Config.h"
class Civilization;
class World;
class RendererSFML;
class Human;
class Monsters;
class CombatSystem;

class Army
{
    public:
    Army();
    static Army* instance;

    struct ArmyShapeProfile
    {
        int currentSpacingX = 2;
        int targetSpacingX = 2;
        int currentSpacingY = 2;
        int targetSpacingY = 2;
        int currentNoise = 0;
        int targetNoise = 0;

        int currentWidth = 16;
        int targetWidth = 16;
    };

    enum ArmyProfession
    {
        soldier,
        archer,
        
        COUNT
    };
    struct Corners
    {
        int leftTop;
        int rightTop;
        int leftBot;
        int rightBot;
    };
    enum States
    {
        idle,
        moving,
        waitingForCombat,
        combat
    };
    struct ArmyData
    {
        std::vector<int> logicID;

        int armyMainIndex;
        int armyTargetIndex;
        int armyAssignedToProfession;
        ArmyShapeProfile armyShape;
        Corners corners;
        int area;
        States states;
        int targetType;

        int totalDMG;
        int totalHP;
        int avarageHP;
    };
    std::array<ArmyData, ArmyProfession::COUNT> armyRegistry;

    enum Dirs
    {
        left,
        right,
        up,
        down,
        none
    };
    void armyInit();
    int assignDecision();
    void addHumanToArmy(RendererSFML &renderer);
    void addHumansToArmy(World &world, Human &human, Civilization & civilization, RendererSFML &renderer, ArmyProfession profession);
    void giveArmyTargetIndex(Monsters &monsters, ArmyProfession profession);
    Dirs armyMoveDecision(ArmyProfession profession);
    void armyMove(Monsters &monsters, ArmyProfession profession);


    void spacingController(ArmyProfession profession);
    void posSpacing(ArmyProfession profession);

    void noiseController(ArmyProfession profession);

    void widthController(Monsters &monsters, ArmyProfession profession);
    void posWidth(ArmyProfession profession);

    void cornerController(ArmyProfession profession);
    void areaController(ArmyProfession profession, int realWidth, int realHeight);

    void targetMonstersDecision(Monsters &monsters, ArmyProfession profession);

    void positioningWhileCombat(Monsters &monsters, ArmyProfession profession);
    void armyController(Monsters &monsters);
};