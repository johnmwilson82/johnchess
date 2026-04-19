#include "search_tree.h"
#include <vector>
#include <limits>
#include <algorithm>
#include <thread>

static constexpr float ASPIRATION_WINDOW = 0.5f;

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

float SearchTree::quiescence(float alpha, float beta)
{
    ++m_nodes;

    float stand_pat = ShannonHeuristic(m_board, m_board.get_colour_to_move()).get();
    if (stand_pat >= beta) return beta;
    if (stand_pat > alpha) alpha = stand_pat;

    BitBoard::MoveList move_list = m_board.get_all_legal_moves(m_board.get_colour_to_move());

    std::sort(move_list.begin(), move_list.end(), [&](const Move& a, const Move& b) {
        return move_score(m_board, a) > move_score(m_board, b);
    });

    for (const auto& move : move_list)
    {
        if (!move.get_captured_piece_type().has_value() && !move.is_en_passant_capture())
            continue;

        m_board.make_move(move);
        float score = -quiescence(-beta, -alpha);
        m_board.unmake_move(move);

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }

    return alpha;
}

float SearchTree::negamax(float alpha, float beta, uint8_t depth_left, bool null_move_ok, uint8_t ply)
{
    ++m_nodes;
    uint64_t hash = hasher->get_hash(m_board);

    const auto& e = m_tt[hash & (TT_SIZE - 1)];
    if (e.flag != TTEntry::Flag::EMPTY && e.key == hash && e.depth >= depth_left) {
        if (e.flag == TTEntry::Flag::EXACT)                          return e.score;
        if (e.flag == TTEntry::Flag::LOWER_BOUND && e.score > alpha) alpha = e.score;
        if (e.flag == TTEntry::Flag::UPPER_BOUND && e.score < beta)  beta  = e.score;
        if (alpha >= beta) return e.score;
    }

    if (depth_left == 0)
        return quiescence(alpha, beta);

    BitBoard::MoveList move_list = m_board.get_all_legal_moves(m_board.get_colour_to_move());

    if (move_list.empty())
    {
        return m_board.get_in_check(m_board.get_colour_to_move()) ? -200.f : 0.f;
    }

    bool in_check = m_board.get_in_check(m_board.get_colour_to_move());

    // Null move pruning: skip our turn and see if the opponent can still beat beta.
    // Skip when in check (illegal) or in pawn-only positions (risk of zugzwang).
    static constexpr int NULL_MOVE_REDUCTION = 2;
    if (null_move_ok && !in_check && depth_left >= NULL_MOVE_REDUCTION + 1)
    {
        PieceColour us = m_board.get_colour_to_move();
        uint64_t our_pieces = m_board.pieces_to_move(us == PieceColour::WHITE);
        bool has_non_pawn_material = our_pieces & (m_board.get_queens() | m_board.get_rooks()
                                                 | m_board.get_bishops() | m_board.get_knights());
        if (has_non_pawn_material)
        {
            auto saved_ep = m_board.get_enpassant_column();
            m_board.set_enpassant_column(std::nullopt);
            m_board.set_colour_to_move(us == PieceColour::WHITE ? PieceColour::BLACK : PieceColour::WHITE);

            float null_score = -negamax(-beta, -beta + 1.f, depth_left - NULL_MOVE_REDUCTION - 1, false, ply + 1);

            m_board.set_colour_to_move(us);
            m_board.set_enpassant_column(saved_ep);

            if (null_score >= beta)
                return beta;
        }
    }

    const auto& killers = m_killers[ply];
    auto score_move = [&](const Move& m) {
        int s = move_score(m_board, m);
        if (!m.get_captured_piece_type().has_value() && !m.is_en_passant_capture()) {
            if (m == killers[0])      s += 9;
            else if (m == killers[1]) s += 8;
        }
        return s;
    };
    std::sort(move_list.begin(), move_list.end(), [&](const Move& a, const Move& b) {
        return score_move(a) > score_move(b);
    });

    float original_alpha = alpha;
    Move best_move;

    for (const auto& move : move_list)
    {
        m_board.make_move(move);
        float score = -negamax(-beta, -alpha, depth_left - 1, true, ply + 1);
        m_board.unmake_move(move);

        if (score >= beta)
        {
            if (!move.get_captured_piece_type().has_value() && !move.is_en_passant_capture()
                && !move.get_promotion_type().has_value())
            {
                if (m_killers[ply][0] != move) {
                    m_killers[ply][1] = m_killers[ply][0];
                    m_killers[ply][0] = move;
                }
            }
            m_tt[hash & (TT_SIZE - 1)] = { hash, move, beta, depth_left, TTEntry::Flag::LOWER_BOUND };
            return beta;
        }
        if (score > alpha)
        {
            alpha = score;
            best_move = move;
        }
    }

    TTEntry::Flag flag = (alpha <= original_alpha) ? TTEntry::Flag::UPPER_BOUND : TTEntry::Flag::EXACT;
    m_tt[hash & (TT_SIZE - 1)] = { hash, best_move, alpha, depth_left, flag };

    return alpha;
}

