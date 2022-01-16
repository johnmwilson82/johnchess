#include "gtest/gtest.h"

#define private public
#include <bitboards/bitboard.h>
#include <utils/board_strings.h>

using namespace utils;

class BitboardTests : public ::testing::Test
{
protected:
    void SetUp()
    {
    }
};

TEST_F(BitboardTests, CheckSimpleKnightMoves)
{
    std::string board_str(
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ n _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ N _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        "w KQkq - 0 0\n"
    );

    BitBoard board = board_from_string_repr<BitBoard>(board_str);

    auto white_moves = board.get_all_legal_moves(PieceColour::WHITE);
    EXPECT_EQ(white_moves.size(), 8);

    auto black_moves = board.get_all_legal_moves(PieceColour::BLACK);
    EXPECT_EQ(black_moves.size(), 8);
}

TEST_F(BitboardTests, CheckBlockedKnightMoves)
{
    std::string board_str(
        " _ q _ R _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ n _ _ _ _ _\n"
        " _ _ _ _ p _ P _\n"
        " _ _ _ p _ _ _ P\n"
        " _ _ _ _ _ N _ _\n"
        " _ _ _ _ _ _ _ R\n"
        " _ _ _ _ _ _ B _\n"
        "w KQkq - 0 0\n"
    );

    BitBoard board = board_from_string_repr<BitBoard>(board_str);

    auto white_moves = board.get_all_legal_moves(PieceColour::WHITE);
    EXPECT_EQ(white_moves.size(), 8);

    auto black_moves = board.get_all_legal_moves(PieceColour::BLACK);
    EXPECT_EQ(black_moves.size(), 8);
}