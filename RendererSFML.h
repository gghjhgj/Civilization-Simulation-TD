#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <map>
#include "Config.h"
#include "XY/XY.h"
#include "DirtyCells.h"
class World;
class Human;

class RendererSFML
{
public:
    float time = 0;
    RendererSFML(int w, int h, int cellSize);

    std::vector<DirtyCells> dirtyCells;
    std::vector<std::vector<DirtyCells>> dirtyBuffers;

    bool isOpen();
    void begin();
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
    void end();

    sf::Color getColor(World& world, uint16_t x, uint16_t y);

    void addToDirtyBuffer(
        World &world, 
        uint16_t x, uint16_t y, 
        sf::Color color, 
        int threadID
    );

    void addChunkToDirtyBuffer(
        World &world, 
        uint16_t chunkX, uint16_t chunkY, 
        sf::Color color, 
        int threadID
    );
    void addToDirtyCells(
        World &world, 
        uint16_t x, uint16_t y, 
        sf::Color color
    );

    void addChunkToDirtyCells(
        World &world, 
        uint16_t chunkX, uint16_t chunkY, 
        sf::Color color
    );

    void mergeDirtyBuffersToDirtyCells();


private:
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