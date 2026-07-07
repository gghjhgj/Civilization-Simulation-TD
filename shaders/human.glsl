#version 430 core

layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

const int TASK_Food_Finding = 0;
const int TASK_Tree_Finding = 1;
const int Task_Stone_Finding = 2;
const int Task_Going_To_Building = 3;
const int TASK_Building = 4;

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

struct HumanData
{
    int index;
    int task;
    int id;
    int HP;
    int Stamina;
    int points;
    int targetIndex;
    int targetBuildingID;
    int buildingBuildersID;
};

layout(std430, binding = 0) readonly buffer WorldBuffer
{
    Cell grid[];
};

layout(std430, binding = 1) buffer HumansBuffer
{
    HumanData humans[];
};