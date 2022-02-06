#include "bitboard_ray_attacks.h"

#include "bitboard_utils.h"

using namespace bitboard_utils;

enum class RayDir : size_t
{
    SE,
    W,
    NE,
    S,
    N,
    NW,
    E,
    SW
};


static constexpr uint64_t RayMaskSE[64] = {
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000001, 0x0000000000000002, 0x0000000000000004, 0x0000000000000008, 0x0000000000000010, 0x0000000000000020, 0x0000000000000040,
    0x0000000000000000, 0x0000000000000100, 0x0000000000000201, 0x0000000000000402, 0x0000000000000804, 0x0000000000001008, 0x0000000000002010, 0x0000000000004020,
    0x0000000000000000, 0x0000000000010000, 0x0000000000020100, 0x0000000000040201, 0x0000000000080402, 0x0000000000100804, 0x0000000000201008, 0x0000000000402010,
    0x0000000000000000, 0x0000000001000000, 0x0000000002010000, 0x0000000004020100, 0x0000000008040201, 0x0000000010080402, 0x0000000020100804, 0x0000000040201008,
    0x0000000000000000, 0x0000000100000000, 0x0000000201000000, 0x0000000402010000, 0x0000000804020100, 0x0000001008040201, 0x0000002010080402, 0x0000004020100804,
    0x0000000000000000, 0x0000010000000000, 0x0000020100000000, 0x0000040201000000, 0x0000080402010000, 0x0000100804020100, 0x0000201008040201, 0x0000402010080402,
    0x0000000000000000, 0x0001000000000000, 0x0002010000000000, 0x0004020100000000, 0x0008040201000000, 0x0010080402010000, 0x0020100804020100, 0x0040201008040201,
};

static constexpr uint64_t RayMaskNE[64] = {
    0x0000000000000000, 0x0000000000000100, 0x0000000000010200, 0x0000000001020400, 0x0000000102040800, 0x0000010204081000, 0x0001020408102000, 0x0102040810204000,
    0x0000000000000000, 0x0000000000010000, 0x0000000001020000, 0x0000000102040000, 0x0000010204080000, 0x0001020408100000, 0x0102040810200000, 0x0204081020400000,
    0x0000000000000000, 0x0000000001000000, 0x0000000102000000, 0x0000010204000000, 0x0001020408000000, 0x0102040810000000, 0x0204081020000000, 0x0408102040000000,
    0x0000000000000000, 0x0000000100000000, 0x0000010200000000, 0x0001020400000000, 0x0102040800000000, 0x0204081000000000, 0x0408102000000000, 0x0810204000000000,
    0x0000000000000000, 0x0000010000000000, 0x0001020000000000, 0x0102040000000000, 0x0204080000000000, 0x0408100000000000, 0x0810200000000000, 0x1020400000000000,
    0x0000000000000000, 0x0001000000000000, 0x0102000000000000, 0x0204000000000000, 0x0408000000000000, 0x0810000000000000, 0x1020000000000000, 0x2040000000000000,
    0x0000000000000000, 0x0100000000000000, 0x0200000000000000, 0x0400000000000000, 0x0800000000000000, 0x1000000000000000, 0x2000000000000000, 0x4000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
};

static constexpr uint64_t RayMaskNW[64] = {
    0x8040201008040200, 0x0080402010080400, 0x0000804020100800, 0x0000008040201000, 0x0000000080402000, 0x0000000000804000, 0x0000000000008000, 0x0000000000000000,
    0x4020100804020000, 0x8040201008040000, 0x0080402010080000, 0x0000804020100000, 0x0000008040200000, 0x0000000080400000, 0x0000000000800000, 0x0000000000000000,
    0x2010080402000000, 0x4020100804000000, 0x8040201008000000, 0x0080402010000000, 0x0000804020000000, 0x0000008040000000, 0x0000000080000000, 0x0000000000000000,
    0x1008040200000000, 0x2010080400000000, 0x4020100800000000, 0x8040201000000000, 0x0080402000000000, 0x0000804000000000, 0x0000008000000000, 0x0000000000000000,
    0x0804020000000000, 0x1008040000000000, 0x2010080000000000, 0x4020100000000000, 0x8040200000000000, 0x0080400000000000, 0x0000800000000000, 0x0000000000000000,
    0x0402000000000000, 0x0804000000000000, 0x1008000000000000, 0x2010000000000000, 0x4020000000000000, 0x8040000000000000, 0x0080000000000000, 0x0000000000000000,
    0x0200000000000000, 0x0400000000000000, 0x0800000000000000, 0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
};

