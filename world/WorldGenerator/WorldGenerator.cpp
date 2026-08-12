#include "WorldGenerator.h"

#include "../WorldData/World.h"

#include "../../entities/civilization/Civilization.h"
#include "../../entities/HumansData/Human.h"

#include "../resources/Food.h"
#include "../resources/Tree.h"
#include "../resources/Stone.h"

#include "../../renderer/RendererSFML.h"
#include "../../renderer/LoadingScreen.h"


#include <iostream>


void WorldGenerator::generate(
    World& world,
    Civilization& civilization,
    Human& human,
    Food& food,
    Tree& tree,
    Stone& stone,
    RendererSFML& renderer,
    LoadingScreen& loadingScreen
)
{
    // ============================================================
    // WORLD
    // ============================================================

    loadingScreen.update(
        0.00f,
        "Creating world..."
    );

    std::cout << "Creating world..."
              << std::endl;

    world.createLand();


    // ============================================================
    // DESERT
    // ============================================================

    loadingScreen.update(
        0.10f,
        "Creating desert..."
    );

    std::cout << "Creating desert..."
              << std::endl;

    world.createStruct(
        TerrainType::Desert
    );


    // ============================================================
    // MOUNTAINS
    // ============================================================

    loadingScreen.update(
        0.20f,
        "Creating mountains..."
    );

    std::cout << "Creating mountains..."
              << std::endl;

    world.createStruct(
        TerrainType::Mountain
    );


    // ============================================================
    // CIVILIZATION
    // ============================================================

    loadingScreen.update(
        0.30f,
        "Creating civilization..."
    );

    std::cout << "Creating civilization..."
              << std::endl;

    civilization.createCivilization(
        world
    );


    // ============================================================
    // HUMANS
    // ============================================================

    loadingScreen.update(
        0.45f,
        "Creating humans..."
    );

    std::cout << "Creating humans..."
              << std::endl;

    human.createHuman(
        world,
        civilization
    );


    // ============================================================
    // TREES
    // ============================================================

    loadingScreen.update(
        0.60f,
        "Creating trees..."
    );

    std::cout << "Creating trees..."
              << std::endl;

    tree.createTree(
        world,
        renderer
    );


    // ============================================================
    // FOOD
    // ============================================================

    loadingScreen.update(
        0.73f,
        "Creating food..."
    );

    std::cout << "Creating food..."
              << std::endl;

    food.createFood(
        world,
        renderer
    );


    // ============================================================
    // STONE
    // ============================================================

    loadingScreen.update(
        0.86f,
        "Creating stone..."
    );

    std::cout << "Creating stone..."
              << std::endl;

    stone.createStone(
        world,
        renderer
    );


    // ============================================================
    // FINISHED
    // ============================================================

    loadingScreen.update(
        1.00f,
        "Finished! \nwait a moment..."
    );

    std::cout << "Finished!"
              << std::endl;
}