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
    loadingScreen.update(
        0.00f,
        "Creating world..."
    );

    std::cout << "Creating world..."
              << std::endl;

    world.createLand();

    loadingScreen.update(
        0.10f,
        "Creating desert..."
    );

    std::cout << "Creating desert..."
              << std::endl;

    world.createStruct(
        TerrainType::Desert
    );

    loadingScreen.update(
        0.20f,
        "Creating mountains..."
    );

    std::cout << "Creating mountains..."
              << std::endl;

    world.createStruct(
        TerrainType::Mountain
    );

    loadingScreen.update(
        0.30f,
        "Creating civilization..."
    );

    std::cout << "Creating civilization..."
              << std::endl;

    civilization.createCivilization(
        world
    );

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

    loadingScreen.update(
        1.00f,
        "Finished! \nwait a moment and don't close the window \nit works I promise"
    );

    std::cout << "Finished!"
              << std::endl;
}