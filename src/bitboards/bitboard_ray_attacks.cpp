#include "bitboard_ray_attacks.h"

#include "bitboard_utils.h"

using namespace bitboard_utils;

uint64_t BitboardRayAttacks::get_ray_mask(uint8_t sq, RayDir dir) const
{
    return ray_masks[static_cast<size_t>(dir)][sq];
}

uint64_t BitboardRayAttacks::get_positive_ray_attacks(uint8_t sq, RayDir dir, bool white_to_move)
{
    uint64_t blocked_ray = m_bitboard.get_occupied() & get_ray_mask(sq, dir);
    check_king_pin(blocked_ray, white_to_move);
    return get_ray_mask(sq, dir) ^ ((blocked_ray == 0) ? 0 : get_ray_mask(bit_scan_forward(blocked_ray), dir));
}

uint64_t BitboardRayAttacks::get_negative_ray_attacks(uint8_t sq, RayDir dir, bool white_to_move)
{
    uint64_t blocked_ray = m_bitboard.get_occupied() & get_ray_mask(sq, dir);
    check_king_pin(blocked_ray, white_to_move);
    return get_ray_mask(sq, dir) ^ ((blocked_ray == 0) ? 0 : get_ray_mask(bit_scan_reverse(blocked_ray), dir));
}

uint64_t BitboardRayAttacks::get_ray_attacks(uint8_t sq, RayDir dir, bool white_to_move)
{
    uint64_t ret;

    switch (dir)
    {
    case RayDir::NE:
    case RayDir::N:
    case RayDir::NW:
    case RayDir::W:
        ret = get_positive_ray_attacks(sq, dir, white_to_move);
        break;

    default:
        ret = get_negative_ray_attacks(sq, dir, white_to_move);
        break;
    }

    return ret & ~m_bitboard.pieces_to_move(white_to_move);
}

void BitboardRayAttacks::check_king_pin(uint64_t blocked_ray, bool white_to_move)
{
    // check blocked ray has king on it
    uint64_t enemy_pieces = m_bitboard.pieces_to_move(!white_to_move);
    uint64_t king_pinned = blocked_ray & (m_bitboard.get_kings() & enemy_pieces);

    blocked_ray ^= king_pinned;

    // check for single bit
    if (king_pinned && (blocked_ray & (blocked_ray - 1)) == 0)
    {
        m_pinned |= blocked_ray;
    }
}

uint64_t BitboardRayAttacks::get_bishop_moves(std::list<Move>& move_list, bool white_to_move, uint64_t pinned)
{
    return m_bitboard.get_moves(move_list, m_bitboard.get_bishops() & ~pinned, white_to_move, [&](uint8_t sq) {
        return get_ray_attacks(sq, RayDir::NE, white_to_move) |
            get_ray_attacks(sq, RayDir::SE, white_to_move) |
            get_ray_attacks(sq, RayDir::NW, white_to_move) |
            get_ray_attacks(sq, RayDir::SW, white_to_move);
        });
}

uint64_t BitboardRayAttacks::get_rook_moves(std::list<Move>& move_list, bool white_to_move, uint64_t pinned)
{
    return m_bitboard.get_moves(move_list, m_bitboard.get_rooks() & ~pinned, white_to_move, [&](uint8_t sq) {
        return get_ray_attacks(sq, RayDir::N, white_to_move) |
            get_ray_attacks(sq, RayDir::S, white_to_move) |
            get_ray_attacks(sq, RayDir::E, white_to_move) |
            get_ray_attacks(sq, RayDir::W, white_to_move);
        });
}

uint64_t BitboardRayAttacks::get_queen_moves(std::list<Move>& move_list, bool white_to_move, uint64_t pinned)
{
    return m_bitboard.get_moves(move_list, m_bitboard.get_queens() & ~pinned, white_to_move, [&](uint8_t sq) {
        return get_ray_attacks(sq, RayDir::NE, white_to_move) |
            get_ray_attacks(sq, RayDir::SE, white_to_move) |
            get_ray_attacks(sq, RayDir::NW, white_to_move) |
            get_ray_attacks(sq, RayDir::SW, white_to_move) |
            get_ray_attacks(sq, RayDir::N, white_to_move) |
            get_ray_attacks(sq, RayDir::S, white_to_move) |
            get_ray_attacks(sq, RayDir::E, white_to_move) |
            get_ray_attacks(sq, RayDir::W, white_to_move);
        });
}

BitboardRayAttacks::BitboardRayAttacks(const BitBoard& bitboard) :
    m_bitboard(bitboard),
    m_pinned(0)
{}