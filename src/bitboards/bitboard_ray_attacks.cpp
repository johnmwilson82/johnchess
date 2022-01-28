#include "bitboard_ray_attacks.h"

#include "bitboard_utils.h"

using namespace bitboard_utils;

uint64_t BitboardRayAttacks::get_ray_mask(uint8_t sq, RayDir dir) const
{
    return ray_masks[static_cast<size_t>(dir)][sq];
}

uint64_t BitboardRayAttacks::get_positive_ray_attacks(uint8_t sq, RayDir dir)
{
    uint64_t blocked_ray = m_bitboard.get_occupied() & get_ray_mask(sq, dir);

    uint8_t blocker = (blocked_ray == 0) ? 0 : bit_scan_forward(blocked_ray);

    uint64_t enemy_king = (m_bitboard.get_kings() & m_bitboard.pieces_to_move(!m_white_to_move));

    uint64_t ret = get_ray_mask(sq, dir) ^ ((blocked_ray == 0) ? 0 : get_ray_mask(blocker, dir));

    uint64_t king_pin = check_king_pin(blocked_ray, ret | (1ULL << sq), dir);

    if (blocked_ray && ((1ULL << blocker) & enemy_king))
    {
        m_king_attacks |= enemy_king;
        get_positive_ray_attacks(blocker, dir);
    }
    else if ((1ULL << sq) & enemy_king)
    {
        m_king_attacks |= ret;
    };

    if (ret & (m_bitboard.get_kings() & m_bitboard.pieces_to_move(!m_white_to_move)))
    {
        m_allowed_next_moves &= ret | (1ULL << sq);
    }

    return ret;
}

uint64_t BitboardRayAttacks::get_negative_ray_attacks(uint8_t sq, RayDir dir)
{
    uint64_t blocked_ray = m_bitboard.get_occupied() & get_ray_mask(sq, dir);

    uint8_t blocker = (blocked_ray == 0) ? 0 : bit_scan_reverse(blocked_ray);

    uint64_t enemy_king = (m_bitboard.get_kings() & m_bitboard.pieces_to_move(!m_white_to_move));

    uint64_t ret = get_ray_mask(sq, dir) ^ ((blocked_ray == 0) ? 0 : get_ray_mask(blocker, dir));
        
    uint64_t king_pin = check_king_pin(blocked_ray, ret | (1ULL << sq), dir);

    if (blocked_ray && ((1ULL << blocker) & enemy_king))
    {
        m_king_attacks |= enemy_king;
        get_negative_ray_attacks(blocker, dir);
    }
    else if((1ULL << sq) & enemy_king)
    {
        m_king_attacks |= ret;
    };

    if (ret & (m_bitboard.get_kings() & m_bitboard.pieces_to_move(!m_white_to_move)))
    {
        m_allowed_next_moves &= ret | (1ULL << sq);
    }

    return ret;
}


uint64_t BitboardRayAttacks::get_ray_attacks(uint8_t sq, RayDir dir)
{
    uint64_t ret;

    switch (dir)
    {
    case RayDir::NE:
    case RayDir::N:
    case RayDir::NW:
    case RayDir::W:
        ret = get_positive_ray_attacks(sq, dir);
        break;

    default:
        ret = get_negative_ray_attacks(sq, dir);
        break;
    }



    return ret & ~m_bitboard.pieces_to_move(m_white_to_move);
}


uint64_t BitboardRayAttacks::check_king_pin(uint64_t blocked_ray, uint64_t allowed_in_pin_moves, RayDir dir)
{
    // check blocked ray has king on it
    uint64_t enemy_pieces = m_bitboard.pieces_to_move(!m_white_to_move);
    uint64_t king = (m_bitboard.get_kings() & enemy_pieces);
    uint64_t king_pinned = blocked_ray & king;

    // check for single bit
    if (king_pinned)
    {
        blocked_ray &= ~(get_ray_mask(bit_scan_reverse(king), dir) | king);
        if (blocked_ray && (blocked_ray & (blocked_ray - 1)) == 0)
        {
            // at this point blocked_ray has one bit on it which is the location of the
            // piece which is blocked by the king.
            m_attacked_pinned |= blocked_ray;
            m_attacked_pinned_allowed[bit_scan_forward(blocked_ray)] = allowed_in_pin_moves;

            return king_pinned;
        }
    }

    // check for en passant rank pin, where the capturing pawn and pawn to be captured are on
    // the same rank as a pinned king (en passant capture is illegal)
    auto ep_col = m_bitboard.get_enpassant_column();
    if (king_pinned && ep_col.has_value())
    {
        uint64_t ep_square = (!m_white_to_move ? 0x00000001'00000000 : 0x00000000'01000000) << *ep_col;
        
        uint64_t blocked_ray_less_ep = blocked_ray & ~(ep_square);
        if ((blocked_ray_less_ep & (blocked_ray_less_ep - 1)) == 0)
        {
            m_en_passant_pinned = true;
        }
    }
    
    return king_pinned;
}


