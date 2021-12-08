#pragma once

#include <unordered_map>

#include "move.h"
#include "board.h"
#include "zobrist_hash.h"
#include "heuristic.h"

class BoardTreeNode
{
public:
    using hash_map_t = std::unordered_map<uint64_t, std::weak_ptr<BoardTreeNode>>;
    using edge_t = std::pair<std::shared_ptr<BoardTreeNode>, Move>;

private:
    const Board m_board;
    const uint64_t m_hash;

    const ShannonHeuristic m_heuristic;

    std::list<edge_t> m_child_nodes;
    bool m_check_for_mate = false;

    const uint32_t m_ply;

public:
    std::shared_ptr<BoardTreeNode> child_node(const Move& move, hash_map_t& hash_map, const ZobristHash& hasher);

    std::list<edge_t>& get_children() { return m_child_nodes; };

    const Board& get_board() const;

    uint64_t get_hash() const;

    uint32_t get_ply() const { return m_ply; }

    const double get_score(Piece::Colour ai_col, Piece::Colour colour_to_move) const;

    void set_check_for_mate() { m_check_for_mate = true; }

    BoardTreeNode(const Board& board, const ZobristHash& hasher);
    BoardTreeNode(const BoardTreeNode& node, const Move& move, const ZobristHash& hasher);
};