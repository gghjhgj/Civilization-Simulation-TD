#pragma once
#include "World.h"
class civilization;
class streets;
class Cars
{
    public:
    struct Cargo
    {
        int food = 0;
        int wood = 0;
        int stone = 0;
    };
    struct CarsData
    {
        std::vector<int> carIndex;
        std::vector<Cargo> carCargo;
        std::vector<int> carTargetIndex;
    };
    int carsCount = 0;
    void buildCar(Civilization &civilization);
    void loadCar();
    void sendCar(int startIDX, int endIDX);
    void unloadCar();
};