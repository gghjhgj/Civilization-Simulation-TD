#include "world/WorldData/World.h"
#include "entities/HumansData/Human.h"

#include <thread>
#include <atomic>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "Config/Config.h"
#include "renderer/RendererSFML.h"

#include "world/resources/Food.h"
#include "world/resources/Tree.h"
#include "world/resources/Stone.h"

#include "entities/civilization/Civilization.h"
#include "system/ThreadController.hpp"
#include "system/CrashHandler.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

int main()
{
    Config::load("Config/Config.ini");

#ifdef _WIN32
    printCPUTopology();

    if (Config::threads.readDeviceThreadCount)
        configureThreadsAutomatically();

    pinThreadToMask(getSimulationLoopMask());
    printCurrentCPU();
#endif

    std::cout << "START\n";

#ifdef __AVX2__
    std::cout << "AVX2\n";
#else
    std::cout << "No AVX2\n";
#endif

    std::cout << "hardware_concurrency = "
              << std::thread::hardware_concurrency()
              << '\n';

#ifdef _WIN32
    SetPriorityClass(
        GetCurrentProcess(),
        HIGH_PRIORITY_CLASS
    );

    SetThreadPriority(
        GetCurrentThread(),
        THREAD_PRIORITY_TIME_CRITICAL
    );

    SetThreadExecutionState(
        ES_CONTINUOUS |
        ES_SYSTEM_REQUIRED |
        ES_AWAYMODE_REQUIRED
    );
#endif

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    srand(static_cast<unsigned>(time(nullptr)));

    World world;

    Food food;
    Tree tree;
    Stone stone;

    Civilization civilization(food, stone, tree);
    Human human;

    RendererSFML renderer(
        Config::rendering.windowSizeX,
        Config::rendering.windowSizeY,
        1
    );

    world.createLand();
    std::cout << "land created" << std::endl;

    world.createStruct(TerrainType::Desert);
    std::cout << "desert created" << std::endl;

    world.createStruct(TerrainType::Mountain);
    std::cout << "mountain created" << std::endl;

    civilization.createCivilization(world);
    std::cout << "civ created" << std::endl;

    human.createHuman(world, civilization);
    std::cout << "humans created" << std::endl;

    tree.createTree(world, renderer);
    std::cout << "tree created" << std::endl;

    food.createFood(world, renderer);
    std::cout << "food created" << std::endl;

    stone.createStone(world, renderer);
    std::cout << "stone created" << std::endl;

    std::atomic<bool> running = true;

    std::thread humanThread([&]()
    {
        while (running)
        {
            human.humanMove(
                world,
                civilization,
                food,
                tree,
                stone,
                renderer
            );
        }
    });

    sf::Clock clock;

    float fileTimer = 0.f;
    float renderTimer = 0.f;

    int ticksCount = 0;
    int framesCount = 0;
    int humanTicksBefore = 0;

    std::cout << "start sim loop\n";

    while (renderer.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();

        renderer.time += deltaTime;
        fileTimer += deltaTime;
        renderTimer += deltaTime;

        ticksCount++;

        if (world.allTicksCount %
            Config::simulation.ticksForBuildingDecision == 0)
        {
            civilization.buildingDecision(
                world,
                renderer,
                human,
                food,
                stone,
                tree
            );
        }

        if (world.allTicksCount %
            Config::simulation.ticksForResourcesGainsFromBuildings == 0)
        {
            civilization.getBuildingsGains();
        }

        civilization.assignHumansToBuilding(
            human,
            Type::HOUSE
        );

        food.foodRespawn(world, renderer);
        stone.stoneRespawn(world, renderer);
        tree.treeRespawn(world, renderer);

        if (fileTimer >= 1.0f)
        {
            int humanTicks =
                human.humanTicks - humanTicksBefore;

            humanTicksBefore = human.humanTicks;

            world.writeStatsToTxt(
                ticksCount,
                framesCount,
                humanTicks,
                civilization,
                human,
                stone,
                food,
                tree
            );

            fileTimer = 0.f;
            ticksCount = 0;
            framesCount = 0;
        }

        if (renderTimer >= 1.f / Config::rendering.fps)
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