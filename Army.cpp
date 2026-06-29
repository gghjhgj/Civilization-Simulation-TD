#include "Army.h"
#include "Civilization.h"
#include "Human.h"
#include "World.h"
#include "RendererSFML.h"
#include "Monsters.h"

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
    int x = 800;
    int y = 400 - (profession * 100);
    int start = y * Config::sizeX + x;
    armyRegistry[profession].armyTargetIndex = start;
}
Army::Dirs Army::armyMoveDecision(ArmyProfession profession)
{
    int armyIndex = armyRegistry[profession].armyMainIndex;
    int targetIndex = armyRegistry[profession].armyTargetIndex;

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



void Army::armyController(Monsters &monsters)
{
    for(int i = 0; i < ArmyProfession::COUNT; i++)
    {
        ArmyProfession profession = ArmyProfession(i);
        //spacingController(profession);
        //posSpacing(profession);
        noiseController(profession);
        widthController(monsters, profession);
        posWidth(profession);
        if(monsters.monstersRegistry[0].hp.size() == 0) continue;
        armyMove(monsters, profession);
    }
}