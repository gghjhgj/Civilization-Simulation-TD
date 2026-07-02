#include "Army.h"
#include "Civilization.h"
#include "Human.h"
#include "World.h"
#include "RendererSFML.h"
#include "Monsters.h"
#include "CombatSystem.h"

Army* Army::instance = nullptr;

Army::Army() {
    instance = this;
}

void Army::armyInit()
{
    for(int i = 0; i < ArmyProfession::COUNT; i++)
    {
        ArmyProfession profession = ArmyProfession(i);
        /*
        if(i == 0)
        armyRegistry[profession].armyMainIndex = 500 * Config::sizeX + 900;
        else
        armyRegistry[profession].armyMainIndex = 800 * Config::sizeX + 300;
        armyRegistry[profession].armyTargetIndex = -1;
        */
       auto &entry = armyRegistry[profession];
       entry.spawnPoint = Config::civSpawnPoint + (200 * (i+1)) * Config::sizeX + 200;
       if(i == 0)
       {
            entry.armyMainIndex = entry.spawnPoint;
       }
       if(i == 1)
       {
            entry.armyMainIndex = entry.spawnPoint;
       }
       entry.states = States::idle;
        entry.armyTargetIndex = -1;
    }
}

int Army::assignDecision()
{
    int armyCount[ArmyProfession::COUNT];
    int smallest = armyRegistry[0].humansCount;
    int smallestProf = 0;
    for(int i = 1; i < ArmyProfession::COUNT; i++)
    {
        armyCount[i] = armyRegistry[i].humansCount;
        if(armyCount[i] < smallest)
        {
            smallest = armyCount[i];
            smallestProf = i;
        }
    }
    return smallestProf;
}
void Army::addHumanToArmy(RendererSFML &renderer)
{
    int profession = assignDecision();
    auto &entry = armyRegistry[profession];
    int vecID = entry.humansCount;
    int logicID = vecID;
    entry.humansCount++;
    entry.totalDMG += Config::humanInArmyDamage;
    entry.totalHP += Config::humanInArmyHP;
    entry.avarageHP = entry.totalHP / entry.humansCount;

    renderer.addProfToBuffer(profession, RendererSFML::Source::armyClass, logicID);
}

void Army::eraseHuman(RendererSFML &renderer, int profession, int id)
{
    auto &entry = armyRegistry[profession];

    int avarageDMG = entry.totalDMG / entry.humansCount;
    entry.totalDMG -= avarageDMG;

    entry.totalHP -= entry.avarageHP;
    
    entry.humansCount--;
    renderer.eraseProfFromBuffer(profession, RendererSFML::Source::armyClass, id);
}

void Army::addHumansToArmy(World &world, Human &human, Civilization &civilization,RendererSFML &renderer, ArmyProfession profession)
{
    int humansAddedToArmy = 0;
    if(human.humans.empty()) return;
    for(int i = human.humans.size() - 1; i >= 0 && humansAddedToArmy < Config::maxHumanCountAddedToArmy; i--)
    {
        if(human.humans[i].task == Human::Tasks::goingToBuilding) continue;

        human.eraseHuman(world, civilization, i);
        addHumanToArmy(renderer);
        humansAddedToArmy++;
    }
}
void Army::giveArmyTargetIndex(Monsters &monsters, ArmyProfession profession)
{
    //y * size + x
    if(armyRegistry[profession].states == States::idle)
    {
    auto &entry = armyRegistry[profession];
    int width = entry.armyShape.targetWidth;
    if(width == 1) return;
    int height = (width > 0) ? (entry.humansCount + width - 1) / width : 0; // 10/3 = 4 not 3
    int spacingX = entry.armyShape.targetSpacingX;
    int spacingY = entry.armyShape.targetSpacingY;
    int realWidth = width * spacingX - spacingX;
    int realHeight = height * spacingY - spacingY;
    
    int x = 800;
    int y = 500 - realHeight - (profession * 50) - 1;
 
    int start = y * Config::sizeX + x;
    armyRegistry[profession].armyTargetIndex = start;
    armyRegistry[profession].states = States::moving;
    }
}
Army::Dirs Army::armyMoveDecision(ArmyProfession profession)
{
    auto &entry = armyRegistry[profession];
    int armyIndex = entry.armyMainIndex;
    int targetIndex = entry.armyTargetIndex;
    if(armyIndex == targetIndex)
    {
        entry.armyTargetIndex = -1;
        entry.states = States::waitingForCombat;
        CombatSystem::incrementArmiesReady();
        if(CombatSystem::areArmiesReady())
        {
            CombatSystem::startCombat();
        }
    }

    int armyX = armyIndex % Config::sizeX;
    int armyY = armyIndex / Config::sizeX;

    int targetX = targetIndex % Config::sizeX;
    int targetY = targetIndex / Config::sizeX;
    if(targetIndex != -1)
    {
    if(armyX > targetX) return left;
    if(armyX < targetX) return right;
    if(armyY > targetY) return up;
    if(armyY < targetY) return down;
    }
    return none;
}

