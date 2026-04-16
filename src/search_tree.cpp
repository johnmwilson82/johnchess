#include "search_tree.h"
#include <vector>
#include <limits>
#include <algorithm>

static int piece_value(PieceType pt)
{
    switch (pt) {
        case PieceType::PAWN:   return 1;
        case PieceType::KNIGHT: return 3;
        case PieceType::BISHOP: return 3;
        case PieceType::ROOK:   return 5;
        case PieceType::QUEEN:  return 9;
        case PieceType::KING:   return 200;
    }
    return 0;
}

static PieceType piece_at(const BitBoard& board, uint8_t sq)
{
    uint64_t bit = 1ULL << sq;
    if (board.get_pawns()   & bit) return PieceType::PAWN;
    if (board.get_knights() & bit) return PieceType::KNIGHT;
    if (board.get_bishops() & bit) return PieceType::BISHOP;
    if (board.get_rooks()   & bit) return PieceType::ROOK;
    if (board.get_queens()  & bit) return PieceType::QUEEN;
    return PieceType::KING;
}

static int move_score(const BitBoard& board, const Move& move)
{
    int score = 0;

    if (move.get_promotion_type().has_value())
        score += 800;

    auto captured = move.get_captured_piece_type();
    if (captured.has_value() || move.is_en_passant_capture()) {
        int victim   = captured.has_value() ? piece_value(*captured) : 1;
        int attacker = piece_value(piece_at(board, move.get_from_loc().get_raw()));
        score += victim * 10 - attacker;
    }

    return score;
}

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

    std::sort(move_list.begin(), move_list.end(), [&](const Move& a, const Move& b) {
        return move_score(m_board, a) > move_score(m_board, b);
    });

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

Move SearchTree::search(std::chrono::steady_clock::time_point deadline, PieceColour ai_colour)
{
    m_tt.clear();

    BitBoard::MoveList root_moves = m_board.get_all_legal_moves(m_board.get_colour_to_move());

    // No choice to make.
    if (root_moves.size() == 1)
        return root_moves.front();

    std::sort(root_moves.begin(), root_moves.end(), [&](const Move& a, const Move& b) {
        return move_score(m_board, a) > move_score(m_board, b);
    });

    std::unique_ptr<Move> best_move;
    int stable_count = 0;

    for (uint8_t depth = 1; depth <= MAX_DEPTH; ++depth)
    {
        float alpha = -std::numeric_limits<float>::infinity();
        float beta = std::numeric_limits<float>::infinity();
        std::unique_ptr<Move> iteration_best;

        for (const auto& move : root_moves)
        {
            m_board.make_move(move);
            float score = -negamax(-beta, -alpha, depth);
            m_board.unmake_move(move);

            if (score > alpha)
            {
                alpha = score;
                iteration_best = std::make_unique<Move>(move);
            }
        }

        // Track whether the best move changed this iteration.
        if (iteration_best) {
            bool same = best_move && (*iteration_best == *best_move);
            best_move = std::move(iteration_best);
            stable_count = same ? stable_count + 1 : 0;
        }

        // A forced mate was found — no deeper search can improve on this.
        if (alpha >= 150.0f)
            break;

        // Best move has been stable for 3 consecutive depths — confident enough to stop.
        if (depth >= 4 && stable_count >= 3)
            break;

        if (std::chrono::steady_clock::now() >= deadline)
            break;
    }

    return *best_move;
}


SearchTree::SearchTree(BitBoard& board) :
    hasher(std::make_unique<ZobristHash>()),
    m_board(board),
    m_mult(1.0)
{

}