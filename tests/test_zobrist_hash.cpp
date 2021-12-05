#include "gtest/gtest.h"

#define private public
#include <zobrist_hash.h>
#include <utils\board_strings.h>

using namespace utils;

class ZobristHashTests : public ::testing::Test 
{
protected:
    std::unique_ptr<ZobristHash> hasher;

    void SetUp()
    {
        hasher = std::make_unique<ZobristHash>();
    }
};

TEST_F(ZobristHashTests, CheckHashWithRemovedPawn) 
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

    std::string board_str_no_e4(
        " r n b q k b _ r\n"
        " p p p p _ Q _ p\n"
        " _ _ _ _ _ _ p _\n"
        " _ _ _ _ p _ _ _\n"
        " _ _ B _ _ _ n _\n"
        " _ _ _ P _ _ _ _\n"
        " P P P _ _ P P P\n"
        " R N B _ K _ N R\n"
    );

    Board board_no_e4 = board_from_string_repr(board_str_no_e4);
    
    Board board_only_e4;

    board_only_e4.add_piece<Pawn>(Piece::WHITE, "e4");

    auto board_hash = hasher->get_hash(board);

    auto board_hash_no_e4 = hasher->get_hash(board_no_e4);

    auto board_hash_only_e4 = hasher->get_hash(board_only_e4);

    EXPECT_NE(board_hash, board_hash_no_e4);
    EXPECT_NE(board_hash, board_hash_only_e4);
    EXPECT_NE(board_hash_no_e4, board_hash_only_e4);

    EXPECT_EQ(board_hash_only_e4 ^ board_hash_no_e4, board_hash);
}


TEST_F(ZobristHashTests, CheckHashThroughCastling)
{
    std::string board_str_pre_castle(
        " r n b q k b _ r\n"
        " p p p _ p p p p\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ p _ _ _ _\n"
        " _ _ _ _ n _ _ _\n"
        " _ _ _ B P N _ _\n"
        " P P P P _ P P P\n"
        " R N B Q K _ _ R\n"
    );

    Board pre_castle_board = board_from_string_repr(board_str_pre_castle);

    std::string board_str_post_castle(
        " r n b q k b _ r\n"
        " p p p _ p p p p\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ _ p _ _ _ _\n"
        " _ _ _ _ n _ _ _\n"
        " _ _ _ B P N _ _\n"
        " P P P P _ P P P\n"
        " R N B Q _ R K _\n"
    );

    Board post_castle_board = board_from_string_repr(board_str_post_castle);
    post_castle_board.square(5, 0).get_piece()->set_has_moved(true);
    post_castle_board.square(6, 0).get_piece()->set_has_moved(true);

    Move castle_move(pre_castle_board, "e1g1");

    Board new_board(pre_castle_board, castle_move);

    auto expected_hash = hasher->get_hash(post_castle_board);

    auto new_hash = hasher->get_hash(new_board);

    EXPECT_EQ(expected_hash, new_hash);

}