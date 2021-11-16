#include "gtest/gtest.h"

#include "ai.h"

#include <board.h>
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

    Board board = board_from_string_repr(board_str);
    board.set_colour_to_move(Piece::WHITE);

    BasicAI ai(Piece::WHITE);

    auto move = ai.make_move(board);

    EXPECT_EQ(move.to_string(), std::string("d1d88"));
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

    Board board = board_from_string_repr(board_str);
    board.set_colour_to_move(Piece::BLACK);

    BasicAI ai(Piece::BLACK);

    auto move = ai.make_move(board);

    EXPECT_EQ(move.to_string(), std::string("b8b1"));
}


TEST_F(AiTests, CheckMoveQueenFromDangerBlack)
{
    std::string board_str(
        " q _ _ _ _ R k _\n"
        " P P P _ _ p p p\n"
        " _ P P _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ K _\n"
    );

    Board board = board_from_string_repr(board_str);
    board.set_colour_to_move(Piece::BLACK);

    BasicAI ai(Piece::BLACK);

    auto move = ai.make_move(board);

    EXPECT_EQ(move.to_string(), std::string("a8e8"));
}