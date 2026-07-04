#include "World.h"
#include "Config.h"
#include "RendererSFML.h"
#include "Food.h"
#include "Tree.h"
#include "Stone.h"
#include "Civilization.h"
#include "Human.h"
#include "Streets.h"
#include "Walls.h"
#include "Army.h"
#include "Monsters.h"
#include "CombatSystem.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h>

int main() {
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    SetProcessAffinityMask(GetCurrentProcess(), 0x00000001);
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    srand(time(NULL));
    World world;
    Food food;
    Tree tree;
    Stone stone;
    Civilization civilization;
    Human human;
    Streets streets;
    Walls walls;
    Army army;
    Monsters monsters;
    CombatSystem combatSystem;
    RendererSFML renderer(Config::WindowSizeX, Config::WindowSizeY, 1);

    world.init();
    world.createOcean();
    world.createLand();
    world.createStruct(World::CellFlags::Water);
    world.createStruct(World::CellFlags::Sand);
    world.createStruct(World::CellFlags::Mountain);
    world.addSandToLand();
    world.smoothShores();
    world.surfaceVectorsInit();
    civilization.createCivilization(world);
    human.createHuman(world, civilization);
    tree.createTree(world);
    food.createFood(world);
    stone.createStone(world);
    world.markAllDirty();
    army.armyInit();
    //civilization.makeEverybodyTreeWorker(human);
    //streets.addStreet(world, civilization, human, 8765, 743986);

    sf::Clock clock;

float fileTimer = 0.f;
float renderTimer = 0.f;

int ticksCount = 0;
int framesCount = 0; 
while (renderer.isOpen())
{
    float deltaTime = clock.restart().asSeconds();
    renderer.time += deltaTime;

    fileTimer += deltaTime;
    renderTimer += deltaTime;

    ticksCount++;
   
    if(world.allTicksCount % Config::ticksForCivilizationDecision == 0)
    {
        civilization.civilizationDecision(human, food, stone, tree);
    }

    if(world.allTicksCount % Config::ticksForBuildingDecision == 0)
    {
        civilization.buildingDecision(world, human, food, stone, tree);
    }

    if(world.allTicksCount % Config::ticksForAssigningDecision == 0)
    {
        civilization.assignHumansToFarm(human);
        civilization.assignHumansToSawmills(human);
        civilization.assignHumansToMine(human);
    }

    if(world.allTicksCount % Config::ticksForResourcesGainsFromBuildings == 0)
    {
        civilization.farmsGains();
        civilization.sawmillsGains();
        civilization.minesGains();
    }
    if(world.allTicksCount % Config::ticksForNewHumans == 0) //&& (spawnArmy || army.armyRegistry[Army::ArmyProfession::soldier].index.size() % Config::countOfTroopsInOneLine != 0))
    {
        human.humanRespawn(world, civilization);
    }
    if(world.allTicksCount > 0 && world.allTicksCount % Config::ticksForBuildingWall == 0)
    {
        if(monsters.monstersRegistry[0].monstersCount == 0)
        {
        monsters.spawnDecision(civilization);
        monsters.waveInit();
        monsters.monstersCreate(renderer);
        }
        walls.buildWalls(world, civilization, Walls::WallsTypes::woodenWall);
    }
    if(world.allTicksCount > 0 && world.allTicksCount % Config::ticksForAddingHumansToArmy == 0 && combatSystem.armiesReadyForCombat != 4) //&& army.armyRegistry[Army::ArmyProfession::soldier].index.size() < 5000 && (spawnArmy || army.armyRegistry[Army::ArmyProfession::soldier].index.size() % Config::countOfTroopsInOneLine != 0))
    {
        army.addHumansToArmy(world, human, civilization, renderer, Army::ArmyProfession::soldier);
    }
    civilization.assignHumansToHouse(human);
    food.foodRespawn(world);
    stone.stoneRespawn(world);
    tree.treeRespawn(world);
    human.humanMove(world, civilization, streets, food, tree, stone, human);
    army.armyController(monsters, renderer);
    monsters.monstersController(army, renderer);
    /*
    for(int i = 0; i < Army::ArmyProfession::COUNT; i++)
    {
        Army::ArmyProfession profession = Army::ArmyProfession(i);
        army.armyMove(profession);
    }
    */
    if (fileTimer >= 1.0f)
    {
        world.writeStatsToTxt(ticksCount, framesCount, civilization, human, stone, food, tree, army, monsters);
        fileTimer = 0.f;
        ticksCount = 0;
        framesCount = 0;
    }


    if (renderTimer >= (1.f / Config::FPS))
    {
        renderTimer = 0.f;

        world.makeAllHumansDirty(human);

    int mainIndex = army.armyRegistry[0].armyMainIndex;
    int leaderX = (mainIndex % Config::sizeX) * 1;
    int leaderY = (mainIndex / Config::sizeX) * 1;

        renderer.begin();
        renderer.render(world);
        renderer.end();

        framesCount++;

        world.makeAllHumansDirty(human);
    }
    /*
    if(world.allTicksCount > Config::ticksForBuildingWall && combatSystem.armiesReadyForCombat == 4)
    {
        Sleep(1);
    }
    */
    world.allTicksCount++;
}

return 0;
}

/*
to do

*/

/*    run
g++ *.cpp -o app -lsfml-graphics -lsfml-window -lsfml-system && app.exe
*/

/*    debug
g++ -g *.cpp -o app -lsfml-graphics -lsfml-window -lsfml-system && gdb app
*/

/*
g++ -Ofast -march=native -flto -funroll-loops -ffast-math *.cpp -o app -lsfml-graphics -lsfml-window -lsfml-system -lmimalloc && app.exe
*/

/*
g++ -Ofast -march=native -flto -fgraphite-identity -floop-nest-optimize -fprofile-generate *.cpp -o app -lsfml-graphics -lsfml-window -lsfml-system -lmimalloc && app.exe
*/
/*
g++ -Ofast -march=native -flto -fgraphite-identity -floop-nest-optimize -fprofile-use *.cpp -o app -lsfml-graphics -lsfml-window -lsfml-system -lmimalloc && app.exe
*/