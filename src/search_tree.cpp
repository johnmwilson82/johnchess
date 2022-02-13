#include "search_tree.h"
#include <vector>
#include <future>

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

static float alpha_beta_min(BitBoard& board, float alpha, float beta, uint8_t depth_left, PieceColour ai_colour);

static float alpha_beta_max(BitBoard& board, float alpha, float beta, uint8_t depth_left, PieceColour ai_colour)
{
    if (depth_left == 0)
    {
        ShannonHeuristic eval(board, ai_colour);
        return eval.get();
    }

    BitBoard::MoveList move_list = board.get_all_legal_moves(board.get_colour_to_move());

    if (move_list.empty())
    {
        return (board.get_in_check(board.get_colour_to_move()) ? (board.get_colour_to_move() == ai_colour ? -200.f : 200.f) : 0);
    }

    for (const auto& move : move_list)
    {
        board.make_move(move);
        float score = alpha_beta_min(board, alpha, beta, depth_left - 1, ai_colour);
        board.unmake_move(move);

        if (score >= beta)
        {
            return beta;
        }
        if (score > alpha)
        {
            alpha = score;
        }
    }

    return alpha;
}

static float alpha_beta_min(BitBoard& board, float alpha, float beta, uint8_t depth_left, PieceColour ai_colour)
{
    if (depth_left == 0)
    {
        ShannonHeuristic eval(board, ai_colour);
        return eval.get();
    }

    BitBoard::MoveList move_list = board.get_all_legal_moves(board.get_colour_to_move());

    if (move_list.empty())
    {
        return (board.get_in_check(board.get_colour_to_move()) ? (board.get_colour_to_move() == ai_colour ? -200.f : 200.f) : 0);
    }

    for (const auto& move : move_list)
    {
        board.make_move(move);
        float score = alpha_beta_max(board, alpha, beta, depth_left - 1, ai_colour);
        board.unmake_move(move);

        if (score <= alpha)
        {
            return alpha;
        }
        if (score < beta)
        {
            beta = score;
        }
    }

    return beta;
}

Move SearchTree::search(uint8_t search_depth, PieceColour ai_colour)
{
    m_mult = ai_colour == PieceColour::WHITE ? 1.0f : -1.0f;

    std::unique_ptr<Move> best_move;

    BitBoard::MoveList move_list = m_board.get_all_legal_moves(m_board.get_colour_to_move());

    std::atomic<float> alpha = -std::numeric_limits<float>::infinity();
    float beta = std::numeric_limits<float>::infinity();

    std::vector<std::future<std::pair<float, std::unique_ptr<Move>>>> futures;

    for (const auto& move : move_list)
    {
        futures.push_back(std::async(std::launch::async, [&, move] {
            BitBoard board(m_board);
            board.make_move(move);
            //float score = mini(search_depth, ai_colour);
            float score = alpha_beta_min(board, alpha, beta, search_depth, ai_colour);
            board.unmake_move(move);

            if (score > alpha)
            {
                alpha = score;
                best_move = std::make_unique<Move>(move);
            }

            return std::make_pair(score, std::make_unique<Move>(move));
            })
        );
    }

    for (auto& future : futures)
    {
        auto [score, move] = future.get();
        if (score == alpha)
        {
            //alpha = score;
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