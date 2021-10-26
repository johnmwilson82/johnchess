#include "gtest/gtest.h"

#define private public

#include <board.h>
#include <utils\board_strings.h>

using namespace utils;

class BoardReadWriteTests : public ::testing::Test 
{
protected:
    void SetUp()
    {
    }
};


TEST_F(BoardReadWriteTests, CheckReadFromString) 
{
    std::string board_str(
        " r n b k _ b r _\n"
        " _ _ _ p _ p _ _\n"
        " p _ p _ _ _ p _\n"
        " _ _ _ N p _ B p\n"
        " _ p _ P P _ _ _\n"
        " _ _ _ B _ N _ Q\n"
        " P P P _ _ P P P\n"
        " R _ _ _ K _ _ R\n"
    );

    Board board = board_from_string_repr(board_str);

    EXPECT_EQ(board.square(BoardLocation("a8", board)).get_piece()->get_type(), Piece::ROOK);
    EXPECT_EQ(board.square(BoardLocation("a8", board)).get_piece()->get_colour(), Piece::BLACK);

    EXPECT_EQ(board.square(BoardLocation("e1", board)).get_piece()->get_type(), Piece::KING);
    EXPECT_EQ(board.square(BoardLocation("e1", board)).get_piece()->get_colour(), Piece::WHITE);

    EXPECT_EQ(board.square(BoardLocation("e4", board)).get_piece()->get_type(), Piece::PAWN);
    EXPECT_EQ(board.square(BoardLocation("e4", board)).get_piece()->get_colour(), Piece::WHITE);
}


TEST_F(BoardReadWriteTests, CheckWriteToString)
{
    std::string board_str(
        " r n b q k b n r\n"
        " p p p p p p p p\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " P P P P P P P P\n"
        " R N B Q K B N R\n"
    );

    Board board;
    board.set_to_start_position();

    auto out_str = board_to_string_repr(board);

    EXPECT_EQ(board_str, out_str);
}

TEST_F(BoardReadWriteTests, CheckWriteToString2)
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

    Board board;

    board.delete_all_pieces();
    // Kings will occupy the first two elements of m_pieces for
    // quick lookup
    board.add_piece(Piece::KING,   Piece::WHITE, "e1");
    board.add_piece(Piece::KING,   Piece::BLACK, "e8");
    
    board.add_piece(Piece::PAWN,   Piece::WHITE, "a4");
    board.add_piece(Piece::PAWN,   Piece::WHITE, "b4");
    board.add_piece(Piece::PAWN,   Piece::WHITE, "d4");
    board.add_piece(Piece::PAWN,   Piece::WHITE, "e4");
    board.add_piece(Piece::PAWN,   Piece::WHITE, "f2");
    board.add_piece(Piece::PAWN,   Piece::WHITE, "g2");
    board.add_piece(Piece::PAWN,   Piece::WHITE, "h2");

    board.add_piece(Piece::ROOK,   Piece::WHITE, "a1");
    board.add_piece(Piece::KNIGHT, Piece::WHITE, "b1");
    board.add_piece(Piece::BISHOP, Piece::WHITE, "g5");
    board.add_piece(Piece::QUEEN,  Piece::WHITE, "h3");
    board.add_piece(Piece::BISHOP, Piece::WHITE, "c4");
    board.add_piece(Piece::KNIGHT, Piece::WHITE, "f3");

    board.add_piece(Piece::ROOK,   Piece::WHITE, "h1");
    board.add_piece(Piece::PAWN,   Piece::BLACK, "a6");
    board.add_piece(Piece::PAWN,   Piece::BLACK, "b7");
    board.add_piece(Piece::PAWN,   Piece::BLACK, "c6");
    board.add_piece(Piece::PAWN,   Piece::BLACK, "d6");
    board.add_piece(Piece::PAWN,   Piece::BLACK, "f7");
    board.add_piece(Piece::PAWN,   Piece::BLACK, "h5");

    board.add_piece(Piece::ROOK,   Piece::BLACK, "a8");
    board.add_piece(Piece::BISHOP, Piece::BLACK, "c8");
    board.add_piece(Piece::KNIGHT, Piece::BLACK, "d7");
    board.add_piece(Piece::BISHOP, Piece::BLACK, "f8");
    board.add_piece(Piece::ROOK,   Piece::BLACK, "g8");

    auto out_str = board_to_string_repr(board);

    EXPECT_EQ(board_str, out_str);
}