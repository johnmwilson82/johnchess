#include "gtest/gtest.h"

#define private public
#include <bitboards/bitboard.h>
#include <utils/board_strings.h>

#include <ranges>

using namespace utils;

class BitboardTests : public ::testing::Test
{
protected:
    void SetUp()
    {
    }

    bool find_fn(std::list<Move> move_list, const std::string& move_str)
    {
        return std::ranges::find_if(move_list, [&move_str](const Move& move) { return move == Move(move_str); }) != move_list.end();
    };
};

TEST_F(BitboardTests, CheckSimpleKnightMoves)
{
    std::string board_str(
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ n _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ N _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        "w - - 0 0\n"
    );

    BitBoard board = board_from_string_repr<BitBoard>(board_str);

    auto white_moves = board.get_all_legal_moves(PieceColour::WHITE);
    EXPECT_EQ(white_moves.size(), 8);

    EXPECT_TRUE(find_fn(white_moves, "f3g1"));
    EXPECT_TRUE(find_fn(white_moves, "f3g5"));
    EXPECT_TRUE(find_fn(white_moves, "f3e1"));
    EXPECT_TRUE(find_fn(white_moves, "f3e5"));
    EXPECT_TRUE(find_fn(white_moves, "f3d2"));
    EXPECT_TRUE(find_fn(white_moves, "f3h2"));
    EXPECT_TRUE(find_fn(white_moves, "f3d4"));
    EXPECT_TRUE(find_fn(white_moves, "f3h4"));

    auto black_moves = board.get_all_legal_moves(PieceColour::BLACK);
    EXPECT_EQ(black_moves.size(), 8);

    EXPECT_TRUE(find_fn(black_moves, "c6b8"));
    EXPECT_TRUE(find_fn(black_moves, "c6d8"));
    EXPECT_TRUE(find_fn(black_moves, "c6b4"));
    EXPECT_TRUE(find_fn(black_moves, "c6d4"));
    EXPECT_TRUE(find_fn(black_moves, "c6a7"));
    EXPECT_TRUE(find_fn(black_moves, "c6e7"));
    EXPECT_TRUE(find_fn(black_moves, "c6a5"));
    EXPECT_TRUE(find_fn(black_moves, "c6e5"));
}

TEST_F(BitboardTests, CheckSimpleKingMoves)
{
    std::string board_str(
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ k _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ K _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        "w - - 0 0\n"
    );

    BitBoard board = board_from_string_repr<BitBoard>(board_str);

    auto white_moves = board.get_all_legal_moves(PieceColour::WHITE);
    EXPECT_EQ(white_moves.size(), 8);

    EXPECT_TRUE(find_fn(white_moves, "f3f2"));
    EXPECT_TRUE(find_fn(white_moves, "f3f4"));
    EXPECT_TRUE(find_fn(white_moves, "f3e3"));
    EXPECT_TRUE(find_fn(white_moves, "f3g3"));
    EXPECT_TRUE(find_fn(white_moves, "f3e2"));
    EXPECT_TRUE(find_fn(white_moves, "f3g2"));
    EXPECT_TRUE(find_fn(white_moves, "f3e4"));
    EXPECT_TRUE(find_fn(white_moves, "f3e2"));

    auto black_moves = board.get_all_legal_moves(PieceColour::BLACK);
    EXPECT_EQ(black_moves.size(), 8);

    EXPECT_TRUE(find_fn(black_moves, "c6c5"));
    EXPECT_TRUE(find_fn(black_moves, "c6c7"));
    EXPECT_TRUE(find_fn(black_moves, "c6d6"));
    EXPECT_TRUE(find_fn(black_moves, "c6b6"));
    EXPECT_TRUE(find_fn(black_moves, "c6d5"));
    EXPECT_TRUE(find_fn(black_moves, "c6d7"));
    EXPECT_TRUE(find_fn(black_moves, "c6b5"));
    EXPECT_TRUE(find_fn(black_moves, "c6b7"));
}

