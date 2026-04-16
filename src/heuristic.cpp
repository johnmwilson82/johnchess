#include "heuristic.h"

#include <bitboards/bitboard_utils.h>

using namespace bitboard_utils;

static constexpr uint64_t FILE_A = 0x0101010101010101ULL;
static constexpr uint64_t FILE_H = 0x8080808080808080ULL;

// PSTs defined from White's perspective, index 0=a1 through 63=h8.
// Rows written rank-8 first for visual readability.

// clang-format off
static constexpr float pawn_pst[64] = {
    // rank 1
     0.00f,  0.00f,  0.00f,  0.00f,  0.00f,  0.00f,  0.00f,  0.00f,
    // rank 2
    -0.05f, -0.05f, -0.05f,  0.00f,  0.00f, -0.05f, -0.05f, -0.05f,
    // rank 3
     0.05f, -0.05f, -0.10f,  0.00f,  0.00f, -0.10f, -0.05f,  0.05f,
    // rank 4
     0.00f,  0.00f,  0.00f,  0.20f,  0.20f,  0.00f,  0.00f,  0.00f,
    // rank 5
     0.05f,  0.05f,  0.10f,  0.25f,  0.25f,  0.10f,  0.05f,  0.05f,
    // rank 6
     0.10f,  0.10f,  0.20f,  0.30f,  0.30f,  0.20f,  0.10f,  0.10f,
    // rank 7 (one step from promotion)
     0.50f,  0.60f,  0.60f,  0.70f,  0.70f,  0.60f,  0.60f,  0.50f,
    // rank 8
     0.00f,  0.00f,  0.00f,  0.00f,  0.00f,  0.00f,  0.00f,  0.00f,
};

static constexpr float knight_pst[64] = {
    // rank 1
    -0.50f, -0.40f, -0.30f, -0.30f, -0.30f, -0.30f, -0.40f, -0.50f,
    // rank 2
    -0.40f, -0.20f,  0.00f,  0.05f,  0.05f,  0.00f, -0.20f, -0.40f,
    // rank 3
    -0.30f,  0.05f,  0.10f,  0.15f,  0.15f,  0.10f,  0.05f, -0.30f,
    // rank 4
    -0.30f,  0.00f,  0.15f,  0.20f,  0.20f,  0.15f,  0.00f, -0.30f,
    // rank 5
    -0.30f,  0.05f,  0.15f,  0.20f,  0.20f,  0.15f,  0.05f, -0.30f,
    // rank 6
    -0.30f,  0.00f,  0.10f,  0.15f,  0.15f,  0.10f,  0.00f, -0.30f,
    // rank 7
    -0.40f, -0.20f,  0.00f,  0.00f,  0.00f,  0.00f, -0.20f, -0.40f,
    // rank 8
    -0.50f, -0.40f, -0.30f, -0.30f, -0.30f, -0.30f, -0.40f, -0.50f,
};

static constexpr float bishop_pst[64] = {
    // rank 1
    -0.20f, -0.10f, -0.10f, -0.10f, -0.10f, -0.10f, -0.10f, -0.20f,
    // rank 2
    -0.10f,  0.00f,  0.00f,  0.00f,  0.00f,  0.00f,  0.00f, -0.10f,
    // rank 3
    -0.10f,  0.00f,  0.05f,  0.10f,  0.10f,  0.05f,  0.00f, -0.10f,
    // rank 4
    -0.10f,  0.05f,  0.05f,  0.10f,  0.10f,  0.05f,  0.05f, -0.10f,
    // rank 5
    -0.10f,  0.00f,  0.10f,  0.10f,  0.10f,  0.10f,  0.00f, -0.10f,
    // rank 6
    -0.10f,  0.10f,  0.10f,  0.10f,  0.10f,  0.10f,  0.10f, -0.10f,
    // rank 7
    -0.10f,  0.05f,  0.00f,  0.00f,  0.00f,  0.00f,  0.05f, -0.10f,
    // rank 8
    -0.20f, -0.10f, -0.10f, -0.10f, -0.10f, -0.10f, -0.10f, -0.20f,
};

static constexpr float rook_pst[64] = {
    // rank 1
     0.00f,  0.00f,  0.00f,  0.00f,  0.00f,  0.00f,  0.00f,  0.00f,
    // rank 2
    -0.05f,  0.00f,  0.00f,  0.00f,  0.00f,  0.00f,  0.00f, -0.05f,
    // rank 3
    -0.05f,  0.00f,  0.00f,  0.00f,  0.00f,  0.00f,  0.00f, -0.05f,
    // rank 4
     0.00f,  0.00f,  0.00f,  0.05f,  0.05f,  0.00f,  0.00f,  0.00f,
    // rank 5
     0.05f,  0.05f,  0.05f,  0.05f,  0.05f,  0.05f,  0.05f,  0.05f,
    // rank 6
     0.10f,  0.10f,  0.10f,  0.10f,  0.10f,  0.10f,  0.10f,  0.10f,
    // rank 7 (dominant on 7th rank)
     0.10f,  0.10f,  0.10f,  0.10f,  0.10f,  0.10f,  0.10f,  0.10f,
    // rank 8
     0.00f,  0.00f,  0.05f,  0.05f,  0.05f,  0.05f,  0.00f,  0.00f,
};

