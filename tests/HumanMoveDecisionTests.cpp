#include <gtest/gtest.h>

#include "../entities/HumansData/Human.h"


TEST(HumanMoveDecisionTest, MovesRight)
{
    Human human;

    Human::Dirs dir =
        human.humanMoveDecision(
            10,
            10,
            20,
            10,
            0
        );

    EXPECT_EQ(dir.x, 1);
    EXPECT_EQ(dir.y, 0);
}


TEST(HumanMoveDecisionTest, MovesLeft)
{
    Human human;

    Human::Dirs dir =
        human.humanMoveDecision(
            20,
            10,
            10,
            10,
            0
        );

    EXPECT_EQ(dir.x, -1);
    EXPECT_EQ(dir.y, 0);
}


TEST(HumanMoveDecisionTest, MovesDown)
{
    Human human;

    Human::Dirs dir =
        human.humanMoveDecision(
            10,
            10,
            10,
            20,
            0
        );

    EXPECT_EQ(dir.x, 0);
    EXPECT_EQ(dir.y, 1);
}


TEST(HumanMoveDecisionTest, MovesUp)
{
    Human human;

    Human::Dirs dir =
        human.humanMoveDecision(
            10,
            20,
            10,
            10,
            0
        );

    EXPECT_EQ(dir.x, 0);
    EXPECT_EQ(dir.y, -1);
}


TEST(HumanMoveDecisionTest, MovesDiagonalDownRight)
{
    Human human;

    Human::Dirs dir =
        human.humanMoveDecision(
            10,
            10,
            20,
            20,
            0
        );

    EXPECT_EQ(dir.x, 1);
    EXPECT_EQ(dir.y, 1);
}


TEST(HumanMoveDecisionTest, MovesDiagonalUpLeft)
{
    Human human;

    Human::Dirs dir =
        human.humanMoveDecision(
            20,
            20,
            10,
            10,
            0
        );

    EXPECT_EQ(dir.x, -1);
    EXPECT_EQ(dir.y, -1);
}


TEST(HumanMoveDecisionTest, DoesNotMoveWhenAtTarget)
{
    Human human;

    Human::Dirs dir =
        human.humanMoveDecision(
            10,
            10,
            10,
            10,
            0
        );

    EXPECT_EQ(dir.x, 0);
    EXPECT_EQ(dir.y, 0);
}


TEST(HumanMoveDecisionTest, NoTargetUsesDeterministicDirection)
{
    Human human;

    const uint16_t invalid = UINT16_MAX;

    for (uint8_t points = 0; points < 8; points++)
    {
        Human::Dirs dir =
            human.humanMoveDecision(
                100,
                100,
                invalid,
                invalid,
                points
            );

        switch (points & 7)
        {
            case 0:
                EXPECT_EQ(dir.x, 1);
                EXPECT_EQ(dir.y, 0);
                break;

            case 1:
                EXPECT_EQ(dir.x, 1);
                EXPECT_EQ(dir.y, 1);
                break;

            case 2:
                EXPECT_EQ(dir.x, 0);
                EXPECT_EQ(dir.y, 1);
                break;

            case 3:
                EXPECT_EQ(dir.x, -1);
                EXPECT_EQ(dir.y, 1);
                break;

            case 4:
                EXPECT_EQ(dir.x, -1);
                EXPECT_EQ(dir.y, 0);
                break;

            case 5:
                EXPECT_EQ(dir.x, -1);
                EXPECT_EQ(dir.y, -1);
                break;

            case 6:
                EXPECT_EQ(dir.x, 0);
                EXPECT_EQ(dir.y, -1);
                break;

            case 7:
                EXPECT_EQ(dir.x, 1);
                EXPECT_EQ(dir.y, -1);
                break;
        }
    }
}