TEST_F(BitboardTests, CheckKingCastleMoves)
{
    std::string board_str(
        " r _ _ _ k _ _ _\n"
        " p _ _ _ _ _ _ _\n"
        " P _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ p\n"
        " _ _ _ _ _ _ _ P\n"
        " _ _ _ _ K _ _ R\n"
        "w Kq - 0 0\n"
    );

    BitBoard board = board_from_string_repr<BitBoard>(board_str);

    auto white_moves = board.get_all_legal_moves(PieceColour::WHITE);
    EXPECT_EQ(white_moves.size(), 8);

    EXPECT_TRUE(find_fn(white_moves, "e1f1"));
    EXPECT_TRUE(find_fn(white_moves, "e1f2"));
    EXPECT_TRUE(find_fn(white_moves, "e1e2"));
    EXPECT_TRUE(find_fn(white_moves, "e1d2"));
    EXPECT_TRUE(find_fn(white_moves, "e1d1"));
    EXPECT_TRUE(find_fn(white_moves, "e1g1")); // KS castle

    EXPECT_TRUE(find_fn(white_moves, "h1g1")); // rook moves
    EXPECT_TRUE(find_fn(white_moves, "h1f1"));

    auto black_moves = board.get_all_legal_moves(PieceColour::BLACK);
    EXPECT_EQ(black_moves.size(), 9);

    EXPECT_TRUE(find_fn(black_moves, "e8d8"));
    EXPECT_TRUE(find_fn(black_moves, "e8d7"));
    EXPECT_TRUE(find_fn(black_moves, "e8e7"));
    EXPECT_TRUE(find_fn(black_moves, "e8f7"));
    EXPECT_TRUE(find_fn(black_moves, "e8f8"));
    EXPECT_TRUE(find_fn(black_moves, "e8c8")); // QS castle

    EXPECT_TRUE(find_fn(black_moves, "a8b8")); // rook moves
    EXPECT_TRUE(find_fn(black_moves, "a8c8"));
    EXPECT_TRUE(find_fn(black_moves, "a8d8"));

}


TEST_F(BitboardTests, CheckSimpleBishopMoves)
{
    std::string board_str(
        " _ _ _ _ _ _ _ b\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ B _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        "w - - 0 0\n"
    );

    BitBoard board = board_from_string_repr<BitBoard>(board_str);

    auto white_moves = board.get_all_legal_moves(PieceColour::WHITE);
    EXPECT_EQ(white_moves.size(), 11);

    auto black_moves = board.get_all_legal_moves(PieceColour::BLACK);
    EXPECT_EQ(black_moves.size(), 7);
}

TEST_F(BitboardTests, CheckBlockedBishopMoves)
{
    std::string board_str(
        " _ _ _ _ _ b _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ p _ _ _ p\n"
        " _ _ _ P _ _ p P\n"
        " _ _ _ _ _ _ P _\n"
        " _ _ _ _ p B _ _\n"
        " _ _ _ _ P _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        "w - - 0 0\n"
    );

    BitBoard board = board_from_string_repr<BitBoard>(board_str);

    auto white_moves = board.get_all_legal_moves(PieceColour::WHITE);
    EXPECT_EQ(white_moves.size(), 3);
    EXPECT_TRUE(find_fn(white_moves, "f3g2"));
    EXPECT_TRUE(find_fn(white_moves, "f3h1"));
    EXPECT_TRUE(find_fn(white_moves, "f3e4"));

    auto black_moves = board.get_all_legal_moves(PieceColour::BLACK);
    EXPECT_EQ(black_moves.size(), 2);
    EXPECT_TRUE(find_fn(black_moves, "f8g7"));
    EXPECT_TRUE(find_fn(black_moves, "f8e7"));
}

TEST_F(BitboardTests, CheckSimpleRookMoves)
{
    std::string board_str(
        " _ _ _ _ _ _ _ r\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ R _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        "w - - 0 0\n"
    );

    BitBoard board = board_from_string_repr<BitBoard>(board_str);

    auto white_moves = board.get_all_legal_moves(PieceColour::WHITE);
    EXPECT_EQ(white_moves.size(), 14);

    auto black_moves = board.get_all_legal_moves(PieceColour::BLACK);
    EXPECT_EQ(black_moves.size(), 14);
}

TEST_F(BitboardTests, CheckBlockedRookMoves)
{
    std::string board_str(
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ r p _ _ _\n"
        " _ _ p R p _ _ _\n"
        " _ _ P _ P _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        "w - - 0 0\n"
    );

    BitBoard board = board_from_string_repr<BitBoard>(board_str);

    auto white_moves = board.get_all_legal_moves(PieceColour::WHITE);
    EXPECT_EQ(white_moves.size(), 6);
    EXPECT_TRUE(find_fn(white_moves, "d4d3"));
    EXPECT_TRUE(find_fn(white_moves, "d4d2"));
    EXPECT_TRUE(find_fn(white_moves, "d4d1"));
    EXPECT_TRUE(find_fn(white_moves, "d4d5"));
    EXPECT_TRUE(find_fn(white_moves, "d4c4"));
    EXPECT_TRUE(find_fn(white_moves, "d4e4"));

    auto black_moves = board.get_all_legal_moves(PieceColour::BLACK);
    EXPECT_EQ(black_moves.size(), 8);
    EXPECT_TRUE(find_fn(black_moves, "d5d6"));
    EXPECT_TRUE(find_fn(black_moves, "d5d7"));
    EXPECT_TRUE(find_fn(black_moves, "d5d8"));
    EXPECT_TRUE(find_fn(black_moves, "d5c5"));
    EXPECT_TRUE(find_fn(black_moves, "d5b5"));
    EXPECT_TRUE(find_fn(black_moves, "d5a5"));
    EXPECT_TRUE(find_fn(black_moves, "d5d4"));
    EXPECT_TRUE(find_fn(black_moves, "e5d4")); // pawn capture
}

