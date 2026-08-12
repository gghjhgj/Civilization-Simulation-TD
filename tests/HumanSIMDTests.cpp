#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

#include "entities/HumansData/Human.h"
#include "entities/HumansData/HumanProcessing.hpp"

namespace
{

constexpr uint16_t INVALID = UINT16_MAX;

Human::Dirs referenceMove(
    uint16_t x,
    uint16_t y,
    uint16_t targetX,
    uint16_t targetY,
    uint8_t points,
    size_t index)
{
    static constexpr int8_t lookupX[8] =
    {
        1, 1, 0, -1, -1, -1, 0, 1
    };

    static constexpr int8_t lookupY[8] =
    {
        0, 1, 1, 1, 0, -1, -1, -1
    };

    if (targetX == INVALID || targetY == INVALID)
    {
        const uint8_t idx =
            static_cast<uint8_t>((index + points) & 7u);

        return
        {
            lookupX[idx],
            lookupY[idx]
        };
    }

    int8_t dx = 0;
    int8_t dy = 0;

    if (targetX > x)
        dx = 1;
    else if (targetX < x)
        dx = -1;

    if (targetY > y)
        dy = 1;
    else if (targetY < y)
        dy = -1;

    return {dx, dy};
}

uint16_t referenceNewCoordinate(
    uint16_t position,
    int8_t direction)
{
    const int32_t result =
        static_cast<int32_t>(position) +
        static_cast<int32_t>(direction);

    return static_cast<uint16_t>(result);
}

}

class SIMDTest : public ::testing::Test
{
protected:
    Human human;
    HumanBase humans;

    World* world = nullptr;
    RendererSFML* renderer = nullptr;
    Civilization* civilization = nullptr;

    void SetUp() override
    {
    }
};

TEST_F(SIMDTest, TargetRight)
{
    const auto result =
        referenceMove(
            100,
            100,
            200,
            100,
            0,
            0);

    EXPECT_EQ(result.x, 1);
    EXPECT_EQ(result.y, 0);
}

TEST_F(SIMDTest, TargetLeft)
{
    const auto result =
        referenceMove(
            200,
            100,
            100,
            100,
            0,
            0);

    EXPECT_EQ(result.x, -1);
    EXPECT_EQ(result.y, 0);
}

TEST_F(SIMDTest, TargetUp)
{
    const auto result =
        referenceMove(
            100,
            200,
            100,
            100,
            0,
            0);

    EXPECT_EQ(result.x, 0);
    EXPECT_EQ(result.y, -1);
}

TEST_F(SIMDTest, TargetDown)
{
    const auto result =
        referenceMove(
            100,
            100,
            100,
            200,
            0,
            0);

    EXPECT_EQ(result.x, 0);
    EXPECT_EQ(result.y, 1);
}

TEST_F(SIMDTest, TargetDiagonal)
{
    const auto result =
        referenceMove(
            100,
            100,
            200,
            200,
            0,
            0);

    EXPECT_EQ(result.x, 1);
    EXPECT_EQ(result.y, 1);
}

TEST_F(SIMDTest, TargetSamePosition)
{
    const auto result =
        referenceMove(
            100,
            100,
            100,
            100,
            0,
            0);

    EXPECT_EQ(result.x, 0);
    EXPECT_EQ(result.y, 0);
}

TEST_F(SIMDTest, NoTargetUsesFallback)
{
    static constexpr int8_t expectedX[8] =
    {
        1, 1, 0, -1, -1, -1, 0, 1
    };

    static constexpr int8_t expectedY[8] =
    {
        0, 1, 1, 1, 0, -1, -1, -1
    };

    for (size_t i = 0; i < 8; ++i)
    {
        const auto result =
            referenceMove(
                100,
                100,
                INVALID,
                INVALID,
                0,
                i);

        EXPECT_EQ(result.x, expectedX[i]);
        EXPECT_EQ(result.y, expectedY[i]);
    }
}

TEST_F(SIMDTest, FallbackDependsOnPoints)
{
    static constexpr int8_t expectedX[8] =
    {
        1, 1, 0, -1, -1, -1, 0, 1
    };

    static constexpr int8_t expectedY[8] =
    {
        0, 1, 1, 1, 0, -1, -1, -1
    };

    for (uint8_t points = 0; points < 8; ++points)
    {
        const auto result =
            referenceMove(
                100,
                100,
                INVALID,
                INVALID,
                points,
                0);

        const size_t idx =
            static_cast<size_t>(points & 7u);

        EXPECT_EQ(result.x, expectedX[idx]);
        EXPECT_EQ(result.y, expectedY[idx]);
    }
}

TEST_F(SIMDTest, Uint16BoundaryValues)
{
    struct TestCase
    {
        uint16_t x;
        uint16_t y;
        uint16_t tx;
        uint16_t ty;
        int expectedX;
        int expectedY;
    };

    constexpr TestCase cases[] =
    {
        {0, 0, 1, 1, 1, 1},
        {1, 1, 0, 0, -1, -1},

        {65534, 65534, 65534, 65534, 0, 0},
        {65534, 65534, 65534, 65533, 0, -1},
        {65534, 65534, 65533, 65534, -1, 0},

        {65535, 65534, 65534, 65534, -1, 0},
        {65534, 65535, 65534, 65534, 0, -1},

        {32767, 32767, 32768, 32768, 1, 1},
        {32768, 32768, 32767, 32767, -1, -1},

        {65535, 0, 65534, 1, -1, 1},
        {0, 65535, 1, 65534, 1, -1}
    };

    for (const auto& tc : cases)
    {
        const auto result =
            referenceMove(
                tc.x,
                tc.y,
                tc.tx,
                tc.ty,
                0,
                0);

        EXPECT_EQ(result.x, tc.expectedX)
            << "x=" << tc.x
            << " y=" << tc.y
            << " tx=" << tc.tx
            << " ty=" << tc.ty;

        EXPECT_EQ(result.y, tc.expectedY)
            << "x=" << tc.x
            << " y=" << tc.y
            << " tx=" << tc.tx
            << " ty=" << tc.ty;
    }
}

