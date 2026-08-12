#include "RendererSFML.h"

#include "../world/WorldData/World.h"
#include "../entities/HumansData/Human.h"

RendererSFML::RendererSFML(
    int w,
    int h,
    int cellSize)
    : window(
          sf::VideoMode({static_cast<unsigned>(
                             w * cellSize),

                         static_cast<unsigned>(
                             h * cellSize)}),
          "Simulation"),

      windowWidth(
          static_cast<unsigned>(
              w * cellSize)),

      windowHeight(
          static_cast<unsigned>(
              h * cellSize)),

      cellSize(cellSize),

      pixelImage(
          {static_cast<unsigned>(
               w * cellSize),

           static_cast<unsigned>(
               h * cellSize)},
          sf::Color::Black),

      texture(
          {static_cast<unsigned>(
               w * cellSize),

           static_cast<unsigned>(
               h * cellSize)}),

      sprite(texture)
{
    const float viewWidth =
        static_cast<float>(
            windowWidth);

    const float viewHeight =
        static_cast<float>(
            windowHeight);

    view = sf::View(
        {viewWidth / 2.f,
         viewHeight / 2.f},
        {viewWidth,
         viewHeight});

    window.setView(view);

    window.setKeyRepeatEnabled(false);

    updateViewport();

    dirtyCells.reserve(10000000);

    const int threads =
        tbb::this_task_arena::max_concurrency();

    dirtyBuffers.resize(threads);

    for (auto &buffer : dirtyBuffers)
    {
        buffer.reserve(1000000);
    }

    humanLayer.vertices.reserve(Config::humans.count + 500000);

    humanLayer.buffer.create(Config::humans.count + 500000);

    updateSpritePosition();

    ImGui::SFML::Init(window);
}
RendererSFML::~RendererSFML()
{
    ImGui::SFML::Shutdown();
}
bool RendererSFML::isOpen()
{
    return window.isOpen();
}

void RendererSFML::handleCameraInput()
{
    while (auto event = window.pollEvent())
    {
        ImGui::SFML::ProcessEvent(
            window,
            *event);

        if (event->is<sf::Event::Closed>())
        {
            window.close();
            continue;
        }

        if (const auto *resized =
                event->getIf<sf::Event::Resized>())
        {
            windowWidth = resized->size.x;
            windowHeight = resized->size.y;

            sf::Vector2f newSize(
                static_cast<float>(windowWidth),
                static_cast<float>(windowHeight));

            view.setSize(newSize);

            window.setView(view);

            pixelImage = sf::Image(
                {windowWidth, windowHeight},
                sf::Color::Black);

            texture = sf::Texture(
                {windowWidth, windowHeight});

            sprite = sf::Sprite(texture);

            viewChanged = true;
            textureDirty = true;

            updateViewport();
        }
    }

    const float dt =
        cameraClock.restart().asSeconds();

    sf::Vector2f movement{0.f, 0.f};

    if (sf::Keyboard::isKeyPressed(
            sf::Keyboard::Key::W))
    {
        movement.y -= cameraSpeed * dt;
    }

    if (sf::Keyboard::isKeyPressed(
            sf::Keyboard::Key::S))
    {
        movement.y += cameraSpeed * dt;
    }

    if (sf::Keyboard::isKeyPressed(
            sf::Keyboard::Key::A))
    {
        movement.x -= cameraSpeed * dt;
    }

    if (sf::Keyboard::isKeyPressed(
            sf::Keyboard::Key::D))
    {
        movement.x += cameraSpeed * dt;
    }

    if (movement.x != 0.f ||
        movement.y != 0.f)
    {
        view.move(movement);

        clampCameraToWorld();

        viewChanged = true;

        window.setView(view);
    }
}

void RendererSFML::begin()
{
    handleCameraInput();

    if (viewChanged)
    {
        updateViewport();
    }

    const float dt =
        cameraClock.getElapsedTime().asSeconds();

    ImGui::SFML::Update(
        window,
        sf::seconds(dt));

    cameraClock.restart();
}

void RendererSFML::updateViewport()
{
    clampCameraToWorld();

    const sf::Vector2f center =
        view.getCenter();

    const sf::Vector2f size =
        view.getSize();

    viewportLeft =
        static_cast<int>(
            std::floor(
                center.x -
                size.x / 2.f));

    viewportTop =
        static_cast<int>(
            std::floor(
                center.y -
                size.y / 2.f));

    calculateVisibleCells();

    updateSpritePosition();

    textureDirty = true;
}

