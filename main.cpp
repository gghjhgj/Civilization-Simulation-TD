#include "world/WorldData/World.h"
#include "world/WorldGenerator/WorldGenerator.h"
#include "entities/HumansData/Human.h"

#include <thread>
#include <atomic>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "Config/Config.h"
#include "renderer/RendererSFML.h"
#include "renderer/LoadingScreen.h"
#include "renderer/LoseScreen.h"

#include "world/resources/Food.h"
#include "world/resources/Tree.h"
#include "world/resources/Stone.h"

#include "entities/civilization/Civilization.h"
#include "system/ThreadController.hpp"
#include "system/CrashHandler.hpp"

#include "stats/Stats.h"

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
        HIGH_PRIORITY_CLASS);

    SetThreadPriority(
        GetCurrentThread(),
        THREAD_PRIORITY_TIME_CRITICAL);

    SetThreadExecutionState(
        ES_CONTINUOUS |
        ES_SYSTEM_REQUIRED |
        ES_AWAYMODE_REQUIRED);
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
        1);
    LoadingScreen loadingScreen(
        renderer.getWindow(),
        Config::rendering.windowSizeX,
        Config::rendering.windowSizeY);

    LoseScreen loseScreen(
        renderer.getWindow(),
        Config::rendering.windowSizeX,
        Config::rendering.windowSizeY);

    Stats stats;

    WorldGenerator::generate(world, civilization, human, food, tree, stone, renderer, loadingScreen);

    std::atomic<bool>
        running = true;

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
        } });

    sf::Clock clock;
    sf::Clock screenReloadClock;

    float fileTimer = 0.f;
    float renderTimer = 0.f;

    int allTicksCount = 0;
    int ticksCount = 0;
    int framesCount = 0;
    int humanTicksBefore = 0;

    std::cout << "start sim loop" << std::endl;
    ;

    while (renderer.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();

        renderer.time += deltaTime;
        fileTimer += deltaTime;
        renderTimer += deltaTime;

        ticksCount++;

        if (screenReloadClock.getElapsedTime().asSeconds() >= 5.f)
        {
            renderer.forceWorldReload();
            screenReloadClock.restart();
        }

        if (allTicksCount >= Config::hunger.firstTickHumansEat &&
            allTicksCount % Config::hunger.firstTickHumansEat == 0)
        {
            civilization.updateHunger(human);

            if (civilization.civilizationChancesLeft <= 0)
            {
                loseScreen.show();
                break;
            }
        }
        if (allTicksCount %
                Config::simulation.ticksForBuildingDecision ==
            0)
        {
            civilization.buildingDecision(
                world,
                renderer,
                human,
                food,
                stone,
                tree);
        }

        if (allTicksCount %
                Config::simulation.ticksForResourcesGainsFromBuildings ==
            0)
        {
            civilization.getBuildingsGains();
        }

        civilization.assignHumansToBuilding(
            human,
            Type::HOUSE);

        food.foodRespawn(world, renderer);
        stone.stoneRespawn(world, renderer);
        tree.treeRespawn(world, renderer);

        if (fileTimer >= 1.0f)
        {
            int humanTicks =
                human.humanTicks - humanTicksBefore;

            humanTicksBefore = human.humanTicks;

            stats.update(
                ticksCount,
                framesCount,
                humanTicks,
                civilization,
                human,
                stone,
                food,
                tree,
                allTicksCount);
            stats.writeToTxt();

            fileTimer = 0.f;
            ticksCount = 0;
            framesCount = 0;
        }

        if (renderTimer >= 1.f / Config::rendering.fps)
        {
            renderTimer = 0.f;

            renderer.begin();
            renderer.render(world, human, stats.get());
            renderer.end();

            framesCount++;
        }

        allTicksCount++;
    }

    running = false;

    if (humanThread.joinable())
        humanThread.join();

    return 0;
}