std::string SearchTree::extract_principal_variation(const Move& first_move, int depth) const
{
    std::string line = first_move.to_string();
    BitBoard board(m_board);
    board.make_move(first_move);

    for (int i = 1; i < depth; ++i)
    {
        uint64_t hash = hasher->get_hash(board);
        const auto& e = m_tt[hash & (TT_SIZE - 1)];
        if (e.flag == TTEntry::Flag::EMPTY || e.key != hash || !e.best_move.is_valid())
            break;

        line += ' ';
        line += e.best_move.to_string();
        board.make_move(e.best_move);
    }

    return line;
}

void SearchTree::run_worker(std::chrono::steady_clock::time_point deadline)
{
    BitBoard::MoveList root_moves = m_board.get_all_legal_moves(m_board.get_colour_to_move());
    if (root_moves.size() <= 1) return;

    std::sort(root_moves.begin(), root_moves.end(), [&](const Move& a, const Move& b) {
        return move_score(m_board, a) > move_score(m_board, b);
    });

    float prev_score = 0.0f;
    const float INF  = std::numeric_limits<float>::infinity();

    for (uint8_t depth = 1; depth <= MAX_DEPTH; ++depth)
    {
        float delta = (depth <= 2) ? INF : ASPIRATION_WINDOW;
        float alpha = (depth <= 2) ? -INF : prev_score - delta;
        float beta  = (depth <= 2) ?  INF : prev_score + delta;

        while (true)
        {
            float cur_alpha  = alpha;
            float best_score = -INF;

            for (const auto& move : root_moves)
            {
                if (std::chrono::steady_clock::now() >= deadline) return;
                m_board.make_move(move);
                float score = -negamax(-beta, -cur_alpha, depth, true, 1);
                m_board.unmake_move(move);
                if (score > best_score) best_score = score;
                if (score > cur_alpha)  cur_alpha   = score;
            }

            if (best_score <= alpha)
            {
                delta = (delta >= INF) ? INF : delta * 2;
                alpha = (delta >= INF) ? -INF : best_score - delta;
            }
            else if (best_score >= beta)
            {
                delta = (delta >= INF) ? INF : delta * 2;
                beta  = (delta >= INF) ?  INF : best_score + delta;
            }
            else
            {
                prev_score = best_score;
                break;
            }

            if (delta > 4.0f) { alpha = -INF; beta = INF; delta = INF; }
        }

        if (std::chrono::steady_clock::now() >= deadline) return;
    }
}

