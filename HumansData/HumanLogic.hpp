#pragma once

#include "HumanTypes.h"

#include <vector>
#include <cstdint>
class Human;

template <typename T>
void addHuman(Human &human, std::vector<T>& vec, typename T::DataType data, uint32_t x, uint32_t y)
{
    T h{};

    h.pos = {x, y};
    h.oldPos = {x, y};
    h.targetPos = {UINT32_MAX, UINT32_MAX};
    h.points = 100;
    h.moves = 0;
    h.targetBuilding = BuildingType::None;

    vec.push_back(h);
    human.humansCount++;
}

template <typename T>
void eraseHuman(Human &human, std::vector<T>& vec, int id)
{
    human.dead.push_back({ vec[id].oldPos, vec[id].pos });

    if (id < static_cast<int>(vec.size()) - 1)
        vec[id] = vec.back();

    vec.pop_back();
}

template <typename FromType, typename ToType>
void switchProf(Human &human, std::vector<FromType>& fromVec, int id, std::vector<ToType>& toVec, typename ToType::DataType newData)
{
    HumanBase base = fromVec[id];
    if (id < static_cast<int>(fromVec.size()) - 1)
        fromVec[id] = fromVec.back();

    fromVec.pop_back();
    toVec.push_back({ base, newData });
}

template <typename FromType, typename ToType>
void switchProfLast(
    Human &human,
    std::vector<FromType>& fromVec,
    std::vector<ToType>& toVec,
    typename ToType::DataType newData)
{
    if (fromVec.empty()) return;

    ToType temp{};

    static_cast<HumanBase&>(temp) = static_cast<HumanBase&>(fromVec.back());

    temp.targetBuilding = newData;

    toVec.push_back(temp);

    fromVec.pop_back();
}



template <typename Func>
void dispatchToVector(HumanType type, Human &human, Func&& func)
{
    switch (type) 
    {
    case HumanType::FoodCollector:  func(human.foodCollectors);   break;
    case HumanType::WoodCollector:  func(human.woodCollectors);   break;
    case HumanType::StoneCollector: func(human.stoneCollectors);  break;
    case HumanType::Builder:        func(human.builders);         break;
    case HumanType::Assigned:       func(human.assigned);         break;
    }
}