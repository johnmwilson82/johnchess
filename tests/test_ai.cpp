#include "gtest/gtest.h"

#include "ai.h"

#include <bitboards\bitboard.h>
#include <utils\board_strings.h>

using namespace utils;

class AiTests : public ::testing::Test
{
protected:
    void SetUp()
    {
    }
};

TEST_F(AiTests, CheckFindBackRankMateWhite)
{
    std::string board_str(
        " _ _ _ _ _ _ k _\n"
        " _ _ _ _ _ p p p\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ R K _ _ _\n"
    );

    auto board = board_from_string_repr<BitBoard>(board_str);
    board.set_colour_to_move(PieceColour::WHITE);

    BasicAI ai(PieceColour::WHITE, 1);

    auto move = ai.make_move(board);

    EXPECT_EQ(move.to_string(), std::string("d1d8"));
}

TEST_F(AiTests, CheckFindBackRankMateBlack)
{
    std::string board_str(
        " _ r _ _ _ _ k _\n"
        " _ _ _ _ _ p p p\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ P P P\n"
        " _ _ _ _ _ _ K _\n"
    );

    auto board = board_from_string_repr<BitBoard>(board_str);
    board.set_colour_to_move(PieceColour::BLACK);

    BasicAI ai(PieceColour::BLACK, 1);

    auto move = ai.make_move(board);

    EXPECT_EQ(move.to_string(), std::string("b8b1"));
}


TEST_F(AiTests, CheckMoveQueenFromDangerBlack)
{
    std::string board_str(
        " _ q n _ _ r k _\n"
        " p p _ _ _ p p p\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ B\n"
        " _ _ _ _ _ _ K _\n"
    );

    auto board = board_from_string_repr<BitBoard>(board_str);
    board.set_colour_to_move(PieceColour::BLACK);

    BasicAI ai(PieceColour::BLACK, 3);

    auto move = ai.make_move(board);

    EXPECT_EQ(move.to_string(), std::string("b8a8"));
}

TEST_F(AiTests, CheckMoveQueenFromDangerWhite)
{
    std::string board_str(
        " r b b _ _ r k _\n"
        " p p _ _ _ p p p\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ P\n"
        " _ _ _ _ _ _ B Q\n"
        " _ _ _ _ _ _ K _\n"
    );

    auto board = board_from_string_repr<BitBoard>(board_str);
    board.set_colour_to_move(PieceColour::WHITE);

    BasicAI ai(PieceColour::WHITE, 5);

    auto move = ai.make_move(board);

    EXPECT_EQ(move.to_string(), std::string("h2h1"));
}

TEST_F(AiTests, CheckWillForkWhite)
{
    std::string board_str(
        " _ _ q _ _ _ k _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ N _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ K _\n"
    );

    auto board = board_from_string_repr<BitBoard>(board_str);
    board.set_colour_to_move(PieceColour::WHITE);

    BasicAI ai(PieceColour::WHITE, 3);

    auto move = ai.make_move(board);

    EXPECT_EQ(move.to_string(), std::string("g6e7"));
}

TEST_F(AiTests, CheckWillForkBlack)
{
    std::string board_str(
        " _ _ _ _ _ _ k _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ n _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ Q _ _ _ K _\n"
    );

    auto board = board_from_string_repr<BitBoard>(board_str);
    board.set_colour_to_move(PieceColour::BLACK);

    BasicAI ai(PieceColour::BLACK, 4);

    auto move = ai.make_move(board);

    EXPECT_EQ(move.to_string(), std::string("d4e2"));
}
