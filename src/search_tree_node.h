#pragma once

#include <unordered_map>

#include "move.h"
#include "bitboards/bitboard.h"
#include "zobrist_hash.h"
#include "heuristic.h"

#if(0)
class SearchTreeNode
{
public:
    using hash_map_t = std::unordered_map<uint64_t, std::weak_ptr<SearchTreeNode>>;
    using edge_t = std::pair<std::shared_ptr<SearchTreeNode>, Move>;

private:
    const std::unique_ptr<BitBoard> m_board;
    const uint64_t m_hash;

    const ShannonHeuristic m_heuristic;

    std::list<edge_t> m_child_nodes;
    bool m_check_for_mate = false;

    const uint32_t m_ply;


public:
    std::shared_ptr<SearchTreeNode> child_node(const Move& move, hash_map_t& hash_map, const ZobristHash& hasher);

    std::list<edge_t>& get_children() { return m_child_nodes; };

    const BitBoard& get_board() const;

    uint64_t get_hash() const;

    uint32_t get_ply() const { return m_ply; }

    const double get_score(PieceColour ai_col, PieceColour colour_to_move) const;

    void set_check_for_mate() { m_check_for_mate = true; }

    SearchTreeNode(const BitBoard& board, const ZobristHash& hasher);
    SearchTreeNode(const SearchTreeNode& node, const Move& move, const ZobristHash& hasher);
};
#endif