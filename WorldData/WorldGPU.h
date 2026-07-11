#pragma once

#include <glad/glad.h>
#include "ChunkData/Chunk.h"
#include "Config.h"

#include <sstream>
#include <fstream>
#include <iostream>

class WorldGPU
{
    GLuint shaderID = 0;
    GLuint bufferID = 0;

    size_t getWorldSizeInBytes() const
    {
        return Config::sizeX * Config::sizeY * sizeof(Chunk);
    }

    public:
    WorldGPU() = default;
    ~WorldGPU();

    std::string loadShaderSource(const std::string& filePath);
    void init(const void* dataPtr);
    void downloadData(void* destPtr);
    void uploadCells(size_t startIndex, size_t count, const void* srcPtr);



    //test
    void runShader();
    void printDebugData();
};