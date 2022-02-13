#pragma once

#include "move.h"
#include "bitboards/bitboard.h"
#include "search_tree_node.h"

#include "utils/board_strings.h"


class SearchTree
{
private:
    std::unique_ptr<ZobristHash> hasher;

    BitBoard& m_board;
    float m_mult;

    float maxi(int depth, PieceColour ai_colour);
    float mini(int depth, PieceColour ai_colour);

public:
    Move search(uint8_t search_depth, PieceColour ai_colour);

    SearchTree(BitBoard& board);
};