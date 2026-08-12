#include <gtest/gtest.h>

#include "../entities/HumansData/Human.h"
#include "../entities/HumansData/HumanProcessing.hpp"


class HumanMovementTest : public ::testing::Test
{
protected:

    Human human;
    HumanBase humans;

    void SetUp() override
    {
        humans.posX.push_back(100);
        humans.posY.push_back(100);

        humans.targetX.push_back(UINT16_MAX);
        humans.targetY.push_back(UINT16_MAX);

        humans.points.push_back(0);

        humans.targetBuilding.push_back(
            BuildingType::None
        );
    }
};


TEST_F(HumanMovementTest, NoTargetMovesAccordingToPoints)
{
    Human::Dirs expected =
        human.humanMoveDecision(
            humans.posX[0],
            humans.posY[0],
            humans.targetX[0],
            humans.targetY[0],
            humans.points[0]
        );

    uint16_t expectedX =
        humans.posX[0] + expected.x;

    uint16_t expectedY =
        humans.posY[0] + expected.y;

    EXPECT_EQ(expectedX, 101);
    EXPECT_EQ(expectedY, 100);
}


TEST_F(HumanMovementTest, TargetMovesTowardsTarget)
{
    humans.posX[0] = 100;
    humans.posY[0] = 100;

    humans.targetX[0] = 105;
    humans.targetY[0] = 103;

    Human::Dirs dir =
        human.humanMoveDecision(
            humans.posX[0],
            humans.posY[0],
            humans.targetX[0],
            humans.targetY[0],
            humans.points[0]
        );

    EXPECT_EQ(dir.x, 1);
    EXPECT_EQ(dir.y, 1);
}


TEST_F(HumanMovementTest, TargetToLeftMovesLeft)
{
    humans.posX[0] = 100;
    humans.posY[0] = 100;

    humans.targetX[0] = 50;
    humans.targetY[0] = 100;

    Human::Dirs dir =
        human.humanMoveDecision(
            humans.posX[0],
            humans.posY[0],
            humans.targetX[0],
            humans.targetY[0],
            humans.points[0]
        );

    EXPECT_EQ(dir.x, -1);
    EXPECT_EQ(dir.y, 0);
}


TEST_F(HumanMovementTest, TargetAboveMovesUp)
{
    humans.posX[0] = 100;
    humans.posY[0] = 100;

    humans.targetX[0] = 100;
    humans.targetY[0] = 50;

    Human::Dirs dir =
        human.humanMoveDecision(
            humans.posX[0],
            humans.posY[0],
            humans.targetX[0],
            humans.targetY[0],
            humans.points[0]
        );

    EXPECT_EQ(dir.x, 0);
    EXPECT_EQ(dir.y, -1);
}