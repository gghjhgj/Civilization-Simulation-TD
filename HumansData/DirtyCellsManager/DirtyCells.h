#include <cstdint>
#include "../XY.h"

const uint32_t MAX_DIRTY_CELLS = 10000;
struct DirtyCells {
    uint32_t count = 0;
    XY cells[MAX_DIRTY_CELLS];
};