TEST_F(BitboardTests, CheckSimpleQueenMoves)
{
    std::string board_str(
        " _ _ _ _ _ _ _ q\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ Q _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        "w - - 0 0\n"
    );

    BitBoard board = board_from_string_repr<BitBoard>(board_str);

    auto white_moves = board.get_all_legal_moves(PieceColour::WHITE);
    EXPECT_EQ(white_moves.size(), 25);

    auto black_moves = board.get_all_legal_moves(PieceColour::BLACK);
    EXPECT_EQ(black_moves.size(), 21);
}

TEST_F(BitboardTests, CheckBlockedQueenMoves)
{
    std::string board_str(
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ p _ _ _\n"
        " _ _ _ _ P _ _ _\n"
        " _ _ _ q P _ _ _\n"
        " _ _ p Q P _ _ _\n"
        " _ _ P _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        "w - - 0 0\n"
    );

    BitBoard board = board_from_string_repr<BitBoard>(board_str);

    auto white_moves = board.get_all_legal_moves(PieceColour::WHITE);
    EXPECT_EQ(white_moves.size(), 12);    
    EXPECT_TRUE(find_fn(white_moves, "d4d3"));
    EXPECT_TRUE(find_fn(white_moves, "d4d2"));
    EXPECT_TRUE(find_fn(white_moves, "d4d1"));
    EXPECT_TRUE(find_fn(white_moves, "d4d5"));
    EXPECT_TRUE(find_fn(white_moves, "d4c4"));
    EXPECT_TRUE(find_fn(white_moves, "d4e3"));
    EXPECT_TRUE(find_fn(white_moves, "d4f2"));
    EXPECT_TRUE(find_fn(white_moves, "d4g1"));
    EXPECT_TRUE(find_fn(white_moves, "d4c5"));
    EXPECT_TRUE(find_fn(white_moves, "d4b6"));
    EXPECT_TRUE(find_fn(white_moves, "d4a7"));
    EXPECT_TRUE(find_fn(white_moves, "e4d5")); // the pawn capture

    auto black_moves = board.get_all_legal_moves(PieceColour::BLACK);
    EXPECT_EQ(black_moves.size(), 13);
    EXPECT_TRUE(find_fn(black_moves, "d5d6"));
    EXPECT_TRUE(find_fn(black_moves, "d5d7"));
    EXPECT_TRUE(find_fn(black_moves, "d5d8"));
    EXPECT_TRUE(find_fn(black_moves, "d5c5"));
    EXPECT_TRUE(find_fn(black_moves, "d5b5"));
    EXPECT_TRUE(find_fn(black_moves, "d5a5"));
    EXPECT_TRUE(find_fn(black_moves, "d5d4"));
    EXPECT_TRUE(find_fn(black_moves, "d5c6"));
    EXPECT_TRUE(find_fn(black_moves, "d5b7"));
    EXPECT_TRUE(find_fn(black_moves, "d5a8"));
    EXPECT_TRUE(find_fn(black_moves, "d5e6"));
    EXPECT_TRUE(find_fn(black_moves, "d5e5"));
    EXPECT_TRUE(find_fn(black_moves, "d5e4"));
}

TEST_F(BitboardTests, CheckPawnMoves)
{
    std::string board_str(
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ p _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ p p _ _ _\n"
        " _ _ _ P _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ P _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        "w - - 0 0\n"
    );

    BitBoard board = board_from_string_repr<BitBoard>(board_str);

    auto white_moves = board.get_all_legal_moves(PieceColour::WHITE);
    EXPECT_EQ(white_moves.size(), 3);
    EXPECT_TRUE(find_fn(white_moves, "b2b3"));
    EXPECT_TRUE(find_fn(white_moves, "b2b4"));
    EXPECT_TRUE(find_fn(white_moves, "d4e5"));


    auto black_moves = board.get_all_legal_moves(PieceColour::BLACK);
    EXPECT_EQ(black_moves.size(), 4);
    EXPECT_TRUE(find_fn(black_moves, "e5e4"));
    EXPECT_TRUE(find_fn(black_moves, "e5d4"));
    EXPECT_TRUE(find_fn(black_moves, "g7g6"));
    EXPECT_TRUE(find_fn(black_moves, "g7g5"));
}

