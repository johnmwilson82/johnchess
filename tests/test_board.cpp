#include "gtest/gtest.h"

#define private public
#include "board.h"


class BoardTests : public ::testing::Test 
{
protected:

    std::unique_ptr<Board> board;

    void SetUp()
    {
        board = std::make_unique<Board>(8, 8);
    }
};

TEST_F(BoardTests, CheckKingMoves) 
{
    board->delete_all_pieces();
    // Kings will occupy the first two elements of m_pieces for
    // quick lookup
    board->add_piece(Piece::KING,   Piece::WHITE, "e1");
    board->add_piece(Piece::KING,   Piece::BLACK, "e8");

    board->add_piece(Piece::PAWN,   Piece::WHITE, "a2");
    board->add_piece(Piece::PAWN,   Piece::WHITE, "b2");
    board->add_piece(Piece::PAWN,   Piece::WHITE, "c2");
    board->add_piece(Piece::PAWN,   Piece::WHITE, "d5");
    board->add_piece(Piece::PAWN,   Piece::WHITE, "e4");
    board->add_piece(Piece::PAWN,   Piece::WHITE, "f2");
    board->add_piece(Piece::PAWN,   Piece::WHITE, "g2");
    board->add_piece(Piece::PAWN,   Piece::WHITE, "h2");
    board->add_piece(Piece::ROOK,   Piece::WHITE, "a1");
    board->add_piece(Piece::KNIGHT, Piece::WHITE, "b1");
    board->add_piece(Piece::BISHOP, Piece::WHITE, "g5");
    board->add_piece(Piece::QUEEN,  Piece::WHITE, "g4");
    board->add_piece(Piece::BISHOP, Piece::WHITE, "f1");
    board->add_piece(Piece::KNIGHT, Piece::WHITE, "d7");
    board->add_piece(Piece::ROOK,   Piece::WHITE, "h1");

    board->add_piece(Piece::PAWN,   Piece::BLACK, "a6");
    board->add_piece(Piece::PAWN,   Piece::BLACK, "b7");
    board->add_piece(Piece::PAWN,   Piece::BLACK, "c6");
    board->add_piece(Piece::PAWN,   Piece::BLACK, "d6");
    board->add_piece(Piece::PAWN,   Piece::BLACK, "f7");
    board->add_piece(Piece::ROOK,   Piece::BLACK, "a8");
    board->add_piece(Piece::BISHOP, Piece::BLACK, "c8");
    board->add_piece(Piece::BISHOP, Piece::BLACK, "f8");
    board->add_piece(Piece::ROOK,   Piece::BLACK, "h7");

    auto piece = board->square(4, 7).get_piece();
    EXPECT_EQ(piece->get_type(), Piece::KING);

    auto moves = piece->get_all_valid_moves(*board);
    EXPECT_EQ(moves.size(), 0);
}