static constexpr float queen_pst[64] = {
    // rank 1
    -0.20f, -0.10f, -0.10f, -0.05f, -0.05f, -0.10f, -0.10f, -0.20f,
    // rank 2
    -0.10f,  0.00f,  0.00f,  0.00f,  0.00f,  0.00f,  0.00f, -0.10f,
    // rank 3
    -0.10f,  0.00f,  0.05f,  0.05f,  0.05f,  0.05f,  0.00f, -0.10f,
    // rank 4
    -0.05f,  0.00f,  0.05f,  0.05f,  0.05f,  0.05f,  0.00f, -0.05f,
    // rank 5
     0.00f,  0.00f,  0.05f,  0.05f,  0.05f,  0.05f,  0.00f, -0.05f,
    // rank 6
    -0.10f,  0.05f,  0.05f,  0.05f,  0.05f,  0.05f,  0.00f, -0.10f,
    // rank 7
    -0.10f,  0.00f,  0.05f,  0.00f,  0.00f,  0.00f,  0.00f, -0.10f,
    // rank 8
    -0.20f, -0.10f, -0.10f, -0.05f, -0.05f, -0.10f, -0.10f, -0.20f,
};

// Middlegame: reward castled position, penalise centre exposure.
static constexpr float king_mg_pst[64] = {
    // rank 1
     0.20f,  0.30f,  0.10f,  0.00f,  0.00f,  0.10f,  0.30f,  0.20f,
    // rank 2
     0.20f,  0.20f,  0.00f,  0.00f,  0.00f,  0.00f,  0.20f,  0.20f,
    // rank 3
    -0.10f, -0.20f, -0.20f, -0.20f, -0.20f, -0.20f, -0.20f, -0.10f,
    // rank 4
    -0.20f, -0.30f, -0.30f, -0.40f, -0.40f, -0.30f, -0.30f, -0.20f,
    // rank 5
    -0.30f, -0.40f, -0.40f, -0.50f, -0.50f, -0.40f, -0.40f, -0.30f,
    // rank 6
    -0.30f, -0.40f, -0.40f, -0.50f, -0.50f, -0.40f, -0.40f, -0.30f,
    // rank 7
    -0.30f, -0.40f, -0.40f, -0.50f, -0.50f, -0.40f, -0.40f, -0.30f,
    // rank 8
    -0.30f, -0.40f, -0.40f, -0.50f, -0.50f, -0.40f, -0.40f, -0.30f,
};

// Endgame: king should centralise and become active.
static constexpr float king_eg_pst[64] = {
    // rank 1
    -0.50f, -0.30f, -0.30f, -0.30f, -0.30f, -0.30f, -0.30f, -0.50f,
    // rank 2
    -0.30f, -0.30f,  0.00f,  0.00f,  0.00f,  0.00f, -0.30f, -0.30f,
    // rank 3
    -0.30f, -0.10f,  0.20f,  0.30f,  0.30f,  0.20f, -0.10f, -0.30f,
    // rank 4
    -0.30f, -0.10f,  0.30f,  0.40f,  0.40f,  0.30f, -0.10f, -0.30f,
    // rank 5
    -0.30f, -0.10f,  0.30f,  0.40f,  0.40f,  0.30f, -0.10f, -0.30f,
    // rank 6
    -0.30f, -0.10f,  0.20f,  0.30f,  0.30f,  0.20f, -0.10f, -0.30f,
    // rank 7
    -0.30f, -0.20f, -0.10f,  0.00f,  0.00f, -0.10f, -0.20f, -0.30f,
    // rank 8
    -0.50f, -0.40f, -0.30f, -0.20f, -0.20f, -0.30f, -0.40f, -0.50f,
};
// clang-format on

static float pst_score(uint64_t pieces, const float* pst, bool mirror)
{
    float score = 0;
    while (pieces) {
        uint8_t sq = bit_scan_forward(pieces);
        pieces &= pieces - 1;
        score += pst[mirror ? (sq ^ 56) : sq];
    }
    return score;
}

static int count_doubled_pawns(uint64_t pawns)
{
    uint64_t file_fill = pawns;
    file_fill |= file_fill >> 8;
    file_fill |= file_fill >> 16;
    file_fill |= file_fill >> 32;
    return pop_count(pawns) - pop_count(file_fill & 0xFFULL);
}

