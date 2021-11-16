#include "board_tree.h"


std::shared_ptr<BoardTreeNode> BoardTreeNode::rebase(uint64_t new_root_hash, hash_map_t& hash_map)
{
    std::shared_ptr<BoardTreeNode> ret;

    for (auto& node : m_child_nodes)
    {
        for(auto& sub_node : node->m_child_nodes)
        {
            if(sub_node->m_hash == new_root_hash)
            {
                ret = sub_node;
            }
            else
            {
                sub_node->delete_node(hash_map);
            }
        }

        //node->delete_node(hash_map);
    }


    return ret;
}


std::shared_ptr<BoardTreeNode> BoardTreeNode::rebase(const Move& move, hash_map_t& hash_map, const ZobristHash& hasher)
{
    std::shared_ptr<BoardTreeNode> ret;

    Board new_root_board(m_board, move);

    uint64_t new_root_hash = hasher.get_hash(new_root_board);

    return rebase(new_root_hash, hash_map);
}


void BoardTreeNode::delete_node(hash_map_t& hash_map)
{
    for(auto& node : m_child_nodes)
    {
        node->delete_node(hash_map);
        node.reset();
    }
    hash_map.erase(m_hash);
}


std::shared_ptr<BoardTreeNode> BoardTreeNode::child_node(const Move& move, hash_map_t& hash_map, const ZobristHash& hasher)
{
    auto hash = hasher.get_hash(m_board, move);

    if(hash_map.contains(hash))
    {
        return hash_map.at(hash);
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

const ShannonHeuristic& BoardTreeNode::get_heuristic() const
{
    return m_heuristic;
}

// Root node
BoardTreeNode::BoardTreeNode(const Board& board, const ZobristHash& hasher) :
    m_prev_hash(std::nullopt),
    m_board(board),
    m_hash(hasher.get_hash(m_board)),
    m_heuristic(m_board)
{
    //std::cout << "hash = " << std::hex << m_hash << ", heuristic = " << std::dec << m_heuristic.get() << std::endl;
}


BoardTreeNode::BoardTreeNode(const BoardTreeNode& node, const Move& move, const ZobristHash& hasher) :
    m_prev_hash(node.get_hash()),
    m_board(node.m_board, move),
    m_hash(hasher.get_hash(m_board)),
    m_heuristic(m_board)
{
    //std::cout << "hash = " << std::hex << m_hash << ", heuristic = " << std::dec << m_heuristic.get() << ", move = " << move.to_string() << std::endl;
}