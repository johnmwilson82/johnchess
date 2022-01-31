#include "gtest/gtest.h"

#include "ai.h"

#include <bitboards/bitboard.h>
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

TEST_F(PerftTests, CheckStartPosition1)
{
    BitBoard board;
    board.set_to_start_position();

    EXPECT_EQ(perft(board, 1), 20);
}

TEST_F(PerftTests, CheckStartPosition2)
{
    BitBoard board;
    board.set_to_start_position();

    EXPECT_EQ(perft(board, 2), 400);
}


TEST_F(PerftTests, CheckStartPosition3)
{
    BitBoard board;
    board.set_to_start_position();

    EXPECT_EQ(perft(board, 3), 8902);
}

TEST_F(PerftTests, CheckStartPosition4)
{
    BitBoard board;
    board.set_to_start_position();

    EXPECT_EQ(perft(board, 4), 197281);
}

TEST_F(PerftTests, CheckStartPosition5)
{
    BitBoard board;
    board.set_to_start_position();

    EXPECT_EQ(perft(board, 5), 4865609);
}

/*TEST_F(PerftTests, CheckStartPosition6)
{
    BitBoard board;
    board.set_to_start_position();

    EXPECT_EQ(perft(board, 6), 119060324);
}

TEST_F(PerftTests, CheckStartPosition7)
{
    BitBoard board;
    board.set_to_start_position();

    EXPECT_EQ(perft(board, 7), 3195901860);
}*/

TEST_F(PerftTests, CheckPosition4)
{
    std::string board_str(
        " r _ _ _ k _ _ r\n"
        " P p p p _ p p p\n"
        " _ b _ _ _ n b N\n"
        " n P _ _ _ _ _ _\n"
        " B B P _ P _ _ _\n"
        " q _ _ _ _ N _ _\n"
        " P p _ P _ _ P P\n"
        " R _ _ Q _ R K _\n"
        "w kq - 0 0\n"
    );

    BitBoard board = board_from_string_repr<BitBoard>(board_str);

    EXPECT_EQ(perft(board, 1), 6);
    EXPECT_EQ(perft(board, 2), 264);
    EXPECT_EQ(perft(board, 3), 9467);
    EXPECT_EQ(perft(board, 4), 422333);
    EXPECT_EQ(perft(board, 5), 15833292);
}

TEST_F(PerftTests, CheckPosition5)
{
    std::string board_str(
        " r n b q _ k _ r\n"
        " p p _ P b p p p\n"
        " _ _ p _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ B _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " P P P _ N n P P\n"
        " R N B Q K _ _ R\n"
        "w KQ - 1 8\n"
    );

    BitBoard board = board_from_string_repr<BitBoard>(board_str);

    EXPECT_EQ(perft(board, 1), 44);
    EXPECT_EQ(perft(board, 2), 1486);
    EXPECT_EQ(perft(board, 3), 62379);
    EXPECT_EQ(perft(board, 4), 2103487);
    EXPECT_EQ(perft(board, 5), 89941194);
}