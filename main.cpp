#include <glad/glad.h>

#include "WorldData/World.h"
#include "WorldData/WorldGPU.h"

#include "HumansData/Human.h"


#include "Config.h"
#include "RendererSFML.h"
#include "Food.h"
#include "Tree.h"
#include "Stone.h"
#include "Civilization.h"
#include "Army.h"
#include "Monsters.h"
#include "CombatSystem.h"

#include <iostream>
#include <cstdlib>
#include <ctime>

#ifdef _WIN32
#include <windows.h>
#endif


int main() {
    std::cout << "START" << std::endl;
    #ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    SetProcessAffinityMask(GetCurrentProcess(), 0x00000001);
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);
    #endif
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    srand(time(NULL));

    World world;
    WorldGPU worldGPU;

    Food food;
    Tree tree;
    Stone stone;
    Civilization civilization;
    Human human;
    //Walls walls;
    Army army;
    Monsters monsters;
    CombatSystem combatSystem;
    RendererSFML renderer(Config::WindowSizeX, Config::WindowSizeY, 1);
    if(!gladLoadGL())
    {
        std::cout << "GLAD juz nie" << std::endl;
    }
    else
    {
        std::cout << "GLAD dziala" << std::endl;
    }
    world.init(); std::cout << "finisted wordl init" << std::endl;
    world.createOcean(); std::cout << "ocean created" << std::endl;
    world.createLand(); std::cout << "land created" << std::endl;
    world.createStruct(TerrainType::Water); std::cout << "water created" << std::endl;
    world.createStruct(TerrainType::Desert); std::cout << "desert created" << std::endl;
    world.createStruct(TerrainType::Mountain); std::cout << "mountain created" << std::endl;
    world.addSandToLand(); std::cout << "sand added to land" << std::endl;
    world.smoothShores(); std::cout << "shores smoothed" << std::endl;
    civilization.createCivilization(world); std::cout << "civ created" << std::endl;
    human.createHuman(world, civilization);
    tree.createTree(world); std::cout << "tree created" << std::endl;
    food.createFood(world); std::cout << "food created" << std::endl;
    stone.createStone(world); std::cout << "stone created" << std::endl;
    world.markAllDirty(); std::cout << "marked all dirty" << std::endl;
    army.armyInit(); std::cout << "army inited" << std::endl;

    
    //worldGPU.init(world.grid);
    //worldGPU.runShader();
    //worldGPU.downloadData(world.grid);
    //worldGPU.printDebugData();
    

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
        civilization.assignHumansToBuilding(human, Type::FARM);
        civilization.assignHumansToBuilding(human, Type::SAWMILL);
        civilization.assignHumansToBuilding(human, Type::MINE);
    }
    if(world.allTicksCount % Config::ticksForResourcesGainsFromBuildings == 0)
    {
        civilization.getBuildingsGains(); 
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
        //walls.buildWalls(world, civilization, Walls::WallsTypes::woodenWall);
    }
    if(world.allTicksCount > 0 && world.allTicksCount % Config::ticksForAddingHumansToArmy == 0 && combatSystem.armiesReadyForCombat != 4) //&& army.armyRegistry[Army::ArmyProfession::soldier].index.size() < 5000 && (spawnArmy || army.armyRegistry[Army::ArmyProfession::soldier].index.size() % Config::countOfTroopsInOneLine != 0))
    {
        army.addHumansToArmy(world, human, civilization, renderer, Army::ArmyProfession::soldier);
    }
    civilization.assignHumansToBuilding(human, Type::HOUSE);
    food.foodRespawn(world); 
    stone.stoneRespawn(world);
    tree.treeRespawn(world);
    human.humanMove(world, civilization, food, tree, stone, human);
    army.armyController(monsters, renderer);
    monsters.monstersController(army, renderer);
    for(int i = 0; i < Army::ArmyProfession::COUNT; i++)
    {
        Army::ArmyProfession profession = Army::ArmyProfession(i);
        army.armyMove(monsters, profession);
    }
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

    int mainIndex = army.armyRegistry[0].armyMainIndex;
    int leaderX = (mainIndex % Config::sizeX) * 1;
    int leaderY = (mainIndex / Config::sizeX) * 1;

        renderer.begin(); 
        renderer.render(world, human);
        renderer.end();

        framesCount++;
    }
    /*
    if(world.allTicksCount > Config::ticksForBuildingWall && combatSystem.armiesReadyForCombat == 4)
    {
        ifdef _WIN32
            Sleep(1);
        endif
    }
    */
    world.allTicksCount++;
}
return 0;
}
/*
generate prof
g++ -Ofast -march=native -flto -fgraphite-identity -floop-nest-optimize -fprofile-generate -I. *.cpp glad.c -o app -lsfml-graphics -lsfml-window -lsfml-system -lmimalloc && app.exe

use prof
g++ -Ofast -march=native -flto -fgraphite-identity -floop-nest-optimize -fprofile-use -I. *.cpp glad.c -o app -lsfml-graphics -lsfml-window -lsfml-system -lmimalloc && app.exe
*/


/* do profilownia jedyne co nie wywala profilera
clang++ -O2 -march=native -fno-omit-frame-pointer -Wno-c++11-narrowing -g -gcodeview -fuse-ld=lld -Wl,-pdb=app.pdb -I. ./Army.cpp ./Civilization.cpp ./CombatSystem.cpp ./Food.cpp ./HumansData/Human.cpp ./HumansData/HumanGPU.cpp ./main.cpp ./Monsters.cpp ./notUsed/Cars.cpp ./notUsed/Streets.cpp ./RendererSFML.cpp ./Stone.cpp ./Tree.cpp ./Walls.cpp ./WorldData/World.cpp ./WorldData/WorldGPU.cpp glad.c -o app.exe -lsfml-graphics -lsfml-window -lsfml-system
*/