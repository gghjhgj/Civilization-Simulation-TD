#pragma once

#include <glad/glad.h>

#include "Config.h"
#include "Cell.h"

#include <sstream>
#include <fstream>
#include <iostream>

class WorldGPU
{
    GLuint shaderID = 0;
    GLuint bufferID = 0;

    size_t getWorldSizeInBytes() const
    {
        return Config::sizeX * Config::sizeY * sizeof(Cell);
    }

    public:
    WorldGPU() = default;
    ~WorldGPU();

    std::string loadShaderSource(const std::string& filePath);
    void init(const void* dataPtr);
    void downloadData(void* destPtr);
    void runShader();
    void printDebugData();

};