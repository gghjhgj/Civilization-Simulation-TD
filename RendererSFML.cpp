#include "RendererSFML.h"
#include "Army.h"
RendererSFML::RendererSFML(int w, int h, int cellSize)
    : window(sf::VideoMode({(unsigned)(w * cellSize), (unsigned)(h * cellSize)}), "Simulation"),
      cellSize(cellSize),
      pixelImage({(unsigned)(w * cellSize), (unsigned)(h * cellSize)}, sf::Color(200, 255, 200)),
      texture({(unsigned)(w * cellSize), (unsigned)(h * cellSize)}),
      sprite(texture)

      
{
    if (!armyShader.loadFromFile("army.vert", sf::Shader::Type::Vertex))
    {
        MessageBoxA(NULL, "Shader load failed", "Error", MB_OK);
    }
    /*
    armyVertices.resize(Config::maxHumansInArmy);
    armyLayer.create(Config::maxHumansInArmy);
    */
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

void RendererSFML::updateCellPixels(int x, int y, sf::Color color)
{
    int startX = x * cellSize;
    int startY = y * cellSize;

    for (int i = 0; i < cellSize; ++i)
    {
        for (int j = 0; j < cellSize; ++j)
        {
            pixelImage.setPixel({(unsigned)(startX + j), (unsigned)(startY + i)}, color);
        }
    }
}

sf::Color RendererSFML::getColor(const World::Cell& c)
{
    if(c.armyTile.armyProf >= 0) return sf::Color::Black;
    if(c.buildings.House > 0) return sf::Color::Red;
    if(c.buildings.Farm > 0) return sf::Color(255, 255, 150);
    if(c.buildings.Sawmill > 0) return sf::Color(165, 42, 42);
    if(c.buildings.Mine > 0) return sf::Color(191, 0, 255);
    if(c.buildings.Construction > 0) return sf::Color(255, 128, 0);
    if(c.buildings.Factory > 0) return sf::Color::Magenta;
    if(c.humanIndex > 0) return sf::Color::Black;
    if(c.walls.woodWallHP > 0) return sf::Color(139, 69, 19);
    if(c.walls.stoneWallHP > 0) return sf::Color(180, 180, 180);
    if(c.streets.Street > 0) return sf::Color(255, 230, 140);
    if (c.civilizationPlace) return sf::Color::Black;
    if (c.food > 0) return sf::Color(255, 165, 0);
    if (c.treeHP > 0)  return sf::Color(0, 120, 0);
    if (c.stoneHP > 0) return sf::Color::White;

    if (c.flags & World::Water)     return sf::Color::Blue;
    if (c.flags & World::Land)      return sf::Color::Green;
    if (c.flags & World::Sand)      return sf::Color::Yellow;
    if (c.flags & World::Mountain)  return sf::Color(120, 120, 120);

    return sf::Color(200, 255, 200);
}

void RendererSFML::render(World& world)
{
    for (int id : world.dirtyCells)
    {
        int x = id % Config::sizeX;
        int y = id / Config::sizeX;
        updateCellPixels(x, y, getColor(world.grid[id]));
    }
    world.dirtyCells.clear();
}
void RendererSFML::addProfToBuffer(int profession, int logicID) 
{
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
void RendererSFML::reloadArmyBuffer()
{
    for(auto& [profID, layer] : armyLayers)
    {
        if (layer.armyVertices.empty()) continue;

    if (!layer.armyLayer.create(layer.armyVertices.size())) 
    {
        // std::cerr << "Blad: Nie udalo sie stworzyc VertexBuffer!" << std::endl;
        return; 
    }
    if (!layer.armyLayer.update(layer.armyVertices.data(), layer.armyVertices.size(), 0)) 
    {
        // std::cerr << "Blad: Nie udalo sie zaktualizowac VertexBuffer!" << std::endl;
    }
    }
}

void RendererSFML::end()
{
    texture.update(pixelImage);
    window.clear();
    window.draw(sprite);
    sf::RenderStates states;
    states.shader = &armyShader;

    armyShader.setUniform("windowSize", sf::Glsl::Vec2(window.getSize().x, window.getSize().y));
    armyShader.setUniform("time", time);

    if(Army::instance != nullptr)
    {
        for(auto& [profID, layer] : armyLayers)
        {
            if (layer.armyVertices.empty()) continue;
            auto& reg = Army::instance->armyRegistry[profID];
            float spacingX = reg.armyShape.currentSpacingX;
            float spacingY = reg.armyShape.currentSpacingY;
            float noise = reg.armyShape.currentNoise;
            float formationWidth = reg.armyShape.currentWidth;
            float armyMainX = reg.armyMainIndex % Config::sizeX;
            float armyMainY = reg.armyMainIndex / Config::sizeX;

            armyShader.setUniform("spacingX", spacingX);
            armyShader.setUniform("spacingY", spacingY);
            armyShader.setUniform("noise", noise);
            armyShader.setUniform("formationWidth", formationWidth);
            armyShader.setUniform("armyMain", sf::Glsl::Vec2(armyMainX, armyMainY));
            window.draw(layer.armyLayer, states);
        }
    }
    window.display();
}