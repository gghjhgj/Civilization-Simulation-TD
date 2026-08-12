#pragma once

#include <SFML/Graphics.hpp>

class LoseScreen
{
public:

    LoseScreen(
        sf::RenderWindow& window,
        int width,
        int height
    );

    void show();

    bool isOpen() const;

private:

    sf::RenderWindow& window;

    sf::Font font;

    sf::Text title;
    sf::Text reason;
    sf::Text instruction;

    sf::RectangleShape background;

    int width;
    int height;

    bool running = true;
};