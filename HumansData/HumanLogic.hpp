#pragma once

#include "HumanTypes.h"

#include <vector>
#include <cstdint>
class Human;

inline uint32_t random100()
{
    thread_local uint32_t state =
        std::chrono::high_resolution_clock::now()
            .time_since_epoch()
            .count();

    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;

    return (state % 100) + 1;
}

inline void addHuman(Human &human, HumanBase &vec, BuildingType data, uint16_t x, uint16_t y)
{
    vec.posX.push_back(x);
    vec.posY.push_back(y);

    vec.targetX.push_back(UINT16_MAX);
    vec.targetY.push_back(UINT16_MAX);

    vec.points.push_back(random100());
    vec.moves.push_back(0);

    vec.targetBuilding.push_back(data);

    human.humansCount++;
}

template <typename T>
void eraseHuman(Human &human, T &vec, int id)
{
    int last = static_cast<int>(vec.posX.size()) - 1;

    if (id != last)
    {
        vec.posX[id] = vec.posX[last];
        vec.posY[id] = vec.posY[last];

        vec.targetX[id] = vec.targetX[last];
        vec.targetY[id] = vec.targetY[last];

        vec.points[id] = vec.points[last];
        vec.moves[id] = vec.moves[last];

        vec.targetBuilding[id] = vec.targetBuilding[last];
    }

    vec.posX.pop_back();
    vec.posY.pop_back();

    vec.targetX.pop_back();
    vec.targetY.pop_back();

    vec.points.pop_back();
    vec.moves.pop_back();

    vec.targetBuilding.pop_back();
}

inline void switchProf(Human &human, HumanBase &fromVec, int id, HumanBase &toVec, BuildingType newData)
{
    toVec.posX.push_back(fromVec.posX[id]);
    toVec.posY.push_back(fromVec.posY[id]);

    toVec.targetX.push_back(UINT16_MAX);
    toVec.targetY.push_back(UINT16_MAX);

    toVec.points.push_back(fromVec.points[id]);
    toVec.moves.push_back(fromVec.moves[id]);

    toVec.targetBuilding.push_back(newData);

    eraseHuman(human, fromVec, id);
}

inline void switchProfLast(Human &human, HumanBase &fromVec, HumanBase &toVec, BuildingType newData)
{
    if (fromVec.posX.empty())
        return;

    int id = fromVec.posX.size() - 1;

    toVec.posX.push_back(fromVec.posX[id]);
    toVec.posY.push_back(fromVec.posY[id]);

    toVec.targetX.push_back(UINT16_MAX);
    toVec.targetY.push_back(UINT16_MAX);

    toVec.points.push_back(fromVec.points[id]);
    toVec.moves.push_back(fromVec.moves[id]);

    toVec.targetBuilding.push_back(newData);

    eraseHuman(human, fromVec, id);
}

template <typename Func>
void dispatchToVector(HumanType type, Human &human, Func &&func)
{
    switch (type)
    {
    case HumanType::FoodCollector:
        func(human.foodCollectors);
        break;
    case HumanType::WoodCollector:
        func(human.woodCollectors);
        break;
    case HumanType::StoneCollector:
        func(human.stoneCollectors);
        break;
    case HumanType::Builder:
        func(human.builders);
        break;
    case HumanType::Assigned:
        func(human.assigned);
        break;
    }
}