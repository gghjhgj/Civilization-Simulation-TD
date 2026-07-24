#include "WorldData/World.h"

#include "HumansData/Human.h"

#include <tbb/parallel_invoke.h>
#include <thread>
#include "Config.h"
#include "RendererSFML.h"
#include "Food.h"
#include "Tree.h"
#include "Stone.h"
#include "Civilization.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ThreadController.hpp"
#ifdef _WIN32
#include <windows.h>
#endif


int main() {
    #ifdef _WIN32
    printCPUTopology();
    pinPhysicalCore(0);
    printCurrentCPU();
    #endif

    std::cout << "START" << std::endl;
    std::cout << "threads " << std::thread::hardware_concurrency() << std::endl;

    #ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);
    #endif

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    srand(time(NULL));

    World world;

    Food food;
    Tree tree;
    Stone stone;
    Civilization civilization(food, stone, tree);
    std::cout << "threads before initing threadpool" << std::thread::hardware_concurrency() << std::endl;
    Human human;
    RendererSFML renderer(Config::WindowSizeX, Config::WindowSizeY, 1);
    
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
    tree.createTree(world, renderer); std::cout << "tree created" << std::endl;
    food.createFood(world, renderer); std::cout << "food created" << std::endl;
    stone.createStone(world, renderer); std::cout << "stone created" << std::endl;
    world.markAllDirty(renderer); std::cout << "marked all dirty" << std::endl;


    std::thread humanThread;
    std::atomic<bool> running = true;

    humanThread = std::thread([&]()
    {
        while(running)
        {
            human.humanMove(world, civilization, food, tree, stone, renderer);
        }
    }
    );

    sf::Clock clock;

float fileTimer = 0.f;
float renderTimer = 0.f;

int ticksCount = 0;
int framesCount = 0; 
std::cout << "start sim loop" << std::endl;
int humanTicksBefore = 0;
while (renderer.isOpen())
{
    float deltaTime = clock.restart().asSeconds();
    renderer.time += deltaTime;

    fileTimer += deltaTime;
    renderTimer += deltaTime;

    ticksCount++;

    if(world.allTicksCount % Config::ticksForBuildingDecision == 0)
    {
        civilization.buildingDecision(world, renderer, human, food, stone, tree);
    }
    if(world.allTicksCount % Config::ticksForResourcesGainsFromBuildings == 0)
    {
        civilization.getBuildingsGains(); 
    }
   
    civilization.assignHumansToBuilding(human, Type::HOUSE);
    food.foodRespawn(world, renderer); 
    stone.stoneRespawn(world, renderer);
    tree.treeRespawn(world, renderer);
    if (fileTimer >= 1.0f)
    {
        int humanTicks = human.humanTicks - humanTicksBefore;
        humanTicksBefore = human.humanTicks;
        world.writeStatsToTxt(ticksCount, framesCount, humanTicks, civilization, human, stone, food, tree);
        fileTimer = 0.f;
        ticksCount = 0;
        framesCount = 0;
    }


    if (renderTimer >= (1.f / Config::FPS))
    {
        renderTimer = 0.f;

        renderer.begin(); 
        renderer.render(world, human);
        renderer.end();

    
        framesCount++;
    }
    world.allTicksCount++;
}
running = false;

if (humanThread.joinable())
    humanThread.join();
return 0;
}