#include "RendererSFML.h"

#include "WorldData/World.h"
#include "HumansData/Human.h"

#ifdef _WIN32
#include <windows.h>
#endif
RendererSFML::RendererSFML(int w, int h, int cellSize)
    : window(sf::VideoMode({ (unsigned)(w * cellSize), (unsigned)(h * cellSize) }), "Simulation"),
    cellSize(cellSize),
    pixelImage({ (unsigned)(w * cellSize), (unsigned)(h * cellSize) }, sf::Color(200, 255, 200)),
    texture({ (unsigned)(w * cellSize), (unsigned)(h * cellSize) }),
    sprite(texture)


{

    dirtyCells.reserve(2000000);
    int threads = tbb::this_task_arena::max_concurrency();

dirtyBuffers.resize(threads);

for(auto& buffer : dirtyBuffers)
{
    buffer.reserve(200000);
}

    humanLayer.vertices.reserve(1000000);
    humanLayer.buffer.create(1000000);

}
bool RendererSFML::isOpen()
{
    return window.isOpen();
}

void RendererSFML::begin()
{
    while (auto event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window.close();
        if (const auto* resized = event->getIf<sf::Event::Resized>())
        {
            sf::Vector2f size(static_cast<float>(resized->size.x), static_cast<float>(resized->size.y));
            sf::Vector2f center(size.x / 2.f, size.y / 2.f);

            window.setView(sf::View(center, size));

        }
    }
}

void RendererSFML::updateCellPixels(int x, int y, sf::Color color)
{
    int startX = x * cellSize;
    int startY = y * cellSize;

    for (int i = 0; i < cellSize; ++i)
    {
        for (int j = 0; j < cellSize; ++j)
        {
            pixelImage.setPixel({ (unsigned)(startX + j), (unsigned)(startY + i) }, color);
        }
    }
}

sf::Color RendererSFML::getColor(World& world, uint16_t x, uint16_t y)
{
    auto ref = world.getCellRef(x, y);


    BuildingType building = world.getBuilding(ref.chunkX, ref.chunkY);

    if (building != BuildingType::None)
    {
        if (world.hasChunkFlag(ref.chunkX, ref.chunkY, ChunkFlag::Construction))
            return sf::Color(255, 128, 0);
        switch (building)
        {
        case BuildingType::House:
            return sf::Color::Red;

        case BuildingType::Farm:
            return sf::Color(255, 255, 150);

        case BuildingType::Sawmill:
            return sf::Color(165, 42, 42);

        case BuildingType::Mine:
            return sf::Color(191, 0, 255);

        default:
            break;
        }
    }

    switch (world.getCell(x, y))
    {
    case TerrainType::Water:
        return sf::Color::Blue;

    case TerrainType::Land:
        return sf::Color::Green;

    case TerrainType::Desert:
        return sf::Color::Yellow;

    case TerrainType::Mountain:
        return sf::Color(120, 120, 120);

    case TerrainType::LandWithFood:
        return sf::Color(255, 165, 0);

    case TerrainType::LandWithTree:
        return sf::Color(0, 120, 0);

    default:
        return sf::Color(200, 255, 200);
    }
}
void RendererSFML::updateHumanLayer(Human& human)
{
    humanLayer.vertices.clear();

    auto add = [&](auto& humans)
    {
        for(size_t i = 0; i < humans.posX.size(); i++)
        {
            sf::Vertex v;

            v.position =
            {
                static_cast<float>(
                    humans.posX[i] * cellSize + cellSize / 2),

                static_cast<float>(
                    humans.posY[i] * cellSize + cellSize / 2)
            };

            v.color = sf::Color::Black;

            humanLayer.vertices.push_back(v);
        }
    };


    add(human.foodCollectors);
    add(human.woodCollectors);
    add(human.stoneCollectors);
    add(human.builders);
    add(human.assigned);


    if(!humanLayer.vertices.empty())
    {
        humanLayer.buffer.update(
            humanLayer.vertices.data(),
            humanLayer.vertices.size(),
            0);
    }
}
void RendererSFML::updateWorldLayer()
{
    mergeDirtyBuffersToDirtyCells();
    for (auto& cell : dirtyCells)
    {
        updateCellPixels(
            cell.coords.x,
            cell.coords.y,
            cell.color
        );
    }

    dirtyCells.clear();
}
void RendererSFML::render(World& world, Human& human)
{
    updateWorldLayer();
    updateHumanLayer(human);
}

void RendererSFML::end()
{
    texture.update(pixelImage);

    window.clear();

    window.draw(sprite);


    if (!humanLayer.vertices.empty())
    {
        window.draw(
            humanLayer.buffer,
            0,
            humanLayer.vertices.size()
        );
    }


    window.display();
}

void RendererSFML::addChunkToDirtyBuffer(World &world, uint16_t chunkX, uint16_t chunkY, sf::Color color, int threadID)
{
    auto cells = world.getCellsInChunk(chunkX, chunkY);
    for(auto &cell : cells)
    {
        addToDirtyBuffer(world, cell.x, cell.y, color, threadID);
    }
}

void RendererSFML::addToDirtyBuffer(World &world, uint16_t x, uint16_t y, sf::Color color, int threadID)
{
    if(world.isValid(x,y))
    {
        dirtyBuffers[threadID].emplace_back(x,y,color);
    }
}

void RendererSFML::addChunkToDirtyCells(World &world, uint16_t chunkX, uint16_t chunkY, sf::Color color)
{
    auto cells = world.getCellsInChunk(chunkX, chunkY);
    for(auto &cell : cells)
    {
        addToDirtyCells(world, cell.x, cell.y, color);
    }
}

void RendererSFML::addToDirtyCells(World &world, uint16_t x, uint16_t y, sf::Color color)
{
    if(world.isValid(x,y))
    {
        dirtyCells.emplace_back(x,y,color);
    }
}

void RendererSFML::mergeDirtyBuffersToDirtyCells()
{
    size_t total = dirtyCells.size();

    for (auto& buffer : dirtyBuffers)
        total += buffer.size();

    dirtyCells.reserve(total);

    for (auto& buffer : dirtyBuffers)
    {
        if (buffer.empty())
            continue;

        dirtyCells.insert(
            dirtyCells.end(),
            buffer.begin(),
            buffer.end()
        );

        buffer.clear();
    }
}