void Army::armyMove(Monsters &monsters, ArmyProfession profession)
{
    auto dir = armyMoveDecision(profession);
    switch(dir)
    {
        case left:
        {
            armyRegistry[profession].armyMainIndex--;
            break;
        }
        case right:
        {
            armyRegistry[profession].armyMainIndex++;
            break;
        }
        case up:
        {
            armyRegistry[profession].armyMainIndex -= Config::sizeX;
            break;
        }
        case down:
        {
            armyRegistry[profession].armyMainIndex += Config::sizeX;
            break;
        }
        case none:
        default:
        {
            giveArmyTargetIndex(monsters, profession);
            break;
        }
    }
}
void Army::spacingController(ArmyProfession profession)
{
    uint8_t currentX = armyRegistry[profession].armyShape.currentSpacingX;
    uint8_t currentY = armyRegistry[profession].armyShape.currentSpacingY;

    if(currentX == 1) armyRegistry[profession].armyShape.targetSpacingX = 10;
    else if(currentX == 10) armyRegistry[profession].armyShape.targetSpacingX = 1;

    if(currentY == 1) armyRegistry[profession].armyShape.targetSpacingY = 10;
    if(currentY == 10) armyRegistry[profession].armyShape.targetSpacingY = 1;
}

void Army::posSpacing(ArmyProfession profession)
{
    uint8_t currentX = armyRegistry[profession].armyShape.currentSpacingX;
    uint8_t currentY = armyRegistry[profession].armyShape.currentSpacingY;

    uint8_t targetX =  armyRegistry[profession].armyShape.targetSpacingX;
    uint8_t targetY =  armyRegistry[profession].armyShape.targetSpacingY;

    if(currentX < targetX)
    {
        currentX += 1;
    }
    else if(currentX > targetX)
    {
        currentX -= 1;
    }

    if(currentY < targetY)
    {
        currentY += 1;
    }
    else if(currentY > targetY)
    {
        currentY -= 1;
    }

    armyRegistry[profession].armyShape.currentSpacingX = currentX;
    armyRegistry[profession].armyShape.currentSpacingY = currentY;
}

void Army::noiseController(ArmyProfession profession)
{
    int maxSpacing = std::min(armyRegistry[profession].armyShape.currentSpacingX,  armyRegistry[profession].armyShape.currentSpacingY);
    armyRegistry[profession].armyShape.currentNoise = maxSpacing/2;
}

void Army::widthController(Monsters &monsters, ArmyProfession profession)
{
    if(monsters.monstersRegistry[0].monstersCount == 0)
    {
        armyRegistry[profession].armyShape.targetWidth = sqrt(armyRegistry[profession].humansCount);
        return;
    }
    int monstersCurrent = monsters.monstersRegistry[0].monstersShape.currentWidth * monsters.monstersRegistry[0].monstersShape.currentSpacingX -  monsters.monstersRegistry[0].monstersShape.currentSpacingX;
    armyRegistry[profession].armyShape.targetWidth = (monstersCurrent + armyRegistry[profession].armyShape.currentSpacingX) / armyRegistry[profession].armyShape.currentSpacingX;
}

void Army::posWidth(ArmyProfession profession)
{
    int current = armyRegistry[profession].armyShape.currentWidth;
    int target = armyRegistry[profession].armyShape.targetWidth;

    if(current < target)
    {
        current++;
    }
    if(current > target)
    {
        current--;
    }
    armyRegistry[profession].armyShape.currentWidth = current;
}

void Army::cornerController(ArmyProfession profession)
{
    auto &entry = armyRegistry[profession];

    int mainIndex = entry.armyMainIndex;
    int width = entry.armyShape.currentWidth;
    int height = (width > 0) ? (entry.humansCount + width - 1) / width : 0; // 10/3 = 4 not 3
    int spacingX = entry.armyShape.currentSpacingX;
    int spacingY = entry.armyShape.currentSpacingY;
    int realWidth = width * spacingX - spacingX + 1; // bez+1
    int realHeight = height * spacingY - spacingY + 1; //bez +1

    int leftTop = mainIndex;
    int rightTop = mainIndex + realWidth;
    int leftBot = mainIndex + (realHeight*Config::sizeX);
    int rightBot = leftBot + realWidth;

    entry.corners.leftTop = leftTop;
    entry.corners.rightTop = rightTop;
    entry.corners.leftBot = leftBot;
    entry.corners.rightBot = rightBot;

    areaController(profession, realWidth, realHeight);
}

