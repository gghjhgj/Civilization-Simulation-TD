#pragma once

class World;
class Civilization;
class Human;
class Food;
class Tree;
class Stone;
class RendererSFML;
class LoadingScreen;


class WorldGenerator
{
public:

    static void generate(
        World& world,
        Civilization& civilization,
        Human& human,
        Food& food,
        Tree& tree,
        Stone& stone,
        RendererSFML& renderer,
        LoadingScreen& loadingScreen
    );
};