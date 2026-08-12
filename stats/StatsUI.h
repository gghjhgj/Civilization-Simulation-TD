#pragma once

#include "Stats.h"
#include "../Config/ConfigConstexpr.h"

class StatsUI
{
public:
    static void draw(const Stats::Data& data);
};