TEST_F(SIMDTest, Uint16PositionArithmetic)
{
    EXPECT_EQ(
        referenceNewCoordinate(0, 1),
        static_cast<uint16_t>(1));

    EXPECT_EQ(
        referenceNewCoordinate(1, -1),
        static_cast<uint16_t>(0));

    EXPECT_EQ(
        referenceNewCoordinate(65534, 1),
        static_cast<uint16_t>(65535));

    EXPECT_EQ(
        referenceNewCoordinate(65535, -1),
        static_cast<uint16_t>(65534));
}

TEST_F(SIMDTest, SixteenLaneBatchReference)
{
    constexpr size_t N = 16;

    std::vector<uint16_t> posX(N);
    std::vector<uint16_t> posY(N);
    std::vector<uint16_t> targetX(N);
    std::vector<uint16_t> targetY(N);
    std::vector<uint8_t> points(N);

    for (size_t i = 0; i < N; ++i)
    {
        posX[i] =
            static_cast<uint16_t>(100 + i);

        posY[i] =
            static_cast<uint16_t>(200 + i);

        points[i] =
            static_cast<uint8_t>(i);

        switch (i & 3u)
        {
        case 0:
            targetX[i] = INVALID;
            targetY[i] = INVALID;
            break;

        case 1:
            targetX[i] =
                static_cast<uint16_t>(posX[i] + 10);
            targetY[i] = posY[i];
            break;

        case 2:
            targetX[i] =
                static_cast<uint16_t>(posX[i] - 10);
            targetY[i] = posY[i];
            break;

        case 3:
            targetX[i] =
                static_cast<uint16_t>(posX[i] + 10);
            targetY[i] =
                static_cast<uint16_t>(posY[i] + 10);
            break;
        }
    }

    for (size_t i = 0; i < N; ++i)
    {
        const auto dir =
            referenceMove(
                posX[i],
                posY[i],
                targetX[i],
                targetY[i],
                points[i],
                i);

        switch (i & 3u)
        {
        case 0:
        {
            static constexpr int8_t lookupX[8] =
            {
                1, 1, 0, -1, -1, -1, 0, 1
            };

            static constexpr int8_t lookupY[8] =
            {
                0, 1, 1, 1, 0, -1, -1, -1
            };

            const size_t idx =
                (i + points[i]) & 7u;

            EXPECT_EQ(dir.x, lookupX[idx]);
            EXPECT_EQ(dir.y, lookupY[idx]);
            break;
        }

        case 1:
            EXPECT_EQ(dir.x, 1);
            EXPECT_EQ(dir.y, 0);
            break;

        case 2:
            EXPECT_EQ(dir.x, -1);
            EXPECT_EQ(dir.y, 0);
            break;

        case 3:
            EXPECT_EQ(dir.x, 1);
            EXPECT_EQ(dir.y, 1);
            break;
        }

        const uint16_t expectedX =
            referenceNewCoordinate(
                posX[i],
                dir.x);

        const uint16_t expectedY =
            referenceNewCoordinate(
                posY[i],
                dir.y);

        const uint16_t actualX =
            referenceNewCoordinate(
                posX[i],
                dir.x);

        const uint16_t actualY =
            referenceNewCoordinate(
                posY[i],
                dir.y);

        EXPECT_EQ(actualX, expectedX);
        EXPECT_EQ(actualY, expectedY);
    }
}

TEST_F(SIMDTest, RandomReferenceCases)
{
    uint32_t seed = 123456789u;

    auto nextRandom = [&]() -> uint16_t
    {
        seed =
            seed * 1664525u +
            1013904223u;

        return static_cast<uint16_t>(
            seed >> 16);
    };

    for (size_t i = 0; i < 100000; ++i)
    {
        const uint16_t x = nextRandom();
        const uint16_t y = nextRandom();

        uint16_t tx;
        uint16_t ty;

        if ((i % 5) == 0)
        {
            tx = INVALID;
            ty = INVALID;
        }
        else
        {
            tx = nextRandom();
            ty = nextRandom();

            if (tx == INVALID)
                tx = 65534;

            if (ty == INVALID)
                ty = 65534;
        }

        const uint8_t points =
            static_cast<uint8_t>(
                nextRandom() & 0xFFu);

        const auto result =
            referenceMove(
                x,
                y,
                tx,
                ty,
                points,
                i);

        EXPECT_GE(result.x, -1);
        EXPECT_LE(result.x, 1);

        EXPECT_GE(result.y, -1);
        EXPECT_LE(result.y, 1);

        if (tx != INVALID && ty != INVALID)
        {
            if (tx > x)
            {
                EXPECT_EQ(result.x, 1);
            }
            else if (tx < x)
            {
                EXPECT_EQ(result.x, -1);
            }
            else
            {
                EXPECT_EQ(result.x, 0);
            }

            if (ty > y)
            {
                EXPECT_EQ(result.y, 1);
            }
            else if (ty < y)
            {
                EXPECT_EQ(result.y, -1);
            }
            else
            {
                EXPECT_EQ(result.y, 0);
            }
        }
    }
}