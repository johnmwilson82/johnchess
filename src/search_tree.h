#pragma once

#include "move.h"
#include "iboard.h"
#include "search_tree_node.h"

#include "utils/board_strings.h"


class SearchTree
{
private:
    std::unique_ptr<ZobristHash> hasher;
    //SearchTreeNode::hash_map_t nodes; 

    //std::shared_ptr<SearchTreeNode> root_node;
    BitBoard& m_board;
    float m_mult;

    float alpha_beta_max(float alpha, float beta, uint8_t depth_left);
    float alpha_beta_min(float alpha, float beta, uint8_t depth_left);

public:
    Move search(uint8_t search_depth, PieceColour ai_colour);

    SearchTree(BitBoard& board);
};