static constexpr uint64_t RayMaskSW[64] = {
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000002, 0x0000000000000004, 0x0000000000000008, 0x0000000000000010, 0x0000000000000020, 0x0000000000000040, 0x0000000000000080, 0x0000000000000000,
    0x0000000000000204, 0x0000000000000408, 0x0000000000000810, 0x0000000000001020, 0x0000000000002040, 0x0000000000004080, 0x0000000000008000, 0x0000000000000000,
    0x0000000000020408, 0x0000000000040810, 0x0000000000081020, 0x0000000000102040, 0x0000000000204080, 0x0000000000408000, 0x0000000000800000, 0x0000000000000000,
    0x0000000002040810, 0x0000000004081020, 0x0000000008102040, 0x0000000010204080, 0x0000000020408000, 0x0000000040800000, 0x0000000080000000, 0x0000000000000000,
    0x0000000204081020, 0x0000000408102040, 0x0000000810204080, 0x0000001020408000, 0x0000002040800000, 0x0000004080000000, 0x0000008000000000, 0x0000000000000000,
    0x0000020408102040, 0x0000040810204080, 0x0000081020408000, 0x0000102040800000, 0x0000204080000000, 0x0000408000000000, 0x0000800000000000, 0x0000000000000000,
    0x0002040810204080, 0x0004081020408000, 0x0008102040800000, 0x0010204080000000, 0x0020408000000000, 0x0040800000000000, 0x0080000000000000, 0x0000000000000000,
};


