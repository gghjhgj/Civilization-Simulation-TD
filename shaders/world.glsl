#version 430 core

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

struct Cell
{
    uint packed_flags;
    uint packed_civPlace;
    int humanIndex;
    int civZone;
    uint buildings[2];
    int woodWallHP;
    int stoneWallHP;
};

layout(std430, binding = 0) buffer WorldBuffer
{
    Cell grid[];
};

void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;

    uint sizeX = 1800;
    uint sizeY = 1000;

    if(x >= sizeX || y >= sizeY) return;

    uint idx = y * sizeX + x;


    grid[idx].civZone = 69;
    grid[idx].stoneWallHP = 1000;
}