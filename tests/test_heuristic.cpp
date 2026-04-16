#include "gtest/gtest.h"

#include "ai.h"

#include <heuristic.h>
#include <bitboards/bitboard.h>
#include <utils/board_strings.h>

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
    // White rook vs black three pawns. No pawn weaknesses on either side
    // (black pawns on f/g/h are all connected), so D/S/I terms are zero.
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

    // Rook - 3 pawns = +2 material; mobility adds 0.1 per move differential
    EXPECT_GT(white_heuristic.get(), 2.0f);
    EXPECT_LT(black_heuristic.get(), -2.0f);
    EXPECT_EQ(white_heuristic.get(), -black_heuristic.get());
}

TEST_F(HeuristicTests, DoubledPawnsArePenalised)
{
    // White doubled (e4+e5 on same file), black undoubled (d5+f5). Equal material.
    std::string doubled_str(
        " k _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ p P p _ _\n"
        " _ _ _ _ P _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " K _ _ _ _ _ _ _\n"
    );
    // White undoubled (d4+f4), black undoubled (d5+f5). Equal material.
    std::string undoubled_str(
        " k _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ p _ p _ _\n"
        " _ _ _ P _ P _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " K _ _ _ _ _ _ _\n"
    );

    auto board_doubled = board_from_string_repr<BitBoard>(doubled_str);
    board_doubled.set_colour_to_move(PieceColour::WHITE);
    auto board_undoubled = board_from_string_repr<BitBoard>(undoubled_str);
    board_undoubled.set_colour_to_move(PieceColour::WHITE);

    ShannonHeuristic doubled(board_doubled, PieceColour::WHITE);
    ShannonHeuristic undoubled(board_undoubled, PieceColour::WHITE);

    EXPECT_LT(doubled.get(), undoubled.get());
}

TEST_F(HeuristicTests, IsolatedPawnsArePenalised)
{
    // White isolated (a4+h4, no adjacent friendly pawns), black connected (b5+c5). Equal material.
    std::string isolated_str(
        " k _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ p p _ _ _ _ _\n"
        " P _ _ _ _ _ _ P\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " K _ _ _ _ _ _ _\n"
    );
    // White connected (a4+b4), black connected (b5+c5). Equal material.
    std::string connected_str(
        " k _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ p p _ _ _ _ _\n"
        " P P _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " K _ _ _ _ _ _ _\n"
    );

    auto board_isolated = board_from_string_repr<BitBoard>(isolated_str);
    board_isolated.set_colour_to_move(PieceColour::WHITE);
    auto board_connected = board_from_string_repr<BitBoard>(connected_str);
    board_connected.set_colour_to_move(PieceColour::WHITE);

    ShannonHeuristic isolated(board_isolated, PieceColour::WHITE);
    ShannonHeuristic connected(board_connected, PieceColour::WHITE);

    EXPECT_LT(isolated.get(), connected.get());
}

TEST_F(HeuristicTests, BlockedPawnsArePenalised)
{
    // White pawn on e4 blocked by own rook on e5.
    std::string blocked_str(
        " k _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ R _ _ _\n"
        " _ _ _ _ P _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " K _ _ _ _ _ _ _\n"
    );
    // White pawn on d4, rook still on e5 — pawn is no longer blocked.
    std::string unblocked_str(
        " k _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ R _ _ _\n"
        " _ _ _ P _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " K _ _ _ _ _ _ _\n"
    );

    auto board_blocked = board_from_string_repr<BitBoard>(blocked_str);
    board_blocked.set_colour_to_move(PieceColour::WHITE);
    auto board_unblocked = board_from_string_repr<BitBoard>(unblocked_str);
    board_unblocked.set_colour_to_move(PieceColour::WHITE);

    ShannonHeuristic blocked(board_blocked, PieceColour::WHITE);
    ShannonHeuristic unblocked(board_unblocked, PieceColour::WHITE);

    EXPECT_LT(blocked.get(), unblocked.get());
}