void Army::areaController(ArmyProfession profession, int realWidth, int realHeight)
{
    armyRegistry[profession].area = realWidth * realHeight;
}



void Army::targetMonstersDecision(Monsters &monsters, ArmyProfession profession)
{
    int maxCoverage = 0;
    Monsters::MonstersTypes targetType = Monsters::MonstersTypes::COUNT;
    auto &armyEntry = armyRegistry[profession].corners;
    for(int i = 0; i < Monsters::MonstersTypes::COUNT; i++)
    {
        auto &monstersEntry = monsters.monstersRegistry[i].corners;
        int coverage = CombatSystem::getCoverage(
            armyEntry.leftTop,
            armyEntry.rightBot,
            monstersEntry.leftTop,
            monstersEntry.rightBot
        );
        if(coverage > maxCoverage)
        {
            maxCoverage = coverage;
            targetType = Monsters::MonstersTypes(i);
        }
    }
    if(targetType == Monsters::MonstersTypes::COUNT)
    {
        if(monsters.monstersRegistry[0].monstersCount != 0)
            targetType = Monsters::MonstersTypes::normalMonster;
        else if(monsters.monstersRegistry[1].monstersCount != 0)
            targetType = Monsters::MonstersTypes::giantMonster;
    }
    armyRegistry[profession].targetType = targetType;
}

void Army::positioningWhileCombat(Monsters &monsters, ArmyProfession profession)
{
    
}

int Army::enemiesToKill(Monsters &monsters, ArmyProfession profession, int DMG)
{
    auto &entry = armyRegistry[profession];
    int target = entry.targetType;
    int avarageHP = monsters.monstersRegistry[target].avarageHP;

    int enemiesToKill = DMG/avarageHP;
    entry.remainingDMG += DMG%avarageHP;

    return enemiesToKill;
}

int Army::eraseDecision(Monsters &monsters, int type)
{
    auto &monstersEntry = monsters.monstersRegistry[type];
    return monstersEntry.monstersCount - 1;
}

void Army::eraseEnemies(RendererSFML &renderer, Monsters &monsters, ArmyProfession profession, int count)
{
    auto &entry = armyRegistry[profession];
    int targetType = entry.targetType;
    for(int i = 0 ; i < count; i++)
    {
        int id = eraseDecision(monsters, targetType);
        if(id == -1) return;
        monsters.eraseMonster(renderer, targetType, id);
    }
}

void Army::armyComeBack(ArmyProfession profession)
{
    auto &entry = armyRegistry[profession];

    entry.armyTargetIndex = entry.spawnPoint;
    entry.states = Army::States::moving;
}
void Army::armyController(Monsters &monsters, RendererSFML &renderer)
{
    for(int i = 0; i < ArmyProfession::COUNT; i++)
    {
        ArmyProfession profession = ArmyProfession(i);
        auto &entry = armyRegistry[profession];
        //spacingController(profession);
        //posSpacing(profession);
        noiseController(profession);
        widthController(monsters, profession);
        posWidth(profession);
        if(monsters.monstersRegistry[0].monstersCount == 0 
            && monsters.monstersRegistry[1].monstersCount == 0 )
        {
            cornerController(profession);
            if(entry.states == Army::States::combat)
            {
                entry.states = Army::States::comingBack;
                CombatSystem::endCombat();
            }
            if(entry.states == Army::States::idle)
                continue;
        }
        if(armyRegistry[profession].states == Army::States::idle)
        {
            giveArmyTargetIndex(monsters, profession);
            continue;
        }
        if(armyRegistry[profession].states == Army::States::moving)
        {
            armyMove(monsters, profession);
            cornerController(profession);
            continue;
        }
        if(armyRegistry[profession].states == States::combat)
        {
            targetMonstersDecision(monsters, profession);
            cornerController(profession);
            int damage = 0;
            if(profession == Army::ArmyProfession::soldier)
            {
            auto &cornerEntry = entry.corners;
            int target = entry.targetType;
            auto &monstersEntry = monsters.monstersRegistry[target].corners;
            int coverage = CombatSystem::getCoverage(
                cornerEntry.leftTop,
                cornerEntry.rightBot,
                monstersEntry.leftTop,
                monstersEntry.rightBot
            );
            }
            else if(profession == Army::ArmyProfession::archer)
            {
                damage = entry.totalDMG;
            }
            damage += entry.remainingDMG;
            int enemiesCountToKill = enemiesToKill(monsters, profession, damage);
            eraseEnemies(renderer, monsters, profession, enemiesCountToKill);
            continue;
        }
        if(armyRegistry[profession].states == Army::States::comingBack)
        {
            armyComeBack(profession);
            continue;
        }
    }
}
