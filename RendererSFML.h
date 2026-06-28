#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <map>
#include "World.h"
#include "Config.h"

class Army;
class RendererSFML
{
public:
    Army* army;
    float time = 0;
    RendererSFML(int w, int h, int cellSize);

    bool isOpen();
    void begin();
    void render(World& world);
    void addProfToBuffer(int profession, int logicID);
    void reloadArmyBuffer();
    void end();

private:
    struct ArmyLayer 
    {
    std::vector<sf::Vertex> armyVertices;
    sf::VertexBuffer armyLayer{sf::PrimitiveType::Points, sf::VertexBuffer::Usage::Stream};
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