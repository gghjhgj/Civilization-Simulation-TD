#pragma once
#include<cstdint>

enum class ChunkFlag : uint32_t
{
    CivZone = 1 << 0,
    Construction = 1 << 1,
};