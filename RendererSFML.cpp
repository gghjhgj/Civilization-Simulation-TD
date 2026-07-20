#include "RendererSFML.h"
#include "Army.h"
#include "Monsters.h"
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
    if (!armyShader.loadFromFile("army.vert", sf::Shader::Type::Vertex))
    {
#ifdef _WIN32
        MessageBoxA(NULL, "Shader load failed", "Error", MB_OK);
#else
        std::cerr << "Shader load failed\n";
#endif
    }
    setArmyColors();

    dirtyCells.reserve(2000000);
    int threads = tbb::this_task_arena::max_concurrency();

dirtyBuffers.resize(threads);

for(auto& buffer : dirtyBuffers)
{
    buffer.reserve(200000);
}

    int maxSize = Config::maxUnits;
    for (int i = 0; i < RendererSFML::Profs::COUNT; i++)
    {
        auto& entry = armyLayers[i];
        entry.armyVertices.reserve(maxSize);
        entry.armyLayer.create(maxSize);
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

            reloadArmyBuffer();
        }
    }
}
void RendererSFML::setArmyColors()
{
    armyLayers[Profs::soldier].color = sf::Color::Black;
    armyLayers[Profs::archer].color = sf::Color::Black;
    armyLayers[Profs::normalMonster].color = sf::Color::Red;
    armyLayers[Profs::giantMonster].color = sf::Color::Red;
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

sf::Color RendererSFML::getColor(World& world, uint32_t x, uint32_t y)
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
            for (auto& h : humans)
            {
                sf::Vertex v;

                v.position =
                {
                    static_cast<float>(h.pos.x * cellSize + cellSize / 2),
                    static_cast<float>(h.pos.y * cellSize + cellSize / 2)
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


    if (!humanLayer.vertices.empty())
    {
        humanLayer.buffer.update(
            humanLayer.vertices.data(),
            humanLayer.vertices.size(),
            0
        );
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
void RendererSFML::addProfToBuffer(int profession, Source source, int logicID)
{
    if (source == Source::monstersClass) profession += 2;
    auto& layer = armyLayers[profession];
    sf::Vertex newVertex;
    newVertex.position = sf::Vector2f(0.f, 0.f);
    newVertex.color.r = (logicID & 255);
    newVertex.color.g = (logicID >> 8) & 255;
    newVertex.color.b = (logicID >> 16) & 255;
    newVertex.color.a = 255;
    layer.armyVertices.push_back(newVertex);

    if (layer.armyVertices.size() > layer.armyLayer.getVertexCount())
    {
        if (!layer.armyLayer.create(layer.armyVertices.size() + 1000)) {
            return;
        }

        if (!layer.armyLayer.update(layer.armyVertices.data(), layer.armyVertices.size(), 0)) {
            return;
        }
    }
    else
    {
        if (!layer.armyLayer.update(&newVertex, 1, layer.armyVertices.size() - 1)) {
            return;
        }
    }
}

void RendererSFML::eraseProfFromBuffer(int profession, Source source, int logicID)
{
    if (source == Source::monstersClass) profession += 2;
    /*
    else
    {
    std::cout << armyLayers[profession].armyVertices.size() << " ";
    std::cout << armyLayers[profession].armyLayer.getVertexCount() << std::endl;
    }
    */
    auto& layer = armyLayers[profession];
    int lastIDX = layer.armyVertices.size() - 1;
    if (logicID > lastIDX || logicID < 0) return;
    if (logicID != lastIDX)
    {
        layer.armyVertices[logicID] = layer.armyVertices[lastIDX];
        layer.armyVertices.pop_back();

        if (!layer.armyLayer.update(&layer.armyVertices[logicID], 1, logicID))
        {
            return;
        }
    }
    else
    {
        layer.armyVertices.pop_back();
    }
}

void RendererSFML::reloadArmyBuffer()
{
    for (auto& [profID, layer] : armyLayers)
    {
        if (layer.armyVertices.empty()) continue;
        layer.armyLayer.update(layer.armyVertices.data(), layer.armyVertices.size(), 0);
    }
}
void RendererSFML::end()
{
    //reloadArmyBuffer();
    texture.update(pixelImage);
    window.clear();
    window.draw(sprite);
    sf::RenderStates states;
    states.shader = &armyShader;

    armyShader.setUniform("windowSize", sf::Glsl::Vec2(window.getSize().x, window.getSize().y));
    armyShader.setUniform("time", time);

    for (auto& [profID, layer] : armyLayers)
    {
        if (layer.armyVertices.empty()) continue;

        float spacingX = 0, spacingY = 0, noise = 0, formationWidth = 0;
        float armyMainX = 0, armyMainY = 0;

        if (profID == Profs::soldier || profID == Profs::archer)
        {
            if (Army::instance != nullptr)
            {
                auto& reg = Army::instance->armyRegistry[profID];
                spacingX = reg.armyShape.currentSpacingX;
                spacingY = reg.armyShape.currentSpacingY;
                noise = reg.armyShape.currentNoise;
                formationWidth = reg.armyShape.currentWidth;
                armyMainX = reg.armyMainIndex % Config::sizeX;
                armyMainY = reg.armyMainIndex / Config::sizeX;
            }
        }
        else if (profID == Profs::normalMonster || profID == Profs::giantMonster)
        {
            if (Monsters::instance != nullptr)
            {
                auto& reg = Monsters::instance->monstersRegistry[profID - 2];
                spacingX = reg.monstersShape.currentSpacingX;
                spacingY = reg.monstersShape.currentSpacingY;
                noise = reg.monstersShape.currentNoise;
                formationWidth = reg.monstersShape.currentWidth;
                armyMainX = reg.monstersMainIndex % Config::sizeX;
                armyMainY = reg.monstersMainIndex / Config::sizeX;
            }
        }
        armyShader.setUniform("spacingX", spacingX);
        armyShader.setUniform("spacingY", spacingY);
        armyShader.setUniform("noise", noise);
        armyShader.setUniform("formationWidth", formationWidth);
        armyShader.setUniform("armyMain", sf::Glsl::Vec2(armyMainX, armyMainY));
        armyShader.setUniform("profID", static_cast<float>(profID));

        armyShader.setUniform("color", sf::Glsl::Vec4(
            layer.color.r / 255.f,
            layer.color.g / 255.f,
            layer.color.b / 255.f,
            layer.color.a / 255.f
        ));

        window.draw(layer.armyLayer, 0, layer.armyVertices.size(), states);

    }
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

void RendererSFML::addChunkToDirtyBuffer(World &world, uint32_t chunkX, uint32_t chunkY, sf::Color color, int threadID)
{
    auto cells = world.getCellsInChunk(chunkX, chunkY);
    for(auto &cell : cells)
    {
        addToDirtyBuffer(world, cell.x, cell.y, color, threadID);
    }
}

void RendererSFML::addToDirtyBuffer(World &world, uint32_t x, uint32_t y, sf::Color color, int threadID)
{
    if(world.isValid(x,y))
    {
        dirtyBuffers[threadID].emplace_back(x,y,color);
    }
}

void RendererSFML::addChunkToDirtyCells(World &world, uint32_t chunkX, uint32_t chunkY, sf::Color color)
{
    auto cells = world.getCellsInChunk(chunkX, chunkY);
    for(auto &cell : cells)
    {
        addToDirtyCells(world, cell.x, cell.y, color);
    }
}

void RendererSFML::addToDirtyCells(World &world, uint32_t x, uint32_t y, sf::Color color)
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