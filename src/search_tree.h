#pragma once

#include "move.h"
#include "bitboards/bitboard.h"
#include "search_tree_node.h"

#include "utils/board_strings.h"
#include <unordered_map>
#include <chrono>
#include <functional>


struct TTEntry {
    enum class Flag : uint8_t { EXACT, LOWER_BOUND, UPPER_BOUND };
    float score;
    uint8_t depth;
    Flag flag;
};


// Called after each completed depth: depth, score in centipawns, elapsed centiseconds, nodes, best move.
using ThinkCallback = std::function<void(uint8_t, int, int, uint64_t, const Move&)>;

class SearchTree
{
private:
    std::unique_ptr<ZobristHash> hasher;
    std::unordered_map<uint64_t, TTEntry> m_tt;

    BitBoard& m_board;
    float m_mult;
    uint64_t m_nodes = 0;

    float negamax(float alpha, float beta, uint8_t depth_left);

    float maxi(int depth, PieceColour ai_colour);
    float mini(int depth, PieceColour ai_colour);

    static constexpr uint8_t MAX_DEPTH = 20;

public:
    Move search(std::chrono::steady_clock::time_point deadline, PieceColour ai_colour,
                ThinkCallback think_cb = nullptr);

    SearchTree(BitBoard& board);
};