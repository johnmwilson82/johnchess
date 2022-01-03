#include "gtest/gtest.h"

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

    EXPECT_EQ(board.square(BoardLocation("a8", board)).get_piece()->get_type(), PieceType::ROOK);
    EXPECT_EQ(board.square(BoardLocation("a8", board)).get_piece()->get_colour(), PieceColour::BLACK);

    EXPECT_EQ(board.square(BoardLocation("e1", board)).get_piece()->get_type(), PieceType::KING);
    EXPECT_EQ(board.square(BoardLocation("e1", board)).get_piece()->get_colour(), PieceColour::WHITE);

    EXPECT_EQ(board.square(BoardLocation("e4", board)).get_piece()->get_type(), PieceType::PAWN);
    EXPECT_EQ(board.square(BoardLocation("e4", board)).get_piece()->get_colour(), PieceColour::WHITE);
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
    board.add_piece<King>(PieceColour::WHITE, "e1");
    board.add_piece<King>(PieceColour::BLACK, "e8");
    
    board.add_piece<Pawn>(PieceColour::WHITE, "a4");
    board.add_piece<Pawn>(PieceColour::WHITE, "b4");
    board.add_piece<Pawn>(PieceColour::WHITE, "d4");
    board.add_piece<Pawn>(PieceColour::WHITE, "e4");
    board.add_piece<Pawn>(PieceColour::WHITE, "f2");
    board.add_piece<Pawn>(PieceColour::WHITE, "g2");
    board.add_piece<Pawn>(PieceColour::WHITE, "h2");
    
    board.add_piece<Rook>(PieceColour::WHITE, "a1");
    board.add_piece<Knight>(PieceColour::WHITE, "b1");
    board.add_piece<Bishop>(PieceColour::WHITE, "g5");
    board.add_piece<Queen>(PieceColour::WHITE, "h3");
    board.add_piece<Bishop>(PieceColour::WHITE, "c4");
    board.add_piece<Knight>(PieceColour::WHITE, "f3");
    board.add_piece<Rook>(PieceColour::WHITE, "h1");

    board.add_piece<Pawn>(PieceColour::BLACK, "a6");
    board.add_piece<Pawn>(PieceColour::BLACK, "b7");
    board.add_piece<Pawn>(PieceColour::BLACK, "c6");
    board.add_piece<Pawn>(PieceColour::BLACK, "d6");
    board.add_piece<Pawn>(PieceColour::BLACK, "f7");
    board.add_piece<Pawn>(PieceColour::BLACK, "h5");

    board.add_piece<Rook>(PieceColour::BLACK, "a8");
    board.add_piece<Bishop>(PieceColour::BLACK, "c8");
    board.add_piece<Knight>(PieceColour::BLACK, "d7");
    board.add_piece<Bishop>(PieceColour::BLACK, "f8");
    board.add_piece<Rook>(PieceColour::BLACK, "g8");

    auto out_str = board_to_string_repr(board);

    EXPECT_EQ(board_str, out_str);
}