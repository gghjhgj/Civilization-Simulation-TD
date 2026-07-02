#include "Army.h"
#include "Civilization.h"
#include "Monsters.h"
#include "World.h"
#include "RendererSFML.h"
#include "CombatSystem.h"

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

        monstersRegistry[i].states = States::idle;
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
            monstersRegistry[i].monstersTotalDMG = Config::normalMonsterDamage * Config::normalMonstersCreated;
        }
        else if(i == 1)
        {
            for(int j = 0; j < Config::giantMonstersCreated; j++)
            {
                renderer.addProfToBuffer(i, RendererSFML::Source::monstersClass, j);
                monstersRegistry[i].addGiant(j);
            }
            monstersRegistry[i].monstersTotalDMG = Config::giantMonsterDamage * Config::giantMonstersCreated;
        }
    }
}

void Monsters::giveMonstersTargetIndex(Army &army, MonstersTypes types)
{
    //y * size + x
    if(monstersRegistry[types].states == States::idle)
    {
    int x = 800;
    int y = 500;
    int start = y * Config::sizeX + x;
    monstersRegistry[types].monstersTargetIndex = start;
    monstersRegistry[types].states = States::moving;
    }
}
Monsters::Dirs Monsters::monstersMoveDecision(MonstersTypes types)
{
    auto &entry = monstersRegistry[types];
    int monstersIndex = entry.monstersMainIndex;
    int targetIndex = entry.monstersTargetIndex;
    if(monstersIndex == targetIndex)
    {
        entry.monstersTargetIndex = -1;
        entry.states = States::waitingForCombat;
        CombatSystem::incrementArmiesReady();
        if(CombatSystem::areArmiesReady())
        {
            CombatSystem::startCombat();
        }
    }

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

void Monsters::monstersMove(Army &army, MonstersTypes types, Dirs dir)
{
    //auto dir = monstersMoveDecision(types);
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
            giveMonstersTargetIndex(army, types);
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

void Monsters::cornerController(MonstersTypes types)
{
    auto &entry = monstersRegistry[types];

    int mainIndex = entry.monstersMainIndex;
    int width = entry.monstersShape.currentWidth;
    int height = (width > 0) ? (entry.hp.size() + width - 1) / width : 0; // 10/3 = 4 not 3
    int spacingX = entry.monstersShape.currentSpacingX;
    int spacingY = entry.monstersShape.currentSpacingY;
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

    areaController(types, realWidth, realHeight);
}

void Monsters::areaController(MonstersTypes types, int realWidth, int realHeight)
{
    monstersRegistry[types].area = realWidth * realHeight;
}

void Monsters::targetProfessionDecission(Army &army, MonstersTypes types)
{
    int maxCoverage = 0;
    Army::ArmyProfession targetProfession = Army::ArmyProfession::COUNT;
    auto &monstersEntry = monstersRegistry[types].corners;
    for(int i = 0; i < Army::ArmyProfession::COUNT; i++)
    {
        auto &armyEntry = army.armyRegistry[i].corners;
        int coverage = CombatSystem::getCoverage(
            monstersEntry.leftTop,
            monstersEntry.rightBot,
            armyEntry.leftTop,
            armyEntry.rightBot
        );
        if(coverage > maxCoverage)
        {
            maxCoverage = coverage;
            targetProfession = Army::ArmyProfession(i);
        }
    }
    if(targetProfession == Army::ArmyProfession::COUNT)
    {
        targetProfession = Army::ArmyProfession::soldier;
    }
    monstersRegistry[types].targetProfession = targetProfession;
}

Monsters::Dirs Monsters::positioningWhileCombat(Army &army, MonstersTypes types)
{
    static constexpr int offsets[4]
    {
            -Config::sizeX,
         -1,               +1,
            +Config::sizeX
    };
    static constexpr Monsters::Dirs dirs[4]
    {
        Monsters::Dirs::up,
        Monsters::Dirs::left,
        Monsters::Dirs::right,
        Monsters::Dirs::down
    };

    int target = monstersRegistry[types].targetProfession;
    auto &monstersEntry = monstersRegistry[types].corners;
    auto &armyEntry = army.armyRegistry[target].corners;
    int maxCoverage = CombatSystem::getCoverage(
            armyEntry.leftTop,
            armyEntry.rightBot,
            monstersEntry.leftTop,
            monstersEntry.rightBot
        );
        //std::cout << "max coverage " << maxCoverage << " ";
    for(int i = 0; i < std::size(offsets); i++)
    {
        int coverage = CombatSystem::getCoverage(
            armyEntry.leftTop,
            armyEntry.rightBot,
            monstersEntry.leftTop + offsets[i],
            monstersEntry.rightBot + offsets[i]
        );
        //std::cout << "check " << i << " " << coverage << " ";
        if(coverage > maxCoverage)
        {
            return dirs[i];
        }
    }

    return Monsters::Dirs::none;
}

void Monsters::monstersController(Army &army)
{
    for(int i = 0; i < MonstersTypes::COUNT; i++)
    {
        MonstersTypes types = MonstersTypes(i);
        auto &entry = monstersRegistry[types];
        //spacingController(types);
        //posSpacing(profession);
        noiseController(types);
        widthController(types);
        posWidth(types);
        if(monstersRegistry[0].hp.size() == 0)
        {
            cornerController(types);
            continue;
        }
        if(monstersRegistry[types].states == Monsters::States::idle)
        {
            //std::cout << "got to idle" << std::endl;
            giveMonstersTargetIndex(army, types);
            continue;
        }
        if(monstersRegistry[types].states == Monsters::States::moving)
        {
            //std::cout << "got to moving" << std::endl;
            auto dir = monstersMoveDecision(types);
            monstersMove(army, types, dir);
            cornerController(types);
            /*
            std::cout << "leftTop: " << entry.corners.leftTop 
            << " rightTop: " << entry.corners.rightTop
            << " leftBot: " << entry.corners.leftBot
            << " rightBot: " << entry.corners.rightBot
            << std::endl;
            */
            continue;
        }
        if(monstersRegistry[types].states == Monsters::States::combat)
        {
            //std::cout << "got to combat" << std::endl;
            targetProfessionDecission(army, types);
            auto dir = positioningWhileCombat(army, types);
            monstersMove(army, types, dir);
            cornerController(types);
            /*
            std::cout
            << "leftTop: " << entry.corners.leftTop 
            << " rightTop: " << entry.corners.rightTop
            << " leftBot: " << entry.corners.leftBot
            << " rightBot: " << entry.corners.rightBot
            << std::endl;
            */
            continue;
        }
        /* debug
        if(i == 0)
        {
        auto &entry = monstersRegistry[types];
        std::cout << "mainIndex " << entry.monstersMainIndex
        << " width " << entry.monstersShape.currentWidth
        << " || " 
        << " leftTop " << entry.corners.leftTop 
        << " rightTop " << entry.corners.rightTop
        << " leftBot " << entry.corners.leftBot
        << " rightBot " << entry.corners.rightBot
        << " || "
        << " area " << entry.area
        << std::endl;
        }
        */
        
    }
}