void RendererSFML::calculateVisibleCells()
{
    const int right =
        viewportLeft +
        static_cast<int>(
            windowWidth);

    const int bottom =
        viewportTop +
        static_cast<int>(
            windowHeight);

    visibleMinX =
        static_cast<int>(
            std::floor(
                static_cast<float>(
                    viewportLeft) /
                static_cast<float>(
                    cellSize)));

    visibleMinY =
        static_cast<int>(
            std::floor(
                static_cast<float>(
                    viewportTop) /
                static_cast<float>(
                    cellSize)));

    visibleMaxX =
        static_cast<int>(
            std::ceil(
                static_cast<float>(
                    right) /
                static_cast<float>(
                    cellSize))) -
        1;

    visibleMaxY =
        static_cast<int>(
            std::ceil(
                static_cast<float>(
                    bottom) /
                static_cast<float>(
                    cellSize))) -
        1;
}

void RendererSFML::clampCameraToWorld()
{
    const float worldWidth =
        static_cast<float>(ConfigConstexpr::sizeX * cellSize);

    const float worldHeight =
        static_cast<float>(ConfigConstexpr::sizeY * cellSize);

    const sf::Vector2f size =
        view.getSize();

    sf::Vector2f center =
        view.getCenter();

    const float halfWidth =
        size.x / 2.f;

    const float halfHeight =
        size.y / 2.f;

    if (size.x <= worldWidth)
    {
        center.x =
            std::clamp(
                center.x,
                halfWidth,
                worldWidth - halfWidth);
    }
    else
    {
        center.x =
            worldWidth / 2.f;
    }

    if (size.y <= worldHeight)
    {
        center.y =
            std::clamp(
                center.y,
                halfHeight,
                worldHeight - halfHeight);
    }
    else
    {
        center.y =
            worldHeight / 2.f;
    }

    view.setCenter(center);
}

void RendererSFML::updateSpritePosition()
{
    sprite.setPosition(
        {static_cast<float>(
             viewportLeft),

         static_cast<float>(
             viewportTop)});
}

bool RendererSFML::isCellVisible(
    int x,
    int y) const
{
    return x >= visibleMinX &&
           x <= visibleMaxX &&
           y >= visibleMinY &&
           y <= visibleMaxY;
}

bool RendererSFML::isCellVisible(
    uint16_t x,
    uint16_t y) const
{
    return isCellVisible(
        static_cast<int>(x),
        static_cast<int>(y));
}

void RendererSFML::updateCellPixels(
    int x,
    int y,
    sf::Color color)
{
    const int worldPixelX =
        x * cellSize;

    const int worldPixelY =
        y * cellSize;

    const int localStartX =
        worldPixelX -
        viewportLeft;

    const int localStartY =
        worldPixelY -
        viewportTop;

    const int beginX =
        std::max(
            0,
            localStartX);

    const int beginY =
        std::max(
            0,
            localStartY);

    const int endX =
        std::min(
            static_cast<int>(
                windowWidth),

            localStartX +
                cellSize);

    const int endY =
        std::min(
            static_cast<int>(
                windowHeight),

            localStartY +
                cellSize);

    if (beginX >= endX ||
        beginY >= endY)
    {
        return;
    }

    for (int py = beginY;
         py < endY;
         ++py)
    {
        for (int px = beginX;
             px < endX;
             ++px)
        {
            pixelImage.setPixel(
                {static_cast<unsigned>(
                     px),

                 static_cast<unsigned>(
                     py)},

                color);
        }
    }

    textureDirty = true;
}

sf::Color RendererSFML::getColor(
    World &world,
    uint16_t x,
    uint16_t y)
{
    auto ref =
        world.getCellRef(
            x,
            y);

    BuildingType building =
        world.getBuilding(
            ref.chunkX,
            ref.chunkY);

    if (building != BuildingType::None)
    {
        if (world.hasChunkFlag(
                ref.chunkX,
                ref.chunkY,
                ChunkFlag::Construction))
        {
            return sf::Color(
                255,
                128,
                0);
        }

        switch (building)
        {
        case BuildingType::House:

            return sf::Color::Red;

        case BuildingType::Farm:

            return sf::Color(
                255,
                255,
                150);

        case BuildingType::Sawmill:

            return sf::Color(
                165,
                42,
                42);

        case BuildingType::Mine:

            return sf::Color(
                191,
                0,
                255);

        default:

            break;
        }
    }

    switch (world.getCell(x, y))
    {
    case TerrainType::Land:

        return sf::Color::Green;

    case TerrainType::Desert:

        return sf::Color::Yellow;

    case TerrainType::Mountain:

        return sf::Color(
            120,
            120,
            120);

    case TerrainType::LandWithFood:

        return sf::Color(
            255,
            165,
            0);

    case TerrainType::LandWithTree:

        return sf::Color(
            0,
            120,
            0);

    default:

        return sf::Color(
            200,
            255,
            200);
    }
}

void RendererSFML::rebuildVisibleWorld(
    World &world)
{
    pixelImage = sf::Image(
        {windowWidth,
         windowHeight},
        sf::Color::Black);

    for (int y = visibleMinY;
         y <= visibleMaxY;
         ++y)
    {
        if (y < 0)
        {
            continue;
        }

        for (int x = visibleMinX;
             x <= visibleMaxX;
             ++x)
        {
            if (x < 0)
            {
                continue;
            }

            const auto ux =
                static_cast<uint16_t>(x);

            const auto uy =
                static_cast<uint16_t>(y);

            if (!world.isValid(
                    ux,
                    uy))
            {
                continue;
            }

            updateCellPixels(
                x,
                y,
                getColor(
                    world,
                    ux,
                    uy));
        }
    }

    textureDirty = true;
}

