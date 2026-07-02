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
       if(i == 0)
       {
        armyRegistry[profession].armyMainIndex = Config::civSpawnPoint + 200 * Config::sizeX + 200;
       }
       if(i == 1)
       {
        armyRegistry[profession].armyMainIndex = Config::civSpawnPoint + 100 * Config::sizeX + 200;
       }
       armyRegistry[profession].states = States::idle;
        armyRegistry[profession].armyTargetIndex = -1;
    }
}

int Army::assignDecision()
{
    int armyCount[ArmyProfession::COUNT];
    int smallest = armyRegistry[0].hp.size();
    int smallestProf = 0;
    for(int i = 1; i < ArmyProfession::COUNT; i++)
    {
        armyCount[i] = armyRegistry[i].hp.size();
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
    int vecID = armyRegistry[profession].hp.size();
    int logicID = vecID;
    armyRegistry[profession].add(logicID);
    armyRegistry[profession].armyTotalDMG += Config::humanInArmyDamage;
    renderer.addProfToBuffer(profession, RendererSFML::Source::armyClass, logicID);
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
    int height = (width > 0) ? (entry.hp.size() + width - 1) / width : 0; // 10/3 = 4 not 3
    int spacingX = entry.armyShape.targetSpacingX;
    int spacingY = entry.armyShape.targetSpacingY;
    int realWidth = width * spacingX - spacingX;
    int realHeight = height * spacingY - spacingY;
    
    int x = 800;
    int y = 500 - realHeight - (profession * 30) - 1;
    /* debug
    std::cout << "width " << width << std::endl;
    std::cout << "size " << entry.hp.size() << std::endl;
    std::cout << "height " << height << std::endl;
    std::cout << "spacingX " << spacingX << std::endl;
    std::cout << "spacingY " << spacingY << std::endl;
    std::cout << "realWidth " << realWidth << std::endl;
    std::cout << "realHight " << realHeight << std::endl;
    std::cout<<y << std::endl;
    */
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
    if(monsters.monstersRegistry[0].hp.empty())
    {
        armyRegistry[profession].armyShape.targetWidth = sqrt(armyRegistry[profession].hp.size());
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
    int height = (width > 0) ? (entry.hp.size() + width - 1) / width : 0; // 10/3 = 4 not 3
    int spacingX = entry.armyShape.currentSpacingX;
    int spacingY = entry.armyShape.currentSpacingY;
    int realWidth = width * spacingX - spacingX;
    int realHeight = height * spacingY - spacingY;

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
    armyRegistry[profession].targetType = targetType;
}

void Army::positioningWhileCombat(Monsters &monsters, ArmyProfession profession)
{
    
}

void Army::armyController(Monsters &monsters)
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
        if(monsters.monstersRegistry[0].hp.size() == 0)
        {
            cornerController(profession);
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
            /*
            auto &entry2 = entry.corners;
            std::cout << "main Index: " << entry.armyMainIndex << "\n"
            << "left top: " << entry2.leftTop << "\n"
            << "right top: " << entry2.rightTop << "\n"
            << "left bot: " << entry2.leftBot << "\n"
            << "right bot: " << entry2.rightBot << "\n"
            <<std::endl;
            */
            continue;
        }
        if(armyRegistry[profession].states == States::combat)
        {

        }
        /* debug
        if(i == 0)
        {
        auto &entry = armyRegistry[profession].corners;
        std::cout << "mainIndex " << armyRegistry[profession].armyMainIndex
        << " width " << armyRegistry[profession].armyShape.currentWidth
        << " || " 
        << " leftTop " << entry.leftTop 
        << " rightTop " << entry.rightTop
        << " leftBot " << entry.leftBot
        << " rightBot " << entry.rightBot
        << " || "
        << " area " << armyRegistry[profession].area
        << std::endl;
        }
        */
    }
}
