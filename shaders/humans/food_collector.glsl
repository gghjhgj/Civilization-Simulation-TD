#version 450

layout(constant_id = 0) const uint WORK_GROUP_SIZE_X = 1;
layout(local_size_x_id = 0, local_size_y = 1, local_size_z = 1) in;

struct XY { uint x; uint y; };
struct HumanBase
{
    uvec2 pos;
    uvec2 oldPos;
    uvec2 targetPos;
    uint targetBuilding;
    uint packedData;
};

layout(std430, binding = 0) buffer HumanBuffer {
    HumanBase humans[];
};

struct Chunk {
    uint data;
};

struct ChunkRegion {
    Chunk chunks[16];
};

layout(std430, binding = 1) buffer WorldBuffer {
    ChunkRegion regions[];
} world;

layout(std430, binding = 2) buffer Stats {
    uint foodCollected;
    uint woodCollected;
    uint stoneCollected;
    uint housesBuilt;
    uint farmsBuilt;
    uint sawmillsBuilt;
    uint minesBuilt;
    uint newHumansInFarms;
    uint newHumansInSawmills;
    uint newHumansInMines;
} stats;

layout(std430, binding = 3) buffer DirtyCells {
    uint count;
    XY cells[]; 
} dirty;

layout(push_constant) uniform PushConstants {
    uint humanCount;
    uint worldRegionsX;
    uint chunksCount;
    uint mapWidth;
    uint mapHeight;
} config;

uint getPoints(uint packedData) {
    return packedData & 0xFFu;
}

uint getMoves(uint packedData) {
    return (packedData >> 8) & 0xFFu;
}

uint getDead(uint packedData) {
    return (packedData >> 16) & 0xFFu;
}

const uint vision = 2;
const uint TERRAIN_LAND_WITH_FOOD = 5;
const uint TERRAIN_LAND = 4;

void atomicMarkDirty(uint x, uint y)
{
    uint slot = atomicAdd(dirty.count, 1);
    if(slot < 10000)
    {
        dirty.cells[slot] = XY(x, y);
    }
}

const uint BITS_PER_CELL = 3;
const uint CELL_MASK = 0x7;
bool atomicSetCell(
    uint regionIndex,
    uint localChunkIndex,
    uint cellIndex,
    uint type)
{
    uint shift = cellIndex * 3u;

    uint mask = ~(0x7u << shift);
    uint value = (type & 0x7u) << shift;


    uint oldValue =
        world.regions[regionIndex]
        .chunks[localChunkIndex]
        .data;


    while(true)
    {
        uint currentCell =
            (oldValue >> shift) & 0x7u;


        // ktoś już zabrał jedzenie
        if(currentCell != TERRAIN_LAND_WITH_FOOD)
            return false;


        uint newValue =
            (oldValue & mask) | value;


        uint prev =
            atomicCompSwap(
                world.regions[regionIndex]
                .chunks[localChunkIndex]
                .data,
                oldValue,
                newValue
            );


        if(prev == oldValue)
            return true;


        oldValue = prev;
    }
}
bool setCellType(uint x, uint y, uint type)
{
    uint chunkX = x / 3u;
    uint chunkY = y / 3u;

    uint regionX = chunkX / 4u;
    uint regionY = chunkY / 4u;

    uint regionIndex =
        regionY * config.worldRegionsX +
        regionX;


    uint localChunkX = chunkX % 4u;
    uint localChunkY = chunkY % 4u;


    uint localChunkIndex =
        localChunkY * 4u +
        localChunkX;


    uint localCellX = x % 3u;
    uint localCellY = y % 3u;


    uint cellIndex =
        localCellY * 3u +
        localCellX;


    return atomicSetCell(
        regionIndex,
        localChunkIndex,
        cellIndex,
        type
    );
}
uint getCellType(uint x, uint y)
{
    if(x >= config.mapWidth || y >= config.mapHeight)
        return 0;

    uint chunkX = x / 3u;
    uint chunkY = y / 3u;

    uint regionX = chunkX / 4u;
    uint regionY = chunkY / 4u;

    uint regionIndex =
        regionY * config.worldRegionsX +
        regionX;

    uint localChunkX = chunkX % 4u;
    uint localChunkY = chunkY % 4u;

    uint localChunkIndex =
        localChunkY * 4u +
        localChunkX;


    uint chunkData =
        world.regions[regionIndex].chunks[localChunkIndex].data;


    uint localCellX = x % 3u;
    uint localCellY = y % 3u;

    uint cellIndex =
        localCellY * 3u +
        localCellX;


    return (chunkData >> (cellIndex * 3u)) & 0x7u;
}
uint setMoves(uint data, uint moves) { 
    return (data & 0xFFFF00FFu) | ((moves & 0xFFu) << 8); 
}
void main() 
{
    uint idx = gl_GlobalInvocationID.x;
    if(idx >= config.humanCount) return;
    uint data = humans[idx].packedData;
    uint points = getPoints(data);
    uint moves = getMoves(data);
    uint dead = getDead(data);
    if(dead == 1u) return;
    moves++;
    uint targetX = 0xFFFFFFFFu;
    uint targetY = 0xFFFFFFFFu;
    bool found = false;

    int vision_i = int(vision);
    for(int dy = -vision_i; dy <= vision_i; dy++) {
        for(int dx = -vision_i; dx <= vision_i; dx++) {
            int nx = int(humans[idx].pos.x) + dx;
            int ny = int(humans[idx].pos.y) + dy;
            if (nx >= 0 && ny >= 0 && nx < config.mapWidth && ny < config.mapHeight) {
                if(getCellType(uint(nx), uint(ny)) == TERRAIN_LAND_WITH_FOOD) {
                    targetX = uint(nx);
                    targetY = uint(ny);
                    found = true;
                    break;
                }
            }
        }
        if(found) break;
    }
    humans[idx].targetPos = uvec2(targetX, targetY);

    int dirX = 0;
    int dirY = 0;

    if(!found)
    {
        uint seed = idx + humans[idx].packedData; 
        uint dirIdx = seed % 8;
        const int lookupX[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
        const int lookupY[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
        dirX = lookupX[dirIdx];
        dirY = lookupY[dirIdx];
    }
    else {
        // Ruch w stronę celu
        int dx = int(targetX) - int(humans[idx].pos.x);
        int dy = int(targetY) - int(humans[idx].pos.y);
        dirX = (dx > 0) ? 1 : ((dx < 0) ? -1 : 0);
        dirY = (dy > 0) ? 1 : ((dy < 0) ? -1 : 0);
    }

    int currentX = int(humans[idx].pos.x);
    int currentY = int(humans[idx].pos.y);
    int nextX_i = currentX + dirX;
    int nextY_i = currentY + dirY;

    uint nextX = uint(clamp(nextX_i, 0, int(config.mapWidth) - 1));
    uint nextY = uint(clamp(nextY_i, 0, int(config.mapHeight) - 1));

    if (found && nextX == targetX && nextY == targetY) {

    bool collected = setCellType(
        nextX,
        nextY,
        TERRAIN_LAND
    );

    if(collected)
    {
        atomicAdd(stats.foodCollected, 1);
        atomicMarkDirty(nextX, nextY);
    }
    else
    {
        humans[idx].targetPos =
            uvec2(0xFFFFFFFFu, 0xFFFFFFFFu);
    }
}

    humans[idx].pos.x = clamp(nextX, 0u, config.mapWidth - 1u);
    humans[idx].pos.y = clamp(nextY, 0u, config.mapHeight - 1u);
    humans[idx].packedData = setMoves(data, moves);
}