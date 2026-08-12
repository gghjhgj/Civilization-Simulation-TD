#pragma once

#include <SFML/Graphics.hpp>

#include <string>


class LoadingScreen
{
public:

    LoadingScreen(
        sf::RenderWindow& window,
        int width,
        int height
    );


    bool update(
        float progress,
        const std::string& status
    );


    bool isOpen() const;


private:

    sf::RenderWindow& window;


    sf::Font font;


    sf::Text title;
    sf::Text statusText;
    sf::Text percentageText;


    sf::RectangleShape background;
    sf::RectangleShape progressBarBackground;
    sf::RectangleShape progressBar;


    int width;
    int height;


    bool running = true;
};