#include "search_tree.h"
#include <vector>

float SearchTree::alpha_beta_max(float alpha, float beta, uint8_t depth_left)
{
    if (depth_left == 0)
    {
        ShannonHeuristic eval(m_board);
        return eval.get() * m_mult;
    }

    IBoard::MoveList move_list = m_board.get_all_legal_moves(m_board.get_colour_to_move());

    if (move_list.empty())
    {
        return m_mult * (m_board.get_in_check(m_board.get_colour_to_move()) ? (m_board.get_colour_to_move() == PieceColour::WHITE ? -200 : 200) : 0);
    }

    for (const auto& move : move_list)
    {
        m_board.make_move(move);
        float score = alpha_beta_min(alpha, beta, depth_left - 1);
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

float SearchTree::alpha_beta_min(float alpha, float beta, uint8_t depth_left)
{
    if (depth_left == 0)
    {
        ShannonHeuristic eval(m_board);
        return -eval.get() * m_mult;
    }

    IBoard::MoveList move_list = m_board.get_all_legal_moves(m_board.get_colour_to_move());

    if (move_list.empty())
    {
        return m_mult * (m_board.get_in_check(m_board.get_colour_to_move()) ? (m_board.get_colour_to_move() == PieceColour::WHITE ? -200 : 200) : 0);
    }

    for (const auto& move : move_list)
    {
        m_board.make_move(move);
        float score = alpha_beta_max(alpha, beta, depth_left - 1);
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
    m_mult = ai_colour == PieceColour::WHITE ? 1.0 : -1.0;

    float best_score = -std::numeric_limits<float>::infinity();
    std::unique_ptr<Move> best_move;

    IBoard::MoveList move_list = m_board.get_all_legal_moves(m_board.get_colour_to_move());

    for (const auto& move : move_list)
    {
        m_board.make_move(move);
        float score = alpha_beta_max(-std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), search_depth);
        if (score > best_score)
        {
            best_score = score;
            best_move = std::make_unique<Move>(move);
        }
        m_board.unmake_move(move);
    }

    return *best_move;
}


SearchTree::SearchTree(BitBoard& board) :
    hasher(std::make_unique<ZobristHash>()),
    m_board(board),
    m_mult(1.0)
{

}