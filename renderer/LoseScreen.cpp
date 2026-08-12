#include "LoseScreen.h"

#include <stdexcept>
#include <string>

namespace
{
    constexpr const char* FONT_PATH =
        "assets/fonts/Arial.ttf";
}

LoseScreen::LoseScreen(
    sf::RenderWindow& window,
    int width,
    int height
)
    : window(window),
      font(),
      title(font),
      reason(font),
      instruction(font),
      width(width),
      height(height)
{
    if (!font.openFromFile(FONT_PATH))
    {
        throw std::runtime_error(
            "Failed to load lose screen font: " +
            std::string(FONT_PATH)
        );
    }

    background.setSize(
        sf::Vector2f(
            static_cast<float>(width),
            static_cast<float>(height)
        )
    );

    background.setFillColor(
        sf::Color(18, 18, 18)
    );

    title.setString(
        "CIVILIZATION LOST"
    );

    title.setCharacterSize(48);

    title.setFillColor(
        sf::Color(200, 60, 60)
    );

    const sf::FloatRect titleBounds =
        title.getLocalBounds();

    title.setOrigin(
        titleBounds.position +
        titleBounds.size / 2.0f
    );

    title.setPosition(
        sf::Vector2f(
            static_cast<float>(width) / 2.0f,
            static_cast<float>(height) * 0.32f
        )
    );

    reason.setString(
        "Civilization collapsed due to hunger."
    );

    reason.setCharacterSize(24);

    reason.setFillColor(
        sf::Color(220, 220, 220)
    );

    const sf::FloatRect reasonBounds =
        reason.getLocalBounds();

    reason.setOrigin(
        reasonBounds.position +
        reasonBounds.size / 2.0f
    );

    reason.setPosition(
        sf::Vector2f(
            static_cast<float>(width) / 2.0f,
            static_cast<float>(height) * 0.50f
        )
    );

    instruction.setString(
        "Close the window to exit."
    );

    instruction.setCharacterSize(18);

    instruction.setFillColor(
        sf::Color(150, 150, 150)
    );

    const sf::FloatRect instructionBounds =
        instruction.getLocalBounds();

    instruction.setOrigin(
        instructionBounds.position +
        instructionBounds.size / 2.0f
    );

    instruction.setPosition(
        sf::Vector2f(
            static_cast<float>(width) / 2.0f,
            static_cast<float>(height) * 0.68f
        )
    );
}

void LoseScreen::show()
{
    while (running && window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                running = false;
                window.close();
            }
        }

        if (!running || !window.isOpen())
            break;

        window.clear();

        window.draw(background);
        window.draw(title);
        window.draw(reason);
        window.draw(instruction);

        window.display();
    }
}

bool LoseScreen::isOpen() const
{
    return running && window.isOpen();
}