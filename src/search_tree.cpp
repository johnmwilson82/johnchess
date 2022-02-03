#include "search_tree.h"
#include <vector>

float SearchTree::maxi(int depth, PieceColour ai_colour) {
    if (depth == 0) 
    {
        ShannonHeuristic eval(m_board, ai_colour);
        return eval.get();
    }

    IBoard::MoveList move_list = m_board.get_all_legal_moves(m_board.get_colour_to_move());

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

    IBoard::MoveList move_list = m_board.get_all_legal_moves(m_board.get_colour_to_move());

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

float SearchTree::alpha_beta_max(float alpha, float beta, uint8_t depth_left, PieceColour ai_colour)
{
    if (depth_left == 0)
    {
        ShannonHeuristic eval(m_board, ai_colour);
        return eval.get();
    }

    IBoard::MoveList move_list = m_board.get_all_legal_moves(m_board.get_colour_to_move());

    if (move_list.empty())
    {
        return (m_board.get_in_check(m_board.get_colour_to_move()) ? (m_board.get_colour_to_move() == ai_colour ? -200.f : 200.f) : 0);
    }

    for (const auto& move : move_list)
    {
        m_board.make_move(move);
        float score = alpha_beta_min(alpha, beta, depth_left - 1, ai_colour);
        m_board.unmake_move(move);

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

float SearchTree::alpha_beta_min(float alpha, float beta, uint8_t depth_left, PieceColour ai_colour)
{
    if (depth_left == 0)
    {
        ShannonHeuristic eval(m_board, ai_colour);
        return -eval.get();
    }

    IBoard::MoveList move_list = m_board.get_all_legal_moves(m_board.get_colour_to_move());

    if (move_list.empty())
    {
        return (m_board.get_in_check(m_board.get_colour_to_move()) ? (m_board.get_colour_to_move() == ai_colour ? -200.f : 200.f) : 0);
    }

    for (const auto& move : move_list)
    {
        m_board.make_move(move);
        float score = alpha_beta_max(alpha, beta, depth_left - 1, ai_colour);
        m_board.unmake_move(move);

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

    IBoard::MoveList move_list = m_board.get_all_legal_moves(m_board.get_colour_to_move());

    float alpha = -std::numeric_limits<float>::infinity();
    float beta = std::numeric_limits<float>::infinity();

    for (const auto& move : move_list)
    {
        m_board.make_move(move);
        //float score = mini(search_depth, ai_colour);
        float score = alpha_beta_min(alpha, beta, search_depth, ai_colour);
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