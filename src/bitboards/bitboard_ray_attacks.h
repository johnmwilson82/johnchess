#pragma once
#include <cstdint>
#include <list>

#include <move.h>
#include "bitboard.h"

struct BitBoardRayState
{
    const BitBoard& m_bitboard;
    uint64_t m_attacked_pinned;
    const std::unordered_map<uint8_t, uint64_t>& m_moving_pinned_allowed;
    uint64_t m_allowed_next_moves;
    bool m_en_passant_pinned;
    uint64_t m_king_attacks;
    std::unordered_map<uint8_t, uint64_t> m_attacked_pinned_allowed;

    BitBoardRayState(const BitBoard& bitboard,
                     const std::unordered_map<uint8_t, uint64_t>& pinned_piece_allowed_moves) :
        m_bitboard(bitboard),
        m_attacked_pinned(0),
        m_moving_pinned_allowed(pinned_piece_allowed_moves),
        m_allowed_next_moves(0xffffffff'ffffffff),
        m_en_passant_pinned(false),
        m_king_attacks(0)
    {}
};


template<bool WhiteToMove>
class BitboardRayAttacks
{
private:
    BitBoardRayState m_state;

    uint64_t get_pinned_piece_moves(BitBoard::MoveList& move_list, uint64_t& pieces, std::function<uint64_t(uint8_t)> attacks_fn) const;

public:    
    uint64_t get_bishop_moves(BitBoard::MoveList& move_list);
    uint64_t get_rook_moves(BitBoard::MoveList& move_list);
    uint64_t get_queen_moves(BitBoard::MoveList& move_list);
    uint64_t get_king_attacks() const { return m_state.m_king_attacks; };

    uint64_t get_allowed_next_move_mask() const { return m_state.m_allowed_next_moves; }

    uint64_t get_pinned() const{ return m_state.m_attacked_pinned; }
    bool is_enpassant_pinned() const { return m_state.m_en_passant_pinned; }

    const std::unordered_map<uint8_t, uint64_t>& get_pinned_allowed() const { return m_state.m_attacked_pinned_allowed; }

    BitboardRayAttacks(const BitBoard& bitboard, 
                       const std::unordered_map<uint8_t, uint64_t>& pinned_piece_allowed_moves);
};
