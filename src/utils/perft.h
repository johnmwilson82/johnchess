#pragma once
#include "../iboard.h"

struct PerftStats
{
    uint64_t nodes;
    uint64_t captures;
    uint64_t en_passant;
    uint64_t promotions;
};

uint64_t perft(IBoard& board, int depth)
{
    if (depth == 0)
    {
        return 1;
    }

    uint64_t nodes = 0;

    auto moves = board.get_all_legal_moves(board.get_colour_to_move());

    if (depth == 1)
    {
        /*for (const auto& move : moves)
        {
            for (int i = 0; i < 4 - depth; ++i)
            {
                std::cout << "\033[6C";
            }
            std::cout << move.to_string() << "                                             \r";
        }*/

        return moves.size();
    }

    for (const auto& move : moves)
    {
        /*for (int i = 0; i < 4 - depth; ++i)
        {
            std::cout << "\033[6C";
        }
        std::cout << move.to_string() << "                                              \r";
        */
        board.make_move(move);
        nodes += perft(board, depth - 1);
        board.unmake_move(move);
    }

    return nodes;
}