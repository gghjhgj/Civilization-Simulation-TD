#include "WorldGPU.h"

WorldGPU::~WorldGPU()
{
    if (bufferID != 0) glDeleteBuffers(1, &bufferID);
    if (shaderID != 0) glDeleteProgram(shaderID);

    std::cout << "GPU World Cleaned" << std::endl;
}

std::string WorldGPU::loadShaderSource(const std::string& filePath)
{
    std::ifstream file(filePath);

    if(!file.is_open())
    {
        std::cout << "no file in path: " << filePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void WorldGPU::init(const void* dataPtr)
{
    size_t totalSize = getWorldSizeInBytes();
    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
    glBufferData(GL_SHADER_STORAGE_BUFFER, totalSize, dataPtr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, bufferID);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    std::string shaderCode = loadShaderSource("shaders/world.glsl");
    const char* computeShaderSource = shaderCode.c_str();
    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(shader, 1, &computeShaderSource, NULL);
    glCompileShader(shader);

    shaderID = glCreateProgram();
    glAttachShader(shaderID, shader);
    glLinkProgram(shaderID);
    glDeleteShader(shader);
}

void WorldGPU::downloadData(void* destPtr)
{
    size_t totalSize = getWorldSizeInBytes();

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, totalSize, destPtr);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void WorldGPU::uploadCells(size_t startIndex, size_t count, const void* srcPtr)
{
    if(bufferID == 0) return;

    size_t offsetInBytes = startIndex * sizeof(Cell);

    size_t sizeInBytes = count * sizeof(Cell);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offsetInBytes, sizeInBytes, srcPtr);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}








void WorldGPU::runShader()
{
    if(shaderID == 0 || bufferID == 0) return;
    glUseProgram(shaderID);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, bufferID);
    glDispatchCompute((Config::sizeX + 15) / 16, (Config::sizeY + 15) / 16, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glUseProgram(0);
}

void WorldGPU::printDebugData()
{
    std::cout << "Rozmiar Cell: " << sizeof(Cell) << "bajtow" << std::endl;
    std::cout << "Rozmiar Chunka " << sizeof(Chunk) << "bajtow" << std::endl;
    if(bufferID == 0) return;

    Cell test[2];

    test[0].civZone = -999;
    test[1].civZone = -999;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Cell) * 2, test);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    std::cout << "\n=== GPU DEBUG CHECK ===" << std::endl;
    std::cout << "1" << std::endl;
    std::cout << "-999- nie dziala, cos innego - dziala, WYNIK: " << test[0].civZone << std::endl;
    std::cout << "oczekiwane-1000, WYNIK: " << test[0].building << "\n" << std::endl;

    std::cout << "2" << std::endl;
    std::cout << "-999- nie dziala, cos innego - dziala, WYNIK: " << test[1].civZone << std::endl;
    std::cout << "oczekiwane-1000, WYNIK: " << test[1].building << std::endl;
    std::cout << "=======================\n" << std::endl;
}