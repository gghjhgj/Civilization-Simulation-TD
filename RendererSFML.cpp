#include "RendererSFML.h"
#include "Army.h"
#include "Monsters.h"
RendererSFML::RendererSFML(int w, int h, int cellSize)
    : window(sf::VideoMode({ (unsigned)(w * cellSize), (unsigned)(h * cellSize) }), "Simulation"),
    cellSize(cellSize),
    pixelImage({ (unsigned)(w * cellSize), (unsigned)(h * cellSize) }, sf::Color(200, 255, 200)),
    texture({ (unsigned)(w * cellSize), (unsigned)(h * cellSize) }),
    sprite(texture)


{
    if (!armyShader.loadFromFile("army.vert", sf::Shader::Type::Vertex))
    {
        MessageBoxA(NULL, "Shader load failed", "Error", MB_OK);
    }
    setArmyColors();


    int maxSize = Config::maxUnits;
    for (int i = 0; i < RendererSFML::Profs::COUNT; i++)
    {
        auto& entry = armyLayers[i];
        entry.armyVertices.reserve(maxSize);
        entry.armyLayer.create(maxSize);
    }

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
    uint32_t chunkX = x / ChunkConfig::CHUNK_SIZE;
    uint32_t chunkY = y / ChunkConfig::CHUNK_SIZE;


    BuildingType building = world.getBuilding(chunkX, chunkY);

    if (building != BuildingType::None)
    {
        if (world.hasChunkFlag(chunkX, chunkY, ChunkFlag::Construction))
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
void RendererSFML::renderHumans(World& world, Human& human)
{
    for (auto& h : human.humans)
    {
        updateCellPixels(h.oldPos.x, h.oldPos.y, getColor(world, h.oldPos.x, h.oldPos.y));
        updateCellPixels(h.pos.x, h.pos.y, sf::Color::Black);
        h.oldPos.x = h.pos.x;
        h.oldPos.y = h.pos.y;
    }
    for (auto& dead : human.deadHumans)
    {
        updateCellPixels(
            dead.oldPos.x,
            dead.oldPos.y,
            getColor(world, dead.oldPos.x, dead.oldPos.y)
        );
    }

    human.deadHumans.clear();
}
void RendererSFML::render(World& world, Human& human)
{
    for (auto& cell : world.dirtyCells)
    {
        updateCellPixels(cell.x, cell.y, getColor(world, cell.x, cell.y));
    }
    renderHumans(world, human);
    world.dirtyCells.clear();
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

    window.display();
}