#include "search_tree.h"
#include <vector>
#include <limits>

float SearchTree::maxi(int depth, PieceColour ai_colour) {
    if (depth == 0) 
    {
        ShannonHeuristic eval(m_board, ai_colour);
        return eval.get();
    }

    BitBoard::MoveList move_list = m_board.get_all_legal_moves(m_board.get_colour_to_move());

    if (move_list.empty())
    {
        return (m_board.get_in_check(m_board.get_colour_to_move()) ? (m_board.get_colour_to_move() == ai_colour ? -200.f : 200.f) : 0);
    }

    float max = -std::numeric_limits<float>::infinity();

    for (const auto& move : move_list)
    {
        m_board.make_move(move);
        float score = mini(depth - 1, ai_colour);
        m_board.unmake_move(move);

        if (score > max)
            max = score;
    }

    return max;
}

float SearchTree::mini(int depth, PieceColour ai_colour) {
    if (depth == 0)
    {
        ShannonHeuristic eval(m_board, ai_colour);
        return -eval.get();
    }

    BitBoard::MoveList move_list = m_board.get_all_legal_moves(m_board.get_colour_to_move());

    if (move_list.empty())
    {
        return (m_board.get_in_check(m_board.get_colour_to_move()) ? (m_board.get_colour_to_move() == ai_colour ? -200.f : 200.f) : 0);
    }

    float min = std::numeric_limits<float>::infinity();

    for (const auto& move : move_list)
    {
        m_board.make_move(move);
        float score = maxi(depth - 1, ai_colour);
        m_board.unmake_move(move);

        if (score < min)
            min = score;
    }

    return min;
}

float SearchTree::negamax(float alpha, float beta, uint8_t depth_left)
{
    uint64_t hash = hasher->get_hash(m_board);

    auto it = m_tt.find(hash);
    if (it != m_tt.end() && it->second.depth >= depth_left) {
        const auto& e = it->second;
        if (e.flag == TTEntry::Flag::EXACT)                          return e.score;
        if (e.flag == TTEntry::Flag::LOWER_BOUND && e.score > alpha) alpha = e.score;
        if (e.flag == TTEntry::Flag::UPPER_BOUND && e.score < beta)  beta  = e.score;
        if (alpha >= beta) return e.score;
    }

    if (depth_left == 0)
    {
        ShannonHeuristic eval(m_board, m_board.get_colour_to_move());
        return eval.get();
    }

    BitBoard::MoveList move_list = m_board.get_all_legal_moves(m_board.get_colour_to_move());

    if (move_list.empty())
    {
        return m_board.get_in_check(m_board.get_colour_to_move()) ? -200.f : 0.f;
    }

    float original_alpha = alpha;

    for (const auto& move : move_list)
    {
        m_board.make_move(move);
        float score = -negamax(-beta, -alpha, depth_left - 1);
        m_board.unmake_move(move);

        if (score >= beta)
        {
            m_tt[hash] = { beta, depth_left, TTEntry::Flag::LOWER_BOUND };
            return beta;
        }
        if (score > alpha)
            alpha = score;
    }

    TTEntry::Flag flag = (alpha <= original_alpha) ? TTEntry::Flag::UPPER_BOUND : TTEntry::Flag::EXACT;
    m_tt[hash] = { alpha, depth_left, flag };

    return alpha;
}

Move SearchTree::search(uint8_t search_depth, PieceColour ai_colour)
{
    m_tt.clear();

    std::unique_ptr<Move> best_move;

    BitBoard::MoveList move_list = m_board.get_all_legal_moves(m_board.get_colour_to_move());

    float alpha = -std::numeric_limits<float>::infinity();
    float beta = std::numeric_limits<float>::infinity();

    for (const auto& move : move_list)
    {
        m_board.make_move(move);
        float score = -negamax(-beta, -alpha, search_depth);
        m_board.unmake_move(move);

        if (score > alpha)
        {
            alpha = score;
            best_move = std::make_unique<Move>(move);
        }
    }

    return *best_move;
}


SearchTree::SearchTree(BitBoard& board) :
    hasher(std::make_unique<ZobristHash>()),
    m_board(board),
    m_mult(1.0)
{

}