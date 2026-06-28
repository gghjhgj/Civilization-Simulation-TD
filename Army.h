
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

class Army
{
    public:
    Army();
    static Army* instance;
    struct ArmyShapeProfile
    {
        int currentSpacingX = 1;
        int targetSpacingX = 1;
        int currentSpacingY = 1;
        int targetSpacingY = 1;
        int currentNoise = 0;
        int targetNoise = 0;

        int currentWidth = 4;
        int targetWidth = 4;
    };

    enum ArmyProfession
    {
        soldier,
        archer,
        
        COUNT
    };
    struct ArmyData
    {
        std::vector<int> logicID;
        std::vector<int> hp;
        std::vector<int> stamina;
        std::vector<int> damage;
        void add(int logicIDARG, int hpARG = Config::humanInArmyHP, int staminaARG = Config::humanInArmyStamina, int damageARG = Config::humanInArmyDamage)
        {
            logicID.push_back(logicIDARG);
            hp.push_back(hpARG);
            stamina.push_back(staminaARG);
            damage.push_back(damageARG);
        }

        int armyMainIndex;
        int armyTargetIndex;
        int armyAssignedToProfession;
        int armyTotalDmg;
        ArmyShapeProfile armyShape;
    };
    std::array<ArmyData, ArmyProfession::COUNT> armyRegistry;

    struct XY
    {
        int x;
        int y;
    };
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
    void giveArmyTargetIndex(ArmyProfession profession);
    Dirs armyMoveDecision(ArmyProfession profession);
    void armyMove(ArmyProfession profession);


    void spacingController(ArmyProfession profession);
    void posSpacing(ArmyProfession profession);

    void noiseController(ArmyProfession profession);

    void widthController(ArmyProfession profession);
    void posWidth(ArmyProfession profession);
};