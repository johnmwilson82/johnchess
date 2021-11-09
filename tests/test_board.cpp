#include "gtest/gtest.h"

#define private public
#include <board.h>
#include <utils\board_strings.h>

using namespace utils;

class BoardTests : public ::testing::Test 
{
protected:
    void SetUp()
    {
    }
};

TEST_F(BoardTests, CheckKingMoves) 
{
    std::string board_str(
        " r _ b _ k b r _\n"
        " _ p _ n _ p _ _\n"
        " p _ p p _ _ _ _\n"
        " _ _ _ _ _ _ B p\n"
        " P P B P P _ _ _\n"
        " _ _ _ _ _ N _ Q\n"
        " _ _ _ _ _ P P P\n"
        " R N _ _ K _ _ R\n"
    );

    Board board = board_from_string_repr(board_str);
    auto piece = board.square(4, 7).get_piece();
    EXPECT_EQ(piece->get_type(), Piece::KING);

    // There are two possible moves for the king (D8 and E7) but both result in check from the bishop on G5
    auto moves = piece->get_all_valid_moves(board);
    ASSERT_EQ(moves.size(), 2);

    EXPECT_FALSE(board.get_in_check(piece->get_colour()));

    for (const auto& move : moves)
    {
        Board test_board1(board, move);
        EXPECT_TRUE(test_board1.get_in_check(piece->get_colour()));

        Board test_board2(board, move);
        EXPECT_TRUE(test_board2.get_in_check(piece->get_colour()));
    }

}

TEST_F(BoardTests, CheckBlackRookCaptureWhitePawn) 
{
    std::string board_str(
        " _ _ b _ _ b r _\n"
        " r P _ _ _ k _ p\n"
        " p _ n p _ _ _ _\n"
        " _ B _ _ P _ B _\n"
        " _ _ _ _ _ _ Q _\n"
        " _ _ _ _ _ _ _ _\n"
        " P P P _ _ _ P P\n"
        " R N _ _ K _ N R\n"
    );

    Board board = board_from_string_repr(board_str);

    std::string board_str2(
        " _ _ b _ _ b r _\n"
        " _ r _ _ _ k _ p\n"
        " p _ B p _ _ _ _\n"
        " _ _ _ _ P _ B _\n"
        " _ _ _ _ _ _ Q _\n"
        " _ _ _ _ _ _ _ _\n"
        " P P P _ _ _ P P\n"
        " R N _ _ K _ N R\n"
    );

    Board board2a(board, "b5c6");
    Board board2(board2a, "a7b7");

    auto out_str2 = board_to_string_repr(board2);

    EXPECT_EQ(board_str2, out_str2);

    std::string board_str3(
        " _ _ b _ _ b r _\n"
        " _ r _ _ _ k _ _\n"
        " p _ B p _ _ _ p\n"
        " _ _ _ _ P _ B _\n"
        " _ _ _ _ _ _ Q _\n"
        " P _ _ _ _ _ _ _\n"
        " _ P P _ _ _ P P\n"
        " R N _ _ K _ N R\n"
    );
    Board board3a(board2, "a2a3");
    Board board3(board3a, "h7h6");

    auto out_str3 = board_to_string_repr(board3);

    EXPECT_EQ(board_str3, out_str3);
}

TEST_F(BoardTests, CheckWhiteMate) 
{
    std::string board_str(
        " r n b q k b _ r\n"
        " p p p p _ Q _ p\n"
        " _ _ _ _ _ _ p _\n"
        " _ _ _ _ p _ _ _\n"
        " _ _ B _ P _ n _\n"
        " _ _ _ P _ _ _ _\n"
        " P P P _ _ P P P\n"
        " R N B _ K _ N R\n"
    );

    Board board = board_from_string_repr(board_str);

    EXPECT_EQ(board.get_mate(Piece::BLACK), Board::CHECKMATE);
    EXPECT_EQ(board.get_mate(Piece::WHITE), Board::NO_MATE);
}
