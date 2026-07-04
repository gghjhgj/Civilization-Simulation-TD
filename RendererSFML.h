#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <map>
#include "World.h"
#include "Config.h"

class Army;
class Monsters;
class RendererSFML
{
public:
    Army* army;
    Monsters* monsters;
    float time = 0;
    RendererSFML(int w, int h, int cellSize);

    enum Source
    {
        armyClass,
        monstersClass
    };
    bool isOpen();
    void begin();
    void setArmyColors();
    void render(World& world);
    void addProfToBuffer(int profession, Source source, int logicID);
    void eraseProfFromBuffer(int profession, Source source, int logicID);
    void reloadArmyBuffer();
    void end();

private:
    struct ArmyLayer 
    {
    std::vector<sf::Vertex> armyVertices;
    sf::VertexBuffer armyLayer{sf::PrimitiveType::Points, sf::VertexBuffer::Usage::Stream};
    sf::Color color;
    };
    enum Profs
    {
        soldier,
        archer,
        normalMonster,
        giantMonster,

        COUNT
    };
    std::map<int, ArmyLayer> armyLayers;
    sf::Shader armyShader;
    sf::RenderWindow window;
    sf::Image pixelImage;
    sf::Texture texture;
    sf::Sprite sprite;
    int cellSize;

    void updateCellPixels(int x, int y, sf::Color color);
    sf::Color getColor(const World::Cell& c);
};