#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <map>
#include "Config.h"
#include "XY/XY.h"
#include "DirtyCells.h"
class World;
class Human;
class Army;
class Monsters;
class RendererSFML
{
public:
    Army* army;
    Monsters* monsters;
    float time = 0;
    RendererSFML(int w, int h, int cellSize);
    std::vector<DirtyCells> dirtyCells;
    enum Source
    {
        armyClass,
        monstersClass
    };
    bool isOpen();
    void begin();
    void setArmyColors();
    template <typename T>
    void renderEntities(std::vector<T>& entities, World& world)
    {
        for (auto& e : entities)
        {
            updateCellPixels(e.oldPos.x, e.oldPos.y, getColor(world, e.oldPos.x, e.oldPos.y));
            updateCellPixels(e.pos.x, e.pos.y, sf::Color::Black);
            e.oldPos = e.pos;
        }
    }
    void updateHumanLayer(Human &human);
    void updateWorldLayer();
    void render(World& world, Human& human);
    void addProfToBuffer(int profession, Source source, int logicID);
    void eraseProfFromBuffer(int profession, Source source, int logicID);
    void reloadArmyBuffer();
    void end();

    sf::Color getColor(World& world, uint32_t x, uint32_t y);
    void addToDirtyCells(World &world, uint32_t x, uint32_t y, sf::Color color);
    void addChunkToDirtyCells(World &world, uint32_t chunkX, uint32_t chunkY, sf::Color color);

private:
    struct ArmyLayer
    {
        std::vector<sf::Vertex> armyVertices;
        sf::VertexBuffer armyLayer{ sf::PrimitiveType::Points, sf::VertexBuffer::Usage::Stream };
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
    struct HumanLayer
    {
        std::vector<sf::Vertex> vertices;
        sf::VertexBuffer buffer{
            sf::PrimitiveType::Points,
            sf::VertexBuffer::Usage::Stream
        };
    };
    HumanLayer humanLayer;


    int cellSize;

    void updateCellPixels(int x, int y, sf::Color color);
};