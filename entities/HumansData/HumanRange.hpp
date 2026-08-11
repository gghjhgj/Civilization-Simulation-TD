#pragma once

#include "HumanProcessing.hpp"

class Human;
class World;
class RendererSFML;

template <HumanType HType>
inline void processHumanRange(
    Human& human,
    HumanBase& h,
    World& world,
    RendererSFML& renderer,
    Civilization &civilization,
    size_t begin,
    size_t end,
    int threadID)
{
    constexpr size_t BATCH_SIZE = 16;

    const size_t batchEnd =
        begin +
        ((end - begin) / BATCH_SIZE) * BATCH_SIZE;

    for (size_t batch = begin;
         batch < batchEnd;
         batch += BATCH_SIZE)
    {
        processHumanTypeBatch<HType>(
            human,
            h,
            world,
            renderer,
            civilization,
            batch,
            threadID);
    }

    for (size_t i = batchEnd;
         i < end;
         ++i)
    {
        processHumanType<HType>(
            human,
            h,
            world,
            renderer,
            civilization,
            i,
            threadID);
    }
}

