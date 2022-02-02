#pragma once

#include "move.h"
#include "bitboards/bitboard.h"
#include "board_tree_node.h"

#include "utils/board_strings.h"


class BoardTree
{
private:
    std::unique_ptr<ZobristHash> hasher;
    BoardTreeNode::hash_map_t nodes; 

    std::shared_ptr<BoardTreeNode> root_node;

    bool prune_except_hash(uint64_t base_hash, uint64_t except_hash);
    void rebase(uint64_t new_hash);

public:
    void set_new_root_from_move(const Move& move);
    void set_new_root_from_board(const BitBoard& board);
    void populate_tree(uint8_t search_depth, std::list<std::shared_ptr<BoardTreeNode>> btns, PieceColour colour_to_move);
    Move search(uint8_t search_depth, PieceColour ai_colour);

    BoardTree(const BitBoard& board);
};