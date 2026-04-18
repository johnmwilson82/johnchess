#pragma once

#include "move.h"
#include "bitboards/bitboard.h"
#include "search_tree_node.h"

#include "utils/board_strings.h"
#include <array>
#include <chrono>
#include <functional>
#include <thread>


struct TTEntry {
    enum class Flag : uint8_t { EMPTY, EXACT, LOWER_BOUND, UPPER_BOUND };
    uint64_t key = 0;
    Move best_move;
    float score = 0.f;
    uint8_t depth = 0;
    Flag flag = Flag::EMPTY;
};


// Called after each completed depth: depth, score in centipawns, elapsed centiseconds, nodes, pv string.
using ThinkCallback = std::function<void(uint8_t, int, int, uint64_t, const std::string&)>;

class SearchTree
{
private:
    static constexpr size_t TT_SIZE = 1 << 20; // ~16 MB
    static constexpr uint8_t MAX_DEPTH = 20;

    std::unique_ptr<ZobristHash> hasher;
    std::unique_ptr<std::array<TTEntry, TT_SIZE>> m_tt_storage;
    std::array<TTEntry, TT_SIZE>& m_tt;

    BitBoard& m_board;
    float m_mult;
    uint64_t m_nodes = 0;

    std::array<std::array<Move, 2>, MAX_DEPTH + 1> m_killers{};

    float quiescence(float alpha, float beta);
    float negamax(float alpha, float beta, uint8_t depth_left, bool null_move_ok = true, uint8_t ply = 0);
    void run_worker(std::chrono::steady_clock::time_point deadline);
    std::string extract_principal_variation(const Move& first_move, int depth) const;

    SearchTree(BitBoard& board, std::array<TTEntry, TT_SIZE>& shared_tt);

public:
    Move search(std::chrono::steady_clock::time_point deadline, PieceColour ai_colour,
                ThinkCallback think_cb = nullptr);

    SearchTree(BitBoard& board);
};