Move SearchTree::search(std::chrono::steady_clock::time_point deadline, PieceColour ai_colour,
                        ThinkCallback think_cb)
{
    m_nodes = 0;
    m_killers = {};
    auto search_start = std::chrono::steady_clock::now();

    BitBoard::MoveList root_moves = m_board.get_all_legal_moves(m_board.get_colour_to_move());

    // No choice to make.
    if (root_moves.size() == 1)
        return root_moves.front();

    std::sort(root_moves.begin(), root_moves.end(), [&](const Move& a, const Move& b) {
        return move_score(m_board, a) > move_score(m_board, b);
    });

    unsigned n_threads = std::max(1u, std::thread::hardware_concurrency());
    std::vector<std::thread> workers;
    workers.reserve(n_threads - 1);
    for (unsigned t = 1; t < n_threads; ++t) {
        workers.emplace_back([this, deadline, board_snapshot = BitBoard(m_board)]() mutable {
            SearchTree worker(board_snapshot, m_tt);
            worker.run_worker(deadline);
        });
    }

    std::unique_ptr<Move> best_move;
    int stable_count = 0;
    float prev_score = 0.0f;


    const float INF = std::numeric_limits<float>::infinity();

    for (uint8_t depth = 1; depth <= MAX_DEPTH; ++depth)
    {
        // Use full window for the first two depths to seed prev_score reliably.
        float delta = (depth <= 2) ? INF : ASPIRATION_WINDOW;
        float alpha = (depth <= 2) ? -INF : prev_score - delta;
        float beta  = (depth <= 2) ?  INF : prev_score + delta;

        std::unique_ptr<Move> iteration_best;
        float best_score = -INF;
        bool timed_out = false;

        while (true)
        {
            float cur_alpha = alpha;
            float window_score = -INF;
            std::unique_ptr<Move> window_best;

            for (const auto& move : root_moves)
            {
                if (std::chrono::steady_clock::now() >= deadline) { timed_out = true; break; }
                m_board.make_move(move);
                float score = -negamax(-beta, -cur_alpha, depth, true, 1);
                m_board.unmake_move(move);

                if (score > window_score)
                {
                    window_score = score;
                    window_best  = std::make_unique<Move>(move);
                }
                if (score > cur_alpha) cur_alpha = score;
            }

            if (timed_out) break;

            if (window_score <= alpha)
            {
                // Fail low: widen downward.
                delta = (delta >= INF) ? INF : delta * 2;
                alpha = (delta >= INF) ? -INF : window_score - delta;
                beta  = (delta >= INF) ?  INF : beta;
            }
            else if (window_score >= beta)
            {
                // Fail high: widen upward.
                delta = (delta >= INF) ? INF : delta * 2;
                beta  = (delta >= INF) ?  INF : window_score + delta;
            }
            else
            {
                // Score is within the window — accept.
                iteration_best = std::move(window_best);
                best_score     = window_score;
                break;
            }

            // After a few doublings fall back to a full-window re-search.
            if (delta > 4.0f) { alpha = -INF; beta = INF; delta = INF; }
        }

        if (timed_out) break;

        prev_score = best_score;

        // Track whether the best move changed this iteration.
        if (iteration_best) {
            bool same = best_move && (*iteration_best == *best_move);
            best_move = std::move(iteration_best);
            stable_count = same ? stable_count + 1 : 0;
        }

        if (think_cb && best_move) {
            int score_cp = static_cast<int>(best_score * 100.0f);
            int elapsed_cs = static_cast<int>(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - search_start).count() / 10);
            think_cb(depth, score_cp, elapsed_cs, m_nodes, extract_principal_variation(*best_move, depth + 1));
        }

        // A forced mate was found — no deeper search can improve on this.
        if (best_score >= 150.0f)
            break;

        // Best move has been stable for 3 consecutive depths — confident enough to stop.
        if (depth >= 4 && stable_count >= 3)
            break;

        if (std::chrono::steady_clock::now() >= deadline)
            break;
    }

    for (auto& w : workers)
        w.join();

    return *best_move;
}


SearchTree::SearchTree(BitBoard& board) :
    hasher(std::make_unique<ZobristHash>()),
    m_tt_storage(std::make_unique<std::array<TTEntry, TT_SIZE>>()),
    m_tt(*m_tt_storage),
    m_board(board),
    m_mult(1.0)
{
}

SearchTree::SearchTree(BitBoard& board, std::array<TTEntry, TT_SIZE>& shared_tt) :
    hasher(std::make_unique<ZobristHash>()),
    m_tt_storage(nullptr),
    m_tt(shared_tt),
    m_board(board),
    m_mult(1.0)
{
}