static int count_blocked_pawns(uint64_t pawns, uint64_t occupied, bool pawns_are_white)
{
    if (pawns_are_white)
        return pop_count(pawns & (occupied >> 8));
    else
        return pop_count(pawns & (occupied << 8));
}

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
    bool ai_is_white = (ai_colour == PieceColour::WHITE);

    uint64_t ai_pieces  = board.pieces_to_move(ai_is_white);
    uint64_t opp_pieces = board.pieces_to_move(!ai_is_white);

    // Phase detection: interpolate king tables based on remaining non-pawn material.
    // Full material weight = 2Q + 4R + 4B + 4N = 62.
    static constexpr float full_material = 62.0f;
    float remaining = 9.0f * pop_count(board.get_queens())
                    + 5.0f * pop_count(board.get_rooks())
                    + 3.0f * pop_count(board.get_bishops())
                    + 3.0f * pop_count(board.get_knights());
    float phase = std::min(remaining / full_material, 1.0f);

    // Material + PST for each piece type.
    uint64_t ai_pawns    = board.get_pawns()   & ai_pieces;
    uint64_t opp_pawns   = board.get_pawns()   & opp_pieces;
    uint64_t ai_knights  = board.get_knights() & ai_pieces;
    uint64_t opp_knights = board.get_knights() & opp_pieces;
    uint64_t ai_bishops  = board.get_bishops() & ai_pieces;
    uint64_t opp_bishops = board.get_bishops() & opp_pieces;
    uint64_t ai_rooks    = board.get_rooks()   & ai_pieces;
    uint64_t opp_rooks   = board.get_rooks()   & opp_pieces;
    uint64_t ai_queens   = board.get_queens()  & ai_pieces;
    uint64_t opp_queens  = board.get_queens()  & opp_pieces;
    uint64_t ai_king     = board.get_kings()   & ai_pieces;
    uint64_t opp_king    = board.get_kings()   & opp_pieces;

    accum += 1.0f * pop_count(ai_pawns)    - 1.0f * pop_count(opp_pawns);
    accum += 3.0f * pop_count(ai_knights)  - 3.0f * pop_count(opp_knights);
    accum += 3.0f * pop_count(ai_bishops)  - 3.0f * pop_count(opp_bishops);
    accum += 5.0f * pop_count(ai_rooks)    - 5.0f * pop_count(opp_rooks);
    accum += 9.0f * pop_count(ai_queens)   - 9.0f * pop_count(opp_queens);
    accum += ai_king ? 200.0f : 0.0f;
    accum -= opp_king ? 200.0f : 0.0f;

    // Bishop pair bonus.
    if (pop_count(ai_bishops)  >= 2) accum += 0.5f;
    if (pop_count(opp_bishops) >= 2) accum -= 0.5f;

    // PST contributions.
    accum += pst_score(ai_pawns,    pawn_pst,   !ai_is_white);
    accum -= pst_score(opp_pawns,   pawn_pst,    ai_is_white);
    accum += pst_score(ai_knights,  knight_pst, !ai_is_white);
    accum -= pst_score(opp_knights, knight_pst,  ai_is_white);
    accum += pst_score(ai_bishops,  bishop_pst, !ai_is_white);
    accum -= pst_score(opp_bishops, bishop_pst,  ai_is_white);
    accum += pst_score(ai_rooks,    rook_pst,   !ai_is_white);
    accum -= pst_score(opp_rooks,   rook_pst,    ai_is_white);
    accum += pst_score(ai_queens,   queen_pst,  !ai_is_white);
    accum -= pst_score(opp_queens,  queen_pst,   ai_is_white);

    // King safety: interpolate between middlegame and endgame tables.
    auto king_pst_score = [&](uint64_t king, bool mirror) {
        float mg = pst_score(king, king_mg_pst, mirror);
        float eg = pst_score(king, king_eg_pst, mirror);
        return phase * mg + (1.0f - phase) * eg;
    };
    accum += king_pst_score(ai_king,  !ai_is_white);
    accum -= king_pst_score(opp_king,  ai_is_white);

    // Pawn structure penalties.
    int ai_D  = count_doubled_pawns(ai_pawns);
    int opp_D = count_doubled_pawns(opp_pawns);
    int ai_S  = count_blocked_pawns(ai_pawns,  board.get_occupied(),  ai_is_white);
    int opp_S = count_blocked_pawns(opp_pawns, board.get_occupied(), !ai_is_white);
    int ai_I  = count_isolated_pawns(ai_pawns);
    int opp_I = count_isolated_pawns(opp_pawns);

    accum -= 0.5f * ((ai_D - opp_D) + (ai_S - opp_S) + (ai_I - opp_I));
}
