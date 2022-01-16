#include "gtest/gtest.h"

#include "ai.h"

#include <board.h>
#include <utils/board_strings.h>
#include <utils/perft.h>

using namespace utils;

class PerftTests : public ::testing::Test
{
protected:
    void SetUp()
    {
    }
};

// Metrics taken from https://www.chessprogramming.org/Perft_Results

TEST_F(PerftTests, CheckStartDepth1)
{
    Board board;
    board.set_to_start_position();

    EXPECT_EQ(perft(board, 1), 20);
}

TEST_F(PerftTests, CheckStartDepth2)
{
    Board board;
    board.set_to_start_position();

    EXPECT_EQ(perft(board, 2), 400);
}

TEST_F(PerftTests, CheckStartDepth3)
{
    Board board;
    board.set_to_start_position();

    EXPECT_EQ(perft(board, 3), 8902);
}
/*
TEST_F(PerftTests, CheckStartDepth4)
{
    Board board;
    board.set_to_start_position();

    EXPECT_EQ(perft(board, 4), 197281);
}

TEST_F(PerftTests, CheckStartDepth5)
{
    Board board;
    board.set_to_start_position();

    EXPECT_EQ(perft(board, 5), 4865609);
}
*/
