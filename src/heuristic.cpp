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

static constexpr uint64_t FILE_A = 0x0101010101010101ULL;
static constexpr uint64_t FILE_H = 0x8080808080808080ULL;

// Doubled: pawns beyond the first on any file. Total pawns minus number of occupied files.
static int count_doubled_pawns(uint64_t pawns)
{
    uint64_t file_fill = pawns;
    file_fill |= file_fill >> 8;
    file_fill |= file_fill >> 16;
    file_fill |= file_fill >> 32;
    return pop_count(pawns) - pop_count(file_fill & 0xFFULL);
}

// Blocked: pawns with any piece directly in front. White moves toward rank 8 (higher bits).
static int count_blocked_pawns(uint64_t pawns, uint64_t occupied, bool pawns_are_white)
{
    if (pawns_are_white)
        return pop_count(pawns & (occupied >> 8));
    else
        return pop_count(pawns & (occupied << 8));
}

// Isolated: pawns with no friendly pawn on either adjacent file.
static int count_isolated_pawns(uint64_t pawns)
{
    uint64_t fill = pawns;
    fill |= fill << 8;  fill |= fill >> 8;
    fill |= fill << 16; fill |= fill >> 16;
    fill |= fill << 32; fill |= fill >> 32;

    uint64_t adj_files = ((fill & ~FILE_A) >> 1) | ((fill & ~FILE_H) << 1);
    return pop_count(pawns & ~adj_files);
}

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

    accum += board.get_all_legal_moves(ai_colour).size() * 0.1;
    accum -= board.get_all_legal_moves(opposite_colour(ai_colour)).size() * 0.1;

    uint64_t ai_pawns  = board.get_pawns() & board.pieces_to_move(ai_player_is_white);
    uint64_t opp_pawns = board.get_pawns() & board.pieces_to_move(!ai_player_is_white);

    int ai_D  = count_doubled_pawns(ai_pawns);
    int opp_D = count_doubled_pawns(opp_pawns);

    int ai_S  = count_blocked_pawns(ai_pawns,  board.get_occupied(), ai_player_is_white);
    int opp_S = count_blocked_pawns(opp_pawns, board.get_occupied(), !ai_player_is_white);

    int ai_I  = count_isolated_pawns(ai_pawns);
    int opp_I = count_isolated_pawns(opp_pawns);

    accum -= 0.5f * ((ai_D - opp_D) + (ai_S - opp_S) + (ai_I - opp_I));

}