template<RayDir Dir>
static constexpr uint64_t get_ray_mask(uint8_t sq)
{
    return Dir == RayDir::N  ?  0x0101010101010100 << sq :
           Dir == RayDir::S  ?  0x0080808080808080 >> (63 - sq) :
           Dir == RayDir::W  ?  (0x7fffffff'ffffffff >> (63 - sq)) - (((2ULL << (sq | 7)) - 1) >> 8) :
           Dir == RayDir::E  ?  2 * ((1ULL << (sq | 7)) - (1ULL << sq)) :
           Dir == RayDir::NE ?  RayMaskNE[sq] :
           Dir == RayDir::NW ?  RayMaskNW[sq] :
           Dir == RayDir::SE ?  RayMaskSE[sq] :
                                RayMaskSW[sq]; // Dir == RayDir::SW ? 
}


template<bool WhiteToMove, RayDir Dir>
static constexpr uint64_t check_king_pin(uint64_t blocked_ray, uint64_t allowed_in_pin_moves, BitBoardRayState& state)
{
    // check blocked ray has king on it
    uint64_t enemy_pieces = state.m_bitboard.pieces_to_move(!WhiteToMove);
    uint64_t king = (state.m_bitboard.get_kings() & enemy_pieces);
    uint64_t king_pinned = blocked_ray & king;

    uint64_t blocked_ray_less_ep = blocked_ray;

    if (king_pinned)
    {
        blocked_ray &= ~(get_ray_mask<Dir>(bit_scan_reverse(king)) | king);
        // check for single bit
        if (blocked_ray && (blocked_ray & (blocked_ray - 1)) == 0)
        {
            // at this point blocked_ray has one bit on it which is the location of the
            // piece which is blocked by the king.
            state.m_attacked_pinned |= blocked_ray;
            state.m_attacked_pinned_allowed[bit_scan_forward(blocked_ray)] = allowed_in_pin_moves;

            return king_pinned;
        }
    }

    // check for en passant rank pin, where the capturing pawn and pawn to be captured are on
    // the same rank as a pinned king (en passant capture is illegal)
    auto ep_col = state.m_bitboard.get_enpassant_column();
    if (king_pinned && ep_col.has_value() && (Dir == RayDir::E || Dir == RayDir::W))
    {
        uint64_t ep_square = (!WhiteToMove ? 0x00000001'00000000 : 0x00000000'01000000) << *ep_col;

        blocked_ray_less_ep = blocked_ray & ~(ep_square);

        if ((blocked_ray_less_ep & (blocked_ray_less_ep - 1)) == 0)
        {
            state.m_en_passant_pinned = true;
        }
    }

    return king_pinned;
}


template<bool WhiteToMove, RayDir Dir>
static constexpr uint64_t get_positive_ray_attacks(uint8_t sq, BitBoardRayState& state)
{
    uint64_t blocked_ray = state.m_bitboard.get_occupied() & get_ray_mask<Dir>(sq);

    uint8_t blocker = (blocked_ray == 0) ? 0 : bit_scan_forward(blocked_ray);

    uint64_t enemy_king = (state.m_bitboard.get_kings() & state.m_bitboard.pieces_to_move(!WhiteToMove));

    uint64_t ret = get_ray_mask<Dir>(sq) ^ ((blocked_ray == 0) ? 0 : get_ray_mask<Dir>(blocker));

    if (enemy_king)
    {
        check_king_pin<WhiteToMove, Dir>(blocked_ray, (1ULL << sq) | (get_ray_mask<Dir>(sq) ^ get_ray_mask<Dir>(bit_scan_forward(enemy_king))), state);
    }

    if (blocked_ray && ((1ULL << blocker) & enemy_king))
    {
        state.m_king_attacks |= enemy_king;
        get_positive_ray_attacks<WhiteToMove, Dir>(blocker, state);
    }
    else if ((1ULL << sq) & enemy_king)
    {
        state.m_king_attacks |= ret;
    };

    if (ret & (state.m_bitboard.get_kings() & state.m_bitboard.pieces_to_move(!WhiteToMove)))
    {
        state.m_allowed_next_moves &= ret | (1ULL << sq);
    }

    return ret;
}


template<bool WhiteToMove, RayDir Dir>
static constexpr uint64_t get_negative_ray_attacks(uint8_t sq, BitBoardRayState& state)
{
    uint64_t blocked_ray = state.m_bitboard.get_occupied() & get_ray_mask<Dir>(sq);

    uint8_t blocker = (blocked_ray == 0) ? 0 : bit_scan_reverse(blocked_ray);

    uint64_t enemy_king = (state.m_bitboard.get_kings() & state.m_bitboard.pieces_to_move(!WhiteToMove));

    uint64_t ret = get_ray_mask<Dir>(sq) ^ ((blocked_ray == 0) ? 0 : get_ray_mask<Dir>(blocker));

    if (enemy_king)
    {
        check_king_pin<WhiteToMove, Dir>(blocked_ray, (1ULL << sq) | (get_ray_mask<Dir>(sq) ^ get_ray_mask<Dir>(bit_scan_forward(enemy_king))), state);
    }

    if (blocked_ray && ((1ULL << blocker) & enemy_king))
    {
        state.m_king_attacks |= enemy_king;
        get_negative_ray_attacks<WhiteToMove, Dir>(blocker, state);
    }
    else if((1ULL << sq) & enemy_king)
    {
        state.m_king_attacks |= ret;
    };

    if (ret & (state.m_bitboard.get_kings() & state.m_bitboard.pieces_to_move(!WhiteToMove)))
    {
        state.m_allowed_next_moves &= ret | (1ULL << sq);
    }

    return ret;
}


template<bool WhiteToMove, RayDir Dir>
static constexpr uint64_t get_ray_attacks(uint8_t sq, BitBoardRayState& state)
{
    if(Dir == RayDir::NE || Dir == RayDir::N || Dir == RayDir::NW || Dir == RayDir::E)
        return get_positive_ray_attacks<WhiteToMove, Dir>(sq, state);

    return get_negative_ray_attacks<WhiteToMove, Dir>(sq, state);
}


template<bool WhiteToMove>
uint64_t BitboardRayAttacks<WhiteToMove>::get_pinned_piece_moves(BitBoard::MoveList& move_list, uint64_t& pieces, std::function<uint64_t(uint8_t)> attacks_fn) const
{
    uint64_t ret = 0;

    uint64_t pieces_pin_check = pieces;

    while (pieces_pin_check)
    {
        uint8_t current_piece = bit_scan_forward(pieces_pin_check);

        if (m_state.m_moving_pinned_allowed.contains(current_piece))
        {
            ret |= m_state.m_bitboard.get_moves<WhiteToMove>(move_list, 1ULL << current_piece, [&](uint8_t sq) {
                uint64_t att = attacks_fn(sq);
                return attacks_fn(sq) & m_state.m_moving_pinned_allowed.at(current_piece);
            });

            pieces &= ~(1ULL << current_piece);
        }

        pieces_pin_check &= (pieces_pin_check - 1);
    }

    return ret;
}


template<bool WhiteToMove>
uint64_t BitboardRayAttacks<WhiteToMove>::get_bishop_moves(BitBoard::MoveList& move_list)
{
    auto bishop_moves = [&](uint8_t sq) {
        return get_ray_attacks<WhiteToMove, RayDir::NE>(sq, m_state) |
               get_ray_attacks<WhiteToMove, RayDir::SE>(sq, m_state) |
               get_ray_attacks<WhiteToMove, RayDir::NW>(sq, m_state) |
               get_ray_attacks<WhiteToMove, RayDir::SW>(sq, m_state);
    };
    
    uint64_t ret = 0;
    uint64_t bishops = m_state.m_bitboard.get_bishops();
    
    if (!bishops) return 0;

    ret |= get_pinned_piece_moves(move_list, bishops, bishop_moves);

    ret |= m_state.m_bitboard.get_moves<WhiteToMove>(move_list, bishops, bishop_moves);

    return ret;
}


template<bool WhiteToMove>
uint64_t BitboardRayAttacks<WhiteToMove>::get_rook_moves(BitBoard::MoveList& move_list)
{
    auto rook_moves = [&](uint8_t sq) {
        return get_ray_attacks<WhiteToMove, RayDir::N>(sq, m_state) |
               get_ray_attacks<WhiteToMove, RayDir::S>(sq, m_state) |
               get_ray_attacks<WhiteToMove, RayDir::E>(sq, m_state) |
               get_ray_attacks<WhiteToMove, RayDir::W>(sq, m_state);
    };

    uint64_t ret = 0;
    uint64_t rooks = m_state.m_bitboard.get_rooks();

    if (!rooks) return 0;

    ret |= get_pinned_piece_moves(move_list, rooks, rook_moves);

    ret |= m_state.m_bitboard.get_moves<WhiteToMove>(move_list, rooks, rook_moves);

    return ret;
}


template<bool WhiteToMove>
uint64_t BitboardRayAttacks<WhiteToMove>::get_queen_moves(BitBoard::MoveList& move_list)
{
    auto queen_moves = [&](uint8_t sq) {
        return get_ray_attacks<WhiteToMove, RayDir::NE>(sq, m_state) |
            get_ray_attacks<WhiteToMove, RayDir::SE>(sq, m_state) |
            get_ray_attacks<WhiteToMove, RayDir::NW>(sq, m_state) |
            get_ray_attacks<WhiteToMove, RayDir::SW>(sq, m_state) |
            get_ray_attacks<WhiteToMove, RayDir::N>(sq, m_state) |
            get_ray_attacks<WhiteToMove, RayDir::S>(sq, m_state) |
            get_ray_attacks<WhiteToMove, RayDir::E>(sq, m_state) |
            get_ray_attacks<WhiteToMove, RayDir::W>(sq, m_state);
    };

    uint64_t ret = 0;
    uint64_t queens = m_state.m_bitboard.get_queens();

    if (!queens) return 0;

    ret |= get_pinned_piece_moves(move_list, queens, queen_moves);

    ret |= m_state.m_bitboard.get_moves<WhiteToMove>(move_list, queens, queen_moves);

    return ret;
}


template<bool WhiteToMove>
BitboardRayAttacks<WhiteToMove>::BitboardRayAttacks(const BitBoard& bitboard,
                                       const std::unordered_map<uint8_t, uint64_t>& pinned_piece_allowed_moves) :
    m_state(bitboard, pinned_piece_allowed_moves)
{}


template uint64_t BitboardRayAttacks<true>::get_queen_moves(BitBoard::MoveList& move_list);
template uint64_t BitboardRayAttacks<true>::get_rook_moves(BitBoard::MoveList& move_list);
template uint64_t BitboardRayAttacks<true>::get_bishop_moves(BitBoard::MoveList& move_list);
template uint64_t BitboardRayAttacks<false>::get_queen_moves(BitBoard::MoveList& move_list);
template uint64_t BitboardRayAttacks<false>::get_rook_moves(BitBoard::MoveList& move_list);
template uint64_t BitboardRayAttacks<false>::get_bishop_moves(BitBoard::MoveList& move_list);

template BitboardRayAttacks<true>::BitboardRayAttacks(const BitBoard& bitboard,
    const std::unordered_map<uint8_t, uint64_t>& pinned_piece_allowed_moves);
template BitboardRayAttacks<false>::BitboardRayAttacks(const BitBoard& bitboard,
    const std::unordered_map<uint8_t, uint64_t>& pinned_piece_allowed_moves);