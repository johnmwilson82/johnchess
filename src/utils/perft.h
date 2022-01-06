#pragma once
#include "../board.h"

uint64_t perft(Board& board, int depth)
{
    uint64_t nodes = 0;

    auto moves = board.get_all_legal_moves(board.get_colour_to_move());

    if (depth == 1)
    {
        return moves.size();
    }

    for (const auto& move : moves)
    {
        board.make_move(move);
        nodes += perft(board, depth - 1);
        board.unmake_move(move);
    }

    return nodes;
}