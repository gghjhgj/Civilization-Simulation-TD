#version 450

layout(constant_id = 0) const uint WORK_GROUP_SIZE_X = 1;
layout(local_size_x_id = 0, local_size_y = 1, local_size_z = 1) in;

layout(push_constant) uniform Config{
    uint worldRegionsX;
    uint chunksCount;
}config;

layout(std430, binding = 0) buffer WorldData{
    uint globalChunks[];
};

const uint CELL_COUNT = 9;
const uint BITS_PER_CELL = 3;
const uint CELL_MASK = 0x7;

const uint BUILDING_SHIFT = 27; // (9 * 3 = 27)
const uint BUILDING_MASK = 0x7;

const uint FLAG_SHIFT = 30; // (27 + 3 = 30)
void setCell(inout uint data, uint index, uint type) {
    uint shift = index * BITS_PER_CELL;
    data &= ~(CELL_MASK << shift);
    data |= (type & CELL_MASK) << shift;
}
void main() {
    uint chunkIndex = gl_LocalInvocationID.x;
    if(chunkIndex >= config.chunksCount)
    {
        return;
    }

    uint rx = gl_WorkGroupID.x;
    uint ry = gl_WorkGroupID.y;
    uint regionIndex = (ry * config.worldRegionsX) + rx;
    uint globalChunkIndex = (regionIndex * config.chunksCount) + chunkIndex;
    //uint chunkData = globalChunks[globalChunkIndex];
    uint chunkData = 0;
    switch(chunkIndex)
    { //1 sand //mtws 3
        case 1:
        {
            setCell(chunkData, 8, 3);
            break;
        }
        case 2:
        {
            setCell(chunkData, 6, 3);
            break;
        }
        case 5:
        {
            setCell(chunkData, 1, 3);
            setCell(chunkData, 2, 3);
            setCell(chunkData, 4, 3);
            setCell(chunkData, 5, 3);
            setCell(chunkData, 7, 1);
            setCell(chunkData, 8, 1);
            break;
        }
        case 6:
        {
            setCell(chunkData, 0, 3);
            setCell(chunkData, 1, 3);
            setCell(chunkData, 3, 3);
            setCell(chunkData, 4, 3);
            setCell(chunkData, 6, 1);
            setCell(chunkData, 7, 1);
            break;
        }
        case 8:
        {
            setCell(chunkData, 8, 1);
            break;
        }
        case 9:
        {
            for(int i = 1; i < 9; i++)
            {
                setCell(chunkData, i, 1);
            }
            break;
        }
        case 10:
        {
            setCell(chunkData, 0, 1);
            setCell(chunkData, 1, 1);
            setCell(chunkData, 3, 1);
            setCell(chunkData, 4, 1);
            for(int i = 6; i < 9; i++)
            {
                setCell(chunkData, i, 1);
            }
            break;
        }
        case 12:
        {
            for(int i = 2; i <= 5; i+=3)
            {
                setCell(chunkData, i, 1);
            }
            break;
        }
        case 13:
        {
            for(int i = 0; i < 7; i++)
            {
                setCell(chunkData, i, 1);
            }
            break;
        }
        case 14:
        {
            for(int i = 0; i < 9; i++)
            {
                setCell(chunkData, i, 1);
            }
            break;
        }
        default: break;
    }
    globalChunks[globalChunkIndex] = chunkData;
}