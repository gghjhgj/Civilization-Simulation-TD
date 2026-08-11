#include "world/WorldData/World.h"

#include "entities/HumansData/Human.h"

#include <tbb/parallel_invoke.h>
#include <thread>
#include "Config/Config.h"
#include "renderer/RendererSFML.h"
#include "world/resources/Food.h"
#include "world/resources/Tree.h"
#include "world/resources/Stone.h"
#include "entities/civilization/Civilization.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "system/ThreadController.hpp"
#include "system/CrashHandler.hpp"
#ifdef _WIN32
#include <windows.h>
#endif

int main()
{
    Config::load("Config/Config.ini");
#ifdef _WIN32
    // SetUnhandledExceptionFilter(crashHandler);
    printCPUTopology();
    pinPhysicalCore(0);
    printCurrentCPU();
#endif

    std::cout << "START" << std::endl;
#ifdef __AVX2__
    std::cout << "AVX2\n";
#else
    std::cout << "No AVX2\n";
#endif
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
    RendererSFML renderer(Config::rendering.windowSizeX, Config::rendering.windowSizeY, 1);

    world.createLand();
    std::cout << "land created" << std::endl;
    world.createStruct(TerrainType::Desert);
    std::cout << "desert created" << std::endl;
    world.createStruct(TerrainType::Mountain);
    std::cout << "mountain created" << std::endl;
    civilization.createCivilization(world);
    std::cout << "civ created" << std::endl;
    human.createHuman(world, civilization);
    tree.createTree(world, renderer);
    std::cout << "tree created" << std::endl;
    food.createFood(world, renderer);
    std::cout << "food created" << std::endl;
    stone.createStone(world, renderer);
    std::cout << "stone created" << std::endl;

    std::thread humanThread;
    std::atomic<bool> running = true;

    humanThread = std::thread([&]()
                              {
        while(running)
        {
            human.humanMove(world, civilization, food, tree, stone, renderer);
        } });

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

        if (world.allTicksCount % Config::simulation.ticksForBuildingDecision == 0)
        {
            civilization.buildingDecision(world, renderer, human, food, stone, tree);
        }
        if (world.allTicksCount % Config::simulation.ticksForResourcesGainsFromBuildings == 0)
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

        if (renderTimer >= (1.f / Config::rendering.fps))
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