TEST_F(BitboardTests, CheckPawnEnPassantMoves)
{
    std::string board_str(
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ P p _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        "w - 5 0 0\n"
    );

    BitBoard board = board_from_string_repr<BitBoard>(board_str);

    auto white_moves = board.get_all_legal_moves(PieceColour::WHITE);
    EXPECT_EQ(white_moves.size(), 2);
    EXPECT_TRUE(find_fn(white_moves, "e5e6"));
    EXPECT_TRUE(find_fn(white_moves, "e5f6"));

    auto black_moves = board.get_all_legal_moves(PieceColour::BLACK);
    EXPECT_EQ(black_moves.size(), 1);
    EXPECT_TRUE(find_fn(black_moves, "f5f4"));
}

TEST_F(BitboardTests, CheckPawnEnPassantEdgeMoves)
{
    std::string board_str(
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ P p\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        "w - 6 0 0\n"
    );

    BitBoard board = board_from_string_repr<BitBoard>(board_str);

    auto white_moves = board.get_all_legal_moves(PieceColour::WHITE);
    EXPECT_EQ(white_moves.size(), 1);
    EXPECT_TRUE(find_fn(white_moves, "g4g5"));

    auto black_moves = board.get_all_legal_moves(PieceColour::BLACK);
    EXPECT_EQ(black_moves.size(), 2);
    EXPECT_TRUE(find_fn(black_moves, "h4h3"));
    EXPECT_TRUE(find_fn(black_moves, "h4g3"));
}

TEST_F(BitboardTests, CheckPawnPromotionMoves)
{
    std::string board_str(
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ P _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " p _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        "w - 6 0 0\n"
    );

    BitBoard board = board_from_string_repr<BitBoard>(board_str);

    auto white_moves = board.get_all_legal_moves(PieceColour::WHITE);
    EXPECT_EQ(white_moves.size(), 4);
    EXPECT_TRUE(find_fn(white_moves, "e7e8n"));
    EXPECT_TRUE(find_fn(white_moves, "e7e8b"));
    EXPECT_TRUE(find_fn(white_moves, "e7e8r"));
    EXPECT_TRUE(find_fn(white_moves, "e7e8q"));

    auto black_moves = board.get_all_legal_moves(PieceColour::BLACK);
    EXPECT_EQ(black_moves.size(), 4);
    EXPECT_TRUE(find_fn(black_moves, "a2a1n"));
    EXPECT_TRUE(find_fn(black_moves, "a2a1b"));
    EXPECT_TRUE(find_fn(black_moves, "a2a1r"));
    EXPECT_TRUE(find_fn(black_moves, "a2a1q"));
}

TEST_F(BitboardTests, CheckPinnedMoves)
{
    std::string board_str(
        " _ _ _ _ _ _ _ _\n"
        " _ b _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ p\n"
        " _ _ _ _ _ _ B P\n"
        " _ _ _ _ _ _ _ K\n"
        "w - 6 0 0\n"
    );

    BitBoard board = board_from_string_repr<BitBoard>(board_str);

    auto white_moves = board.get_all_legal_moves(PieceColour::WHITE);
    EXPECT_EQ(white_moves.size(), 6);
    EXPECT_TRUE(find_fn(white_moves, "h1g1")); // King move

    EXPECT_TRUE(find_fn(white_moves, "g2b7"));
    EXPECT_TRUE(find_fn(white_moves, "g2c6"));
    EXPECT_TRUE(find_fn(white_moves, "g2d5"));
    EXPECT_TRUE(find_fn(white_moves, "g2e4"));
    EXPECT_TRUE(find_fn(white_moves, "g2f3"));

}

TEST_F(BitboardTests, CheckKingCheckMoves)
{
    std::string board_str(
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ q _ _ _ _ _\n"
        " _ _ _ _ K _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        "w - 6 0 0\n"
    );

    BitBoard board = board_from_string_repr<BitBoard>(board_str);

    auto white_moves = board.get_all_legal_moves(PieceColour::WHITE);
    EXPECT_EQ(white_moves.size(), 3);
    EXPECT_TRUE(find_fn(white_moves, "e3f2"));
    EXPECT_TRUE(find_fn(white_moves, "e3f3"));
    EXPECT_TRUE(find_fn(white_moves, "e3d2"));
}