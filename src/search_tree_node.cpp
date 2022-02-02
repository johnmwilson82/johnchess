#include <ranges>
#include <algorithm>

#include "search_tree_node.h"

#if(0)

std::shared_ptr<SearchTreeNode> SearchTreeNode::child_node(const Move& move, hash_map_t& hash_map, const ZobristHash& hasher)
{
    auto hash = hasher.get_hash(*m_board, move);

    if (hash_map.contains(hash))
    {
        auto btn = hash_map.at(hash).lock();

        if (btn->get_ply() > m_ply)
        {
            m_child_nodes.emplace_back(btn, move);
        }

        return btn;
    }
    else
    {
        auto btn = std::make_shared<SearchTreeNode>(*this, move, hasher);

        hash_map.emplace(btn->m_hash, btn);

        m_child_nodes.emplace_back(btn, move);

        return btn;
    }
}


const IBoard& SearchTreeNode::get_board() const
{
    return *m_board;
}


uint64_t SearchTreeNode::get_hash() const
{
    return m_hash;
}



const double SearchTreeNode::get_score(PieceColour ai_col, PieceColour colour_to_move) const
{
    double return_multiplier = ai_col == PieceColour::WHITE ? 1.0 : -1.0;

    if (m_child_nodes.empty())
    {
        if (m_check_for_mate)
        {
            switch (m_board->get_mate(colour_to_move))
            {
            case IBoard::CHECKMATE:
                return 200;

            case IBoard::STALEMATE:
                return 0;

            default:
                break;
            }
        }

        return m_heuristic.get() * return_multiplier;

    }

    auto child_scores = m_child_nodes |
        std::views::transform([&](edge_t edge) { return edge.first->get_score(ai_col, opposite_colour(colour_to_move)); });

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
SearchTreeNode::SearchTreeNode(const IBoard& board, const ZobristHash& hasher) :
    m_board(board.clone()),
    m_hash(hasher.get_hash(*m_board)),
    m_heuristic(*m_board),
    m_ply(0)
{
}


SearchTreeNode::SearchTreeNode(const SearchTreeNode& node, const Move& move, const ZobristHash& hasher) :
    m_board(node.m_board->clone_moved(move)),
    m_hash(hasher.get_hash(*m_board)),
    m_heuristic(*m_board),
    m_ply(node.m_ply + 1)
{
}

#endif