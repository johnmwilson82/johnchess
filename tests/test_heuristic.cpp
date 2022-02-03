#include "gtest/gtest.h"

#include "ai.h"

#include <heuristic.h>
#include <bitboards\bitboard.h>
#include <utils\board_strings.h>

using namespace utils;

class HeuristicTests : public ::testing::Test
{
protected:
    void SetUp()
    {
    }
};

TEST_F(HeuristicTests, CheckShannonHeuristic)
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

    ShannonHeuristic white_heuristic(board, PieceColour::WHITE);
    ShannonHeuristic black_heuristic(board, PieceColour::BLACK);

    EXPECT_EQ(white_heuristic.get(), 2.0f);
    EXPECT_EQ(black_heuristic.get(), -2.0f);
}