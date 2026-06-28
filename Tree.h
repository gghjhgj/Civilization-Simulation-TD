#pragma once
#include "World.h"
class Tree
{
    public:
    int treesCount = 0;
    void addTree(World &world, int index);
    void addTreeNoVec(World &world, int index);
    void createTree(World &world);
    void treeRespawn(World &world);
};