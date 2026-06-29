#include "Army.h"
#include "Civilization.h"
#include "Monsters.h"
#include "World.h"
#include "RendererSFML.h"


Monsters* Monsters::instance = nullptr;

Monsters::Monsters() {
    instance = this;
}
/*
void Monsters::monstersResize()
{
    uint32_t size = 100000;
    for(int i = 0; i < MonstersTypes::COUNT; i++)
    {
        monstersRegistry[i].logicID.resize(size);
        monstersRegistry[i].hp.resize(size);
        monstersRegistry[i].damage.resize(size);
    }
}
*/

void Monsters::spawnDecision(Civilization &civ)
{
    /*
    int civSpawnX = civ.spawn % Config::sizeX;
    int civSpawnY = civ.spawn / Config::sizeX;

    int spawnX = 0;
    int spawnY = 0;
    for(int i = 0; i < MonstersTypes::COUNT; i++)
    {
        if(civSpawnX > Config::sizeX/2)
        {
            spawnX = civSpawnX - Config::sizeX/2 + 100 * (i - 1);
        }
        else if(civSpawnX < Config::sizeX/2)
        {
            spawnX = civSpawnX + Config::sizeX/2 - 100 * (i - 1);
        }

        if(civSpawnY > Config::sizeY/2)
        {
            spawnY = civSpawnY - Config::sizeY/2 + 100 * (i - 1);
        }
        else if(civSpawnY < Config::sizeY/2)
        {
            spawnY = civSpawnY + Config::sizeY/2 + 100 * (i - 1);
        }

        monstersRegistry[i].monstersSpawn = spawnY * Config::sizeX + spawnX;
    }
     */
    for(int i = 0; i < MonstersTypes::COUNT; i++)
    {
        int x = Config::sizeX - 200 - (i * 100);
        int y = Config::sizeY - 200;
        monstersRegistry[i].monstersSpawn = y * Config::sizeX + x;
    }
}

void Monsters::waveInit()
{
    for(int i = 0; i < MonstersTypes::COUNT; i++)
    {
        if(i == 0)
        monstersRegistry[i].monstersMainIndex = monstersRegistry[i].monstersSpawn;
        else
        monstersRegistry[i].monstersMainIndex = monstersRegistry[i].monstersSpawn;

        monstersRegistry[i].monstersTargetIndex = -1;
    }
}

void Monsters::monstersCreate(RendererSFML &renderer)
{
    for(int i = 0; i < MonstersTypes::COUNT; i++)
    {
        if(i == 0)
        {
            for(int j = 0; j < Config::normalMonstersCreated; j++)
            {
                renderer.addProfToBuffer(i, RendererSFML::Source::monstersClass, j);
                monstersRegistry[i].addNormalMonster(j);
            }
        }
        else if(i == 1)
        {
            for(int j = 0; j < Config::giantMonstersCreated; j++)
            {
                renderer.addProfToBuffer(i, RendererSFML::Source::monstersClass, j);
                monstersRegistry[i].addGiant(j);
            }
        }
    }
}

void Monsters::giveMonstersTargetIndex(MonstersTypes types)
{
    //y * size + x
    /*
    int x = 800;
    int y = 300 - (profession * 100);
    int start = y * Config::sizeX + x;
    armyRegistry[profession].armyTargetIndex = start;
    */
    int x = 800;
    int y = 600; - (types * 100);
    int start = y * Config::sizeX + x;
    monstersRegistry[types].monstersTargetIndex = start;
}
Monsters::Dirs Monsters::monstersMoveDecision(MonstersTypes types)
{
    int monstersIndex = monstersRegistry[types].monstersMainIndex;
    int targetIndex = monstersRegistry[types].monstersTargetIndex;

    int monstersX = monstersIndex % Config::sizeX;
    int monstersY = monstersIndex / Config::sizeX;

    int targetX = targetIndex % Config::sizeX;
    int targetY = targetIndex / Config::sizeX;
    if(targetIndex != -1)
    {
    if(monstersX > targetX) return left;
    if(monstersX < targetX) return right;
    if(monstersY > targetY) return up;
    if(monstersY < targetY) return down;
    }
    return none;
}

void Monsters::monstersMove(MonstersTypes types)
{
    auto dir = monstersMoveDecision(types);
    switch(dir)
    {
        case left:
        {
            monstersRegistry[types].monstersMainIndex--;
            break;
        }
        case right:
        {
            monstersRegistry[types].monstersMainIndex++;
            break;
        }
        case up:
        {
            monstersRegistry[types].monstersMainIndex -= Config::sizeX;
            break;
        }
        case down:
        {
            monstersRegistry[types].monstersMainIndex += Config::sizeX;
            break;
        }
        case none:
        default:
        {
            giveMonstersTargetIndex(types);
            break;
        }
    }
}

void Monsters::spacingController(MonstersTypes types)
{
    uint8_t currentX = monstersRegistry[types].monstersShape.currentSpacingX;
    uint8_t currentY = monstersRegistry[types].monstersShape.currentSpacingY;

    if(currentX == 2) monstersRegistry[types].monstersShape.targetSpacingX = 2;
    else if(currentX == 2) monstersRegistry[types].monstersShape.targetSpacingX = 2;

    if(currentY == 2) monstersRegistry[types].monstersShape.targetSpacingY = 2;
    if(currentY == 2) monstersRegistry[types].monstersShape.targetSpacingY = 2;
}

void Monsters::posSpacing(MonstersTypes types)
{
    uint8_t currentX = monstersRegistry[types].monstersShape.currentSpacingX;
    uint8_t currentY = monstersRegistry[types].monstersShape.currentSpacingY;

    uint8_t targetX =  monstersRegistry[types].monstersShape.targetSpacingX;
    uint8_t targetY =  monstersRegistry[types].monstersShape.targetSpacingY;

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

    monstersRegistry[types].monstersShape.currentSpacingX = currentX;
    monstersRegistry[types].monstersShape.currentSpacingY = currentY;
}

void Monsters::noiseController(MonstersTypes types)
{
    int maxSpacing = std::min(monstersRegistry[types].monstersShape.currentSpacingX,  monstersRegistry[types].monstersShape.currentSpacingY);
    monstersRegistry[types].monstersShape.currentNoise = maxSpacing/2;
}

void Monsters::widthController(MonstersTypes types)
{
    int current = monstersRegistry[types].monstersShape.currentWidth;
    if(current == 1)
    {
        monstersRegistry[types].monstersShape.currentWidth = sqrt(monstersRegistry[types].hp.size()) * 2;
        return;
    }
    monstersRegistry[types].monstersShape.targetWidth = sqrt(monstersRegistry[types].hp.size()) * 2;
}

void Monsters::posWidth(MonstersTypes types)
{
    int current = monstersRegistry[types].monstersShape.currentWidth;
    int target = monstersRegistry[types].monstersShape.targetWidth;

    if(current < target)
    {
        current++;
    }
    if(current > target)
    {
        current--;
    }
    monstersRegistry[types].monstersShape.currentWidth = current;
}


void Monsters::monstersController(Army &army)
{
    for(int i = 0; i < MonstersTypes::COUNT; i++)
    {
        MonstersTypes types = MonstersTypes(i);
        //spacingController(types);
        //posSpacing(profession);
        noiseController(types);
        widthController(types);
        posWidth(types);
        if(monstersRegistry[0].hp.size() == 0) continue;
        monstersMove(types);
    }
}