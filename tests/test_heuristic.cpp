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

    // Rook - 3 pawns = +2 material. Black's pawns earn a small passed-pawn bonus
    // so the net score is slightly below 2.0; use 1.5 as the meaningful floor.
    EXPECT_GT(white_heuristic.get(), 1.5f);
    EXPECT_LT(black_heuristic.get(), -1.5f);
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

TEST_F(HeuristicTests, PassedPawnsAreRewarded)
{
    // White pawn e6 is passed (black pawn a6 is on a completely different file).
    std::string passed_str(
        " k _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ P _ _ _\n"
        " p _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " K _ _ _ _ _ _ _\n"
    );
    // White pawn e6 is NOT passed: black pawn d7 sits on adjacent file one rank ahead.
    std::string not_passed_str(
        " k _ _ _ _ _ _ _\n"
        " _ _ _ p _ _ _ _\n"
        " _ _ _ _ P _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " K _ _ _ _ _ _ _\n"
    );

    auto board_passed     = board_from_string_repr<BitBoard>(passed_str);
    auto board_not_passed = board_from_string_repr<BitBoard>(not_passed_str);
    board_passed.set_colour_to_move(PieceColour::WHITE);
    board_not_passed.set_colour_to_move(PieceColour::WHITE);

    ShannonHeuristic passed(board_passed, PieceColour::WHITE);
    ShannonHeuristic not_passed(board_not_passed, PieceColour::WHITE);

    EXPECT_GT(passed.get(), not_passed.get());
}

TEST_F(HeuristicTests, RookOnOpenFileIsRewarded)
{
    // White rook e1 on open e-file: the only pawn (a4) is on a different file.
    std::string open_str(
        " k _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " P _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " K _ _ _ R _ _ _\n"
    );
    // White rook e1 on closed e-file: pawn on e4 blocks the file.
    std::string closed_str(
        " k _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ P _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " K _ _ _ R _ _ _\n"
    );

    auto board_open   = board_from_string_repr<BitBoard>(open_str);
    auto board_closed = board_from_string_repr<BitBoard>(closed_str);
    board_open.set_colour_to_move(PieceColour::WHITE);
    board_closed.set_colour_to_move(PieceColour::WHITE);

    ShannonHeuristic open_file(board_open, PieceColour::WHITE);
    ShannonHeuristic closed_file(board_closed, PieceColour::WHITE);

    EXPECT_GT(open_file.get(), closed_file.get());
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