uint64_t BitboardRayAttacks::get_pinned_piece_moves(std::list<Move>& move_list, uint64_t& pieces, std::function<uint64_t(uint8_t)> attacks_fn) const
{
    uint64_t ret = 0;

    uint64_t pieces_pin_check = pieces;

    uint8_t current_piece = bit_scan_forward(pieces_pin_check);

    while (m_moving_pinned_allowed.contains(current_piece))
    {
        ret |= m_bitboard.get_moves(move_list, 1ULL << current_piece, m_white_to_move, [&](uint8_t sq) {
            uint64_t att = attacks_fn(sq);
            return attacks_fn(sq) & m_moving_pinned_allowed.at(current_piece);
        });

        pieces_pin_check &= (pieces_pin_check - 1);
        pieces &= ~(1ULL << current_piece);

        current_piece = bit_scan_forward(pieces_pin_check);
    }

    return ret;
}


uint64_t BitboardRayAttacks::get_bishop_moves(std::list<Move>& move_list)
{
    auto bishop_moves = [&](uint8_t sq) {
        return get_ray_attacks(sq, RayDir::NE) |
               get_ray_attacks(sq, RayDir::SE) |
               get_ray_attacks(sq, RayDir::NW) |
               get_ray_attacks(sq, RayDir::SW);
    };
    
    uint64_t ret = 0;
    uint64_t bishops = m_bitboard.get_bishops();
    
    if (!bishops) return 0;

    ret |= get_pinned_piece_moves(move_list, bishops, bishop_moves);

    ret |= m_bitboard.get_moves(move_list, bishops, m_white_to_move, bishop_moves);

    return ret;
}


uint64_t BitboardRayAttacks::get_rook_moves(std::list<Move>& move_list)
{
    auto rook_moves = [&](uint8_t sq) {
        return get_ray_attacks(sq, RayDir::N) |
               get_ray_attacks(sq, RayDir::S) |
               get_ray_attacks(sq, RayDir::E) |
               get_ray_attacks(sq, RayDir::W);
    };
    uint64_t ret = 0;
    uint64_t rooks = m_bitboard.get_rooks();

    if (!rooks) return 0;

    ret |= get_pinned_piece_moves(move_list, rooks, rook_moves);

    ret |= m_bitboard.get_moves(move_list, rooks, m_white_to_move, rook_moves);

    return ret;
}


uint64_t BitboardRayAttacks::get_queen_moves(std::list<Move>& move_list)
{
    auto queen_moves = [&](uint8_t sq) {
        return get_ray_attacks(sq, RayDir::NE) |
               get_ray_attacks(sq, RayDir::SE) |
               get_ray_attacks(sq, RayDir::NW) |
               get_ray_attacks(sq, RayDir::SW) |
               get_ray_attacks(sq, RayDir::N) |
               get_ray_attacks(sq, RayDir::S) |
               get_ray_attacks(sq, RayDir::E) |
               get_ray_attacks(sq, RayDir::W);
    };

    uint64_t ret = 0;
    uint64_t queens = m_bitboard.get_queens();

    if (!queens) return 0;

    ret |= get_pinned_piece_moves(move_list, queens, queen_moves);

    ret |= m_bitboard.get_moves(move_list, queens, m_white_to_move, queen_moves);

    return ret;
}


BitboardRayAttacks::BitboardRayAttacks(const BitBoard& bitboard,
                                       bool white_to_move,
                                       const std::unordered_map<uint8_t, uint64_t>& pinned_piece_allowed_moves) :
    m_bitboard(bitboard),
    m_attacked_pinned(0),
    m_white_to_move(white_to_move),
    m_moving_pinned_allowed(pinned_piece_allowed_moves),
    m_allowed_next_moves(0xffffffff'ffffffff),
    m_en_passant_pinned(false),
    m_king_attacks(0)
{}