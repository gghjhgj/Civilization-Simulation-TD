
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
class Army;

class Monsters
{
    public:
    Monsters();
    static Monsters* instance;
    struct MonstersShapeProfile
    {
        int currentSpacingX = 2;
        int targetSpacingX = 2;
        int currentSpacingY = 2;
        int targetSpacingY = 2;
        int currentNoise = 0;
        int targetNoise = 0;

        int currentWidth = 1;
        int targetWidth = 1;
    };

    enum MonstersTypes
    {
        normalMonster,
        giantMonster,
        
        COUNT
    };
    struct MonstersData
    {
        std::vector<int> logicID;
        std::vector<int> hp;
        std::vector<int> damage;
        void addNormalMonster(int logicIDARG, int hpARG = Config::normalMonsterHP, int damageARG = Config::normalMonsterDamage)
        {
            logicID.push_back(logicIDARG);
            hp.push_back(hpARG);
            damage.push_back(damageARG);
        }

        void addGiant(int logicIDARG, int hpARG = Config::giantMonsterHP, int damageARG = Config::giantMonsterDamage)
        {
            logicID.push_back(logicIDARG);
            hp.push_back(hpARG);
            damage.push_back(damageARG);
        }

        int monstersSpawn;
        int monstersMainIndex;
        int monstersTargetIndex;
        int monstersAssignedToType;
        int monstersTotalDMG;
        MonstersShapeProfile monstersShape;
    };
    std::array<MonstersData, MonstersTypes::COUNT> monstersRegistry;

    enum Dirs
    {
        left,
        right,
        up,
        down,
        none
    };

    //void monstersResize();
    void spawnDecision(Civilization &civ);
    void waveInit();

    void monstersCreate(RendererSFML &renderer);
    
    void giveMonstersTargetIndex(MonstersTypes types);
    Dirs monstersMoveDecision(MonstersTypes types);
    void monstersMove(MonstersTypes types);

    void spacingController(MonstersTypes types);
    void posSpacing(MonstersTypes types);

    void noiseController(MonstersTypes types);

    void widthController(MonstersTypes types);
    void posWidth(MonstersTypes types);


    void monstersController(Army &army);
};