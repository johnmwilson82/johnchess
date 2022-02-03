#include "heuristic.h"

#include <bitboards/bitboard_utils.h>

// f(p) = 200(K-K')
//        + 9(Q-Q')
//        + 5(R-R')
//        + 3(B-B' + N-N')
//        + 1(P-P')
//        - 0.5(D-D' + S-S' + I-I')
//        + 0.1(M-M') + ...

// KQRBNP = number of kings, queens, rooks, bishops, knights and pawns
// D,S,I = doubled, blocked and isolated pawns
// M = Mobility (the number of legal moves)

using namespace bitboard_utils;

ShannonHeuristic::ShannonHeuristic(const BitBoard& board, PieceColour ai_colour)
{
    bool ai_player_is_white = PieceColour::WHITE == ai_colour;

    accum += pop_count(board.get_pawns() & board.pieces_to_move(ai_player_is_white));
    accum -= pop_count(board.get_pawns() & board.pieces_to_move(!ai_player_is_white));

    accum += 3.0f * pop_count(board.get_knights() & board.pieces_to_move(ai_player_is_white));
    accum -= 3.0f * pop_count(board.get_knights() & board.pieces_to_move(!ai_player_is_white));

    accum += 3.0f * pop_count(board.get_bishops() & board.pieces_to_move(ai_player_is_white));
    accum -= 3.0f * pop_count(board.get_bishops() & board.pieces_to_move(!ai_player_is_white));

    accum += 5.0f * pop_count(board.get_rooks() & board.pieces_to_move(ai_player_is_white));
    accum -= 5.0f * pop_count(board.get_rooks() & board.pieces_to_move(!ai_player_is_white));

    accum += 9.0f * pop_count(board.get_queens() & board.pieces_to_move(ai_player_is_white));
    accum -= 9.0f * pop_count(board.get_queens() & board.pieces_to_move(!ai_player_is_white));

    accum += board.get_kings() & board.pieces_to_move(ai_player_is_white) ? 200 : 0;
    accum -= board.get_kings() & board.pieces_to_move(!ai_player_is_white) ? 200 : 0;

    //accum += board.get_all_legal_moves(Piece::WHITE).size() * 0.1;
    //accum -= board.get_all_legal_moves(Piece::BLACK).size() * 0.1;

}