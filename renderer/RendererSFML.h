#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdint>

#include <tbb/task_arena.h>

#include <imgui-SFML.h>

#include "../stats/Stats.h"
#include "../stats/StatsUI.h"
#include "../Config/Config.h"
#include "../XY/XY.h"
#include "DirtyCells.h"

class World;
class Human;

class RendererSFML
{
public:
    float time = 0.f;

    RendererSFML(
        int w,
        int h,
        int cellSize);
    ~RendererSFML();

    sf::RenderWindow &getWindow()
    {
        return window;
    }
    
    std::vector<DirtyCells> dirtyCells;

    std::vector<std::vector<DirtyCells>> dirtyBuffers;

    bool isOpen();

    void begin();

    void render(
        World &World,
        const Stats::Data &stats);

    void end();

    template <typename T>
    void renderEntities(
        std::vector<T> &entities,
        World &world)
    {
        for (auto &e : entities)
        {
            if (isCellVisible(
                    e.oldPos.x,
                    e.oldPos.y))
            {
                updateCellPixels(
                    e.oldPos.x,
                    e.oldPos.y,
                    getColor(
                        world,
                        e.oldPos.x,
                        e.oldPos.y));
            }

            if (isCellVisible(
                    e.pos.x,
                    e.pos.y))
            {
                updateCellPixels(
                    e.pos.x,
                    e.pos.y,
                    sf::Color::Black);
            }

            e.oldPos = e.pos;
        }
    }

    void updateHumanLayer(
        Human &human);

    void updateWorldLayer(
        World &world);

    sf::Color getColor(
        World &world,
        uint16_t x,
        uint16_t y);

    void addToDirtyBuffer(
        World &world,
        uint16_t x,
        uint16_t y,
        sf::Color color,
        int threadID);

    void addChunkToDirtyBuffer(
        World &world,
        uint16_t chunkX,
        uint16_t chunkY,
        sf::Color color,
        int threadID);

    void addToDirtyCells(
        World &world,
        uint16_t x,
        uint16_t y,
        sf::Color color);

    void addChunkToDirtyCells(
        World &world,
        uint16_t chunkX,
        uint16_t chunkY,
        sf::Color color);

    void mergeDirtyBuffersToDirtyCells();

    void forceWorldReload();

private:
    sf::RenderWindow window;

    unsigned windowWidth;
    unsigned windowHeight;

    sf::Image pixelImage;

    sf::Texture texture;

    sf::Sprite sprite;

    bool textureDirty = true;

    sf::View view;

    sf::Clock cameraClock;

    float cameraSpeed = 10000.f;

    bool viewChanged = true;

    int viewportLeft = 0;
    int viewportTop = 0;

    int visibleMinX = 0;
    int visibleMaxX = -1;

    int visibleMinY = 0;
    int visibleMaxY = -1;

    struct HumanLayer
    {
        std::vector<sf::Vertex> vertices;

        sf::VertexBuffer buffer{
            sf::PrimitiveType::Points,
            sf::VertexBuffer::Usage::Stream};
    };

    HumanLayer humanLayer;

    int cellSize;

    void handleCameraInput();

    void updateViewport();

    void calculateVisibleCells();

    void clampCameraToWorld();

    void updateSpritePosition();

    void rebuildVisibleWorld(
        World &world);

    void updateCellPixels(
        int x,
        int y,
        sf::Color color);

    bool isCellVisible(
        int x,
        int y) const;

    bool isCellVisible(
        uint16_t x,
        uint16_t y) const;

    bool isDirtyCellVisible(
        const DirtyCells &cell) const;
};