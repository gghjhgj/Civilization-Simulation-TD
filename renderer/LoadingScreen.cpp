#include "LoadingScreen.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>


namespace
{
    constexpr const char* FONT_PATH =
        "assets/fonts/Arial.ttf";
}


LoadingScreen::LoadingScreen(
    sf::RenderWindow& window,
    int width,
    int height
)
    : window(window),
      font(),
      title(font),
      statusText(font),
      percentageText(font),
      width(width),
      height(height)
{
    if (!font.openFromFile(FONT_PATH))
    {
        throw std::runtime_error(
            "Failed to load loading screen font: " +
            std::string(FONT_PATH)
        );
    }


    // ============================================================
    // BACKGROUND
    // ============================================================

    background.setSize(
        sf::Vector2f(
            static_cast<float>(width),
            static_cast<float>(height)
        )
    );

    background.setFillColor(
        sf::Color(18, 18, 18)
    );


    // ============================================================
    // TITLE
    // ============================================================

    title.setString(
        "Civilization Simulation"
    );

    title.setCharacterSize(32);

    title.setFillColor(
        sf::Color::White
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
            static_cast<float>(height) * 0.30f
        )
    );


    // ============================================================
    // STATUS
    // ============================================================

    statusText.setCharacterSize(20);

    statusText.setFillColor(
        sf::Color(210, 210, 210)
    );


    // ============================================================
    // PERCENTAGE
    // ============================================================

    percentageText.setCharacterSize(18);

    percentageText.setFillColor(
        sf::Color(180, 180, 180)
    );


    // ============================================================
    // PROGRESS BAR
    // ============================================================

    const float barWidth =
        static_cast<float>(width) * 0.60f;

    const float barHeight = 24.0f;

    const float barX =
        (static_cast<float>(width) - barWidth) / 2.0f;

    const float barY =
        static_cast<float>(height) * 0.65f;


    progressBarBackground.setSize(
        sf::Vector2f(
            barWidth,
            barHeight
        )
    );


    progressBarBackground.setPosition(
        sf::Vector2f(
            barX,
            barY
        )
    );


    progressBarBackground.setFillColor(
        sf::Color(55, 55, 55)
    );


    progressBar.setSize(
        sf::Vector2f(
            0.0f,
            barHeight
        )
    );


    progressBar.setPosition(
        sf::Vector2f(
            barX,
            barY
        )
    );


    progressBar.setFillColor(
        sf::Color(80, 180, 100)
    );
}


bool LoadingScreen::update(
    float progress,
    const std::string& status
)
{
    progress = std::clamp(
        progress,
        0.0f,
        1.0f
    );


    // ============================================================
    // EVENTS
    // ============================================================

    while (const auto event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            running = false;
            window.close();
        }
    }


    if (!running)
        return false;


    // ============================================================
    // STATUS TEXT
    // ============================================================

    statusText.setString(status);


    const sf::FloatRect statusBounds =
        statusText.getLocalBounds();


    statusText.setOrigin(
        statusBounds.position +
        statusBounds.size / 2.0f
    );


    statusText.setPosition(
        sf::Vector2f(
            static_cast<float>(width) / 2.0f,
            static_cast<float>(height) * 0.57f
        )
    );


    // ============================================================
    // PROGRESS BAR
    // ============================================================

    progressBar.setSize(
        sf::Vector2f(
            progressBarBackground.getSize().x * progress,
            progressBarBackground.getSize().y
        )
    );


    // ============================================================
    // PERCENTAGE
    // ============================================================

    const int percentage =
        static_cast<int>(
            std::round(progress * 100.0f)
        );


    percentageText.setString(
        std::to_string(percentage) + "%"
    );


    const sf::FloatRect percentageBounds =
        percentageText.getLocalBounds();


    percentageText.setOrigin(
        percentageBounds.position +
        percentageBounds.size / 2.0f
    );


    percentageText.setPosition(
        sf::Vector2f(
            static_cast<float>(width) / 2.0f,
            static_cast<float>(height) * 0.72f
        )
    );


    // ============================================================
    // RENDER
    // ============================================================

    window.clear();


    window.draw(background);

    window.draw(title);

    window.draw(statusText);

    window.draw(progressBarBackground);

    window.draw(progressBar);

    window.draw(percentageText);


    window.display();


    return true;
}


bool LoadingScreen::isOpen() const
{
    return running && window.isOpen();
}