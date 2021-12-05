#include "board_tree.h"
#include <vector>
#include <ranges>
#include <algorithm>


std::shared_ptr<BoardTreeNode> BoardTreeNode::child_node(const Move& move, hash_map_t& hash_map, const ZobristHash& hasher)
{
    auto hash = hasher.get_hash(m_board, move);

    if(hash_map.contains(hash))
    {
        auto prev_btn = hash_map.at(hash);

        auto btn = std::make_shared<BoardTreeNode>(*prev_btn, move);

        m_child_nodes.push_back(btn);

        return btn;
    }
    else
    {
        auto btn = std::make_shared<BoardTreeNode>(*this, move, hasher);

        hash_map.emplace(btn->m_hash, btn);

        m_child_nodes.push_back(btn);

        return btn;
    }
}


const Board& BoardTreeNode::get_board() const
{
    return m_board;
}


uint64_t BoardTreeNode::get_hash() const
{
    return m_hash;
}



const double BoardTreeNode::get_score(Piece::Colour ai_col, Piece::Colour colour_to_move) const
{
    double return_multiplier = ai_col == Piece::WHITE ? 1.0 : -1.0;

    if (m_child_nodes.empty())
    {
        if (m_check_for_mate)
        {
            switch (m_board.get_mate(colour_to_move))
            {
            case Board::CHECKMATE:
                return 200;

            case Board::STALEMATE:
                return 0;

            default:
                break;
            }
        }

        return m_heuristic.get() * return_multiplier;

    }

    auto child_scores = m_child_nodes |
        std::views::transform([&](std::shared_ptr<BoardTreeNode> node) { return node->get_score(ai_col, Piece::opposite_colour(colour_to_move)); });

    if (ai_col == colour_to_move)
    {
        return *std::ranges::max_element(child_scores);
    }
    else
    {
        return *std::ranges::min_element(child_scores);
    }
}

// Root node
BoardTreeNode::BoardTreeNode(const Board& board, const ZobristHash& hasher) :
    m_prev_hash(std::nullopt),
    m_board(board),
    m_hash(hasher.get_hash(m_board)),
    m_heuristic(m_board),
    m_move(std::nullopt)
{
    //std::cout << "hash = " << std::hex << m_hash << ", heuristic = " << std::dec << m_heuristic.get() << std::endl;
}


BoardTreeNode::BoardTreeNode(const BoardTreeNode& node, const Move& move, const ZobristHash& hasher) :
    m_prev_hash(node.get_hash()),
    m_board(node.m_board, move),
    m_hash(hasher.get_hash(m_board)),
    m_heuristic(m_board),
    m_move(move)
{
}

// Version for copying from existing nodes, with new move
BoardTreeNode::BoardTreeNode(const BoardTreeNode& node, const Move& move) :
    m_prev_hash(node.get_hash()),
    m_board(node.m_board),
    m_hash(node.m_hash),
    m_heuristic(node.m_heuristic),
    m_move(move)
{
}