#pragma once
#include "XY/XY.h"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

struct DirtyCells
{
    XY coords;
    sf::Color color;

     DirtyCells(uint16_t x, uint16_t y, sf::Color c)
        :
        coords{x,y},
        color(c)
    {}
};