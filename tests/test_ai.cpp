#include "gtest/gtest.h"

#include "ai.h"

#include <bitboards/bitboard.h>
#include <utils/board_strings.h>
#include <chrono>

static auto deadline() {
    return std::chrono::steady_clock::now() + std::chrono::seconds(30);
}

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

    BasicAI ai(PieceColour::WHITE);

    auto move = ai.make_move(board, deadline());

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

    BasicAI ai(PieceColour::BLACK);

    auto move = ai.make_move(board, deadline());

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

    BasicAI ai(PieceColour::BLACK);

    auto move = ai.make_move(board, deadline());

    // Bishop on h2 attacks b8 via the h2-g3-f4-e5-d6-c7-b8 diagonal.
    // Acceptable responses: queen retreats from b8, or knight blocks on d6.
    bool queen_retreats = move.get_from_loc().to_string() == "b8";
    bool knight_blocks  = move.get_from_loc().to_string() == "c8" && move.get_to_loc().to_string() == "d6";
    EXPECT_TRUE(queen_retreats || knight_blocks);
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

    BasicAI ai(PieceColour::WHITE);

    auto move = ai.make_move(board, deadline());

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

    BasicAI ai(PieceColour::WHITE);

    auto move = ai.make_move(board, deadline());

    EXPECT_EQ(move.to_string(), std::string("g6e7"));
}

TEST_F(AiTests, ThinkCallbackIncludesPrincipalVariation)
{
    // Knight on g6 forks queen on c8 and king on g8 via Nge7+.
    // After the fork the king must move, then the knight takes the queen —
    // so the principal variation should contain at least two moves.
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

    BasicAI ai(PieceColour::WHITE);

    std::string final_variation;
    ThinkCallback cb = [&](uint8_t, int, int, uint64_t, const std::string& principal_variation) {
        final_variation = principal_variation;
    };

    auto move = ai.make_move(board, deadline(), cb);

    EXPECT_EQ(move.to_string(), "g6e7");

    // PV must be non-empty and start with the chosen move.
    EXPECT_FALSE(final_variation.empty());
    EXPECT_EQ(final_variation.substr(0, 4), "g6e7");
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

    BasicAI ai(PieceColour::BLACK);

    auto move = ai.make_move(board, deadline());

    EXPECT_EQ(move.to_string(), std::string("d4e2"));
}

TEST_F(AiTests, SingleLegalMoveIsReturnedImmediately)
{
    // White king on h1, in check from black queen on f1.
    // Rook on g8 covers g1 and g2, so the only escape is h1-h2.
    std::string board_str(
        " _ _ _ _ _ _ r _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ q _ K\n"
    );

    auto board = board_from_string_repr<BitBoard>(board_str);
    board.set_colour_to_move(PieceColour::WHITE);

    BasicAI ai(PieceColour::WHITE);
    auto move = ai.make_move(board, deadline());

    EXPECT_EQ(move.to_string(), "h1h2");
}