void RendererSFML::updateWorldLayer(
    World &world)
{
    if (viewChanged)
    {
        rebuildVisibleWorld(world);

        viewChanged = false;

        dirtyCells.clear();

        for (auto &buffer : dirtyBuffers)
        {
            buffer.clear();
        }
    }

    mergeDirtyBuffersToDirtyCells();

    for (auto &cell : dirtyCells)
    {
        if (!isDirtyCellVisible(cell))
        {
            continue;
        }

        updateCellPixels(
            cell.coords.x,
            cell.coords.y,
            cell.color);
    }

    dirtyCells.clear();
}

bool RendererSFML::isDirtyCellVisible(
    const DirtyCells &cell) const
{
    return isCellVisible(
        cell.coords.x,
        cell.coords.y);
}

void RendererSFML::updateHumanLayer(
    Human &human)
{
    humanLayer.vertices.clear();

    auto add =
        [&](auto &humans, sf::Color color)
    {
        for (size_t i = 0;
             i < humans.posX.size();
             ++i)
        {
            const uint16_t x =
                humans.posX[i];

            const uint16_t y =
                humans.posY[i];

            if (!isCellVisible(
                    x,
                    y))
            {
                continue;
            }

            sf::Vertex vertex;

            vertex.position =
                {
                    static_cast<float>(
                        x * cellSize +
                        cellSize / 2),

                    static_cast<float>(
                        y * cellSize +
                        cellSize / 2)};

            vertex.color = color;

            humanLayer.vertices.push_back(
                vertex);
        }
    };

    add(
        human.foodCollectors,
        sf::Color(
            255,
            0,
            0));

    add(
        human.woodCollectors,
        sf::Color(
            139,
            69,
            19));

    add(
        human.stoneCollectors,
        sf::Color(
            0,
            100,
            255));

    add(
        human.builders,
        sf::Color(
            255,
            128,
            0));

    add(
        human.assigned,
        sf::Color(
            160,
            32,
            240));

    if (!humanLayer.vertices.empty())
    {
        humanLayer.buffer.update(
            humanLayer.vertices.data(),
            humanLayer.vertices.size(),
            0);
    }
}

void RendererSFML::mergeDirtyBuffersToDirtyCells()
{
    size_t total =
        dirtyCells.size();

    for (auto &buffer : dirtyBuffers)
    {
        total += buffer.size();
    }

    dirtyCells.reserve(total);

    for (auto &buffer : dirtyBuffers)
    {
        if (buffer.empty())
        {
            continue;
        }

        dirtyCells.insert(
            dirtyCells.end(),
            buffer.begin(),
            buffer.end());

        buffer.clear();
    }
}

void RendererSFML::addToDirtyBuffer(
    World &world,
    uint16_t x,
    uint16_t y,
    sf::Color color,
    int threadID)
{
    dirtyBuffers[threadID]
        .emplace_back(
            x,
            y,
            color);
}

void RendererSFML::addChunkToDirtyBuffer(
    World &world,
    uint16_t chunkX,
    uint16_t chunkY,
    sf::Color color,
    int threadID)
{
    auto cells =
        world.getCellsInChunk(
            chunkX,
            chunkY);

    for (auto &cell : cells)
    {
        addToDirtyBuffer(
            world,
            cell.x,
            cell.y,
            color,
            threadID);
    }
}

void RendererSFML::addToDirtyCells(
    World &world,
    uint16_t x,
    uint16_t y,
    sf::Color color)
{
    if (!world.isValid(
            x,
            y))
    {
        return;
    }

    dirtyCells.emplace_back(
        x,
        y,
        color);
}

void RendererSFML::addChunkToDirtyCells(
    World &world,
    uint16_t chunkX,
    uint16_t chunkY,
    sf::Color color)
{
    auto cells =
        world.getCellsInChunk(
            chunkX,
            chunkY);

    for (auto &cell : cells)
    {
        addToDirtyCells(
            world,
            cell.x,
            cell.y,
            color);
    }
}

void RendererSFML::render(
    World& world,
    Human& human,
    const Stats::Data& stats)
{
    updateWorldLayer(world);

    updateHumanLayer(human);

    StatsUI::draw(stats);
}

void RendererSFML::end()
{
    if (textureDirty)
    {
        texture.update(
            pixelImage);

        textureDirty = false;
    }

    window.clear();

    window.draw(sprite);

    if (!humanLayer.vertices.empty())
    {
        window.draw(
            humanLayer.buffer,
            0,
            humanLayer.vertices.size());
    }

    ImGui::SFML::Render(window);

    window.display();
}

