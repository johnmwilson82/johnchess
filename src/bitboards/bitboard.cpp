#include "bitboard.h"

#include "bitboard_utils.h"

#include "bitboard_ray_attacks.h"

using namespace bitboard_utils;

BitBoard::BitBoard() :
    m_pawns(0),
    m_knights(0),
    m_bishops(0),
    m_rooks(0),
    m_queens(0),
    m_kings(0),
    m_white_pieces(0),
    m_black_pieces(0),
    m_occupied(0),
    m_opposite_attacks(0),
    m_current_attacks(0),
    m_white_to_move(1),
    m_allowed_moves(0xffffffff'ffffffff),
    m_new_allowed_moves(0xffffffff'ffffffff)
{

}

void BitBoard::set_to_start_position()
{
    m_pawns   = 0x00ff0000'0000ff00;
    m_knights = 0x42000000'00000042;
    m_bishops = 0x24000000'00000024;
    m_rooks   = 0x81000000'00000081;
    m_queens  = 0x08000000'00000008;
    m_kings   = 0x10000000'00000010;

    m_black_pieces = 0xffff0000'00000000;
    m_white_pieces = 0x00000000'0000ffff;

    m_occupied = 0xffff0000'0000ffff;

    m_white_to_move = 1;
}

void BitBoard::set_from_edit_mode(std::vector<std::string> edit_mode_strings)
{
}

bool BitBoard::get_in_check(PieceColour col) const
{
    uint64_t ret = m_kings & ((col == get_colour_to_move()) ? m_opposite_attacks : m_current_attacks);
    ret &= col == PieceColour::WHITE ? m_white_pieces : m_black_pieces;

    return ret;
}

uint64_t BitBoard::pieces_to_move(bool white_to_move) const
{
    return m_white_pieces * white_to_move | m_black_pieces * !white_to_move;
}

Move& BitBoard::emplace_move(std::list<Move>& move_list, const BoardLocation& from_loc, const BoardLocation& to_loc) const
{
    auto& move = move_list.emplace_back(from_loc, to_loc);
    for (const auto& pieces : { std::make_pair(m_pawns, PieceType::PAWN), 
                                std::make_pair(m_knights, PieceType::KNIGHT),
                                std::make_pair(m_bishops, PieceType::BISHOP),
                                std::make_pair(m_rooks, PieceType::ROOK),
                                std::make_pair(m_queens, PieceType::QUEEN),
                                std::make_pair(m_kings, PieceType::KING)})
    {
        auto test_to = to_loc.to_bitboard_mask();
        auto test_from = from_loc.to_bitboard_mask();
        if (pieces.first & to_loc.to_bitboard_mask())
        {
            move.set_captured_piece_type(pieces.second);
            break;
        }
    }
    move.set_old_castling_rights(m_castling_rights);
    return move;
}

uint64_t BitBoard::get_moves(std::list<Move>& move_list, uint64_t pieces, bool white_to_move, std::function<uint64_t(uint8_t)> attacks_fn) const
{
    uint64_t friendly_pieces = pieces_to_move(white_to_move);
    uint64_t moving_pieces = pieces & friendly_pieces;
    
    uint64_t all_attacks = 0;

    while (moving_pieces)
    {
        uint64_t attacks = 0;
        
        auto piece_sq = bit_scan_forward(moving_pieces);

        attacks |= attacks_fn(piece_sq);

        all_attacks |= attacks;

        attacks &= (~friendly_pieces) & m_allowed_moves;

        while (attacks)
        {
            auto attack_sq = bit_scan_forward(attacks);
            attacks &= attacks - 1;
            emplace_move(move_list, BoardLocation(piece_sq), BoardLocation(attack_sq));
        }

        moving_pieces &= moving_pieces - 1;
    }

    return all_attacks;
}

uint64_t BitBoard::get_pawn_moves(std::list<Move>& move_list, bool white_to_move, const std::unordered_map<uint8_t, uint64_t>& pinned_piece_allowed_moves) const
{
    // Flip the board vertically to calculate the black moves
    uint64_t friendly_pieces = white_to_move ? pieces_to_move(white_to_move) : mirror_vertical(pieces_to_move(white_to_move));
    uint64_t occupied = white_to_move ? m_occupied : mirror_vertical(m_occupied);
    uint64_t enemy_pieces = occupied ^ friendly_pieces;

    uint64_t moving_pieces = (white_to_move ? m_pawns : mirror_vertical(m_pawns)) & friendly_pieces;

    uint64_t first_rank = 0x00000000'0000ff00;

    int8_t forward = 8;

    uint64_t all_attacks = 0;

    while (moving_pieces)
    {
        auto piece_sq = bit_scan_forward(moving_pieces);

        // one square forward
        uint64_t attacks = 1ULL << (piece_sq + forward);
        
        // two squares forward
        attacks |= ((first_rank & (1ULL << piece_sq)) << (2 * forward));

        // remove blocked forward moves 
        attacks &= ~(occupied | ((occupied & ~(1ULL << piece_sq)) << forward));

        // capture left
        uint64_t left_attack = ((1ULL << piece_sq) & 0x00010101'01010100) ? 0 : 1ULL << (piece_sq + forward - 1);
        attacks |= enemy_pieces & left_attack;

        // capture right
        uint64_t right_attack = ((1ULL << piece_sq) & 0x00808080'80808000) ? 0 : 1ULL << (piece_sq + forward + 1);
        attacks |= enemy_pieces & right_attack;

        // attacks can't move into friendly pieces
        attacks &= ~friendly_pieces;

        // flip back attacks
        attacks = (white_to_move ? attacks : mirror_vertical(attacks)) & m_allowed_moves;

        // Scalar square vertical mirror
        piece_sq = white_to_move ? piece_sq : piece_sq ^ 56;

        // If pawn is pinned ensure it moves to allowed square during capture
        if (pinned_piece_allowed_moves.contains(piece_sq))
        {
            attacks &= pinned_piece_allowed_moves.at(piece_sq);
        }

        all_attacks |= (white_to_move ? (left_attack | right_attack) : mirror_vertical(left_attack | right_attack));

        while (attacks)
        {
            auto attack_sq = bit_scan_forward(attacks);
            attacks &= attacks - 1;
            
            if ((1ULL << attack_sq) & 0xff000000'000000ff)
            {
                for (auto promote_to : { Move::PromotionType::QUEEN, Move::PromotionType::ROOK, Move::PromotionType::BISHOP, Move::PromotionType::KNIGHT })
                {
                    auto& move = emplace_move(move_list, BoardLocation(piece_sq), BoardLocation(attack_sq));
                    move.set_promotion_type(promote_to);
                }
            }
            else
            {
                emplace_move(move_list, BoardLocation(piece_sq), BoardLocation(attack_sq));
            }
        }

        moving_pieces &= moving_pieces - 1;
    }

    return all_attacks;
}

uint64_t BitBoard::get_en_passant_pawn_moves(std::list<Move>& move_list, bool white_to_move, const std::unordered_map<uint8_t, uint64_t>& pinned_piece_allowed_moves) const
{
    // Flip the board vertically to calculate the black moves
    uint64_t friendly_pieces = white_to_move ? pieces_to_move(white_to_move) : mirror_vertical(pieces_to_move(white_to_move));

    uint64_t en_passant_from_row = 0x000000ff'00000000;
    uint64_t en_passant_from_sqs = en_passant_from_row & (0x00000002'80000000 << *m_en_passant_col);

    uint64_t moving_pieces = (white_to_move ? m_pawns : mirror_vertical(m_pawns)) & friendly_pieces & en_passant_from_sqs;

    int8_t forward = 8;

    uint64_t attacks = 0;

    while (moving_pieces)
    {
        auto piece_sq = bit_scan_forward(moving_pieces);

        // en passant capture
        uint64_t attacks = ((1ULL << piece_sq) & en_passant_from_sqs) ?
            (en_passant_from_row << forward) & (0x00000100'00010000 << *m_en_passant_col) : 0;

        // flip back attacks
        attacks = (white_to_move ? attacks : mirror_vertical(attacks)) & m_allowed_moves;

        // Scalar square vertical mirror
        piece_sq = white_to_move ? piece_sq : piece_sq ^ 56;
        
        if (attacks & (m_kings & pieces_to_move(!m_white_to_move)))
        {
            m_new_allowed_moves &= (1ULL << piece_sq);
        }

        // If pawn is pinned ensure it moves to allowed square during capture
        if (pinned_piece_allowed_moves.contains(piece_sq))
        {
            attacks &= pinned_piece_allowed_moves.at(piece_sq);
        }

        if (attacks)
        {                
            // If captured pawn is pinned and capturing piece won't move into piece cancel attack
            uint8_t capture_sq = bit_scan_forward(attacks) - forward;
            if (!(pinned_piece_allowed_moves.contains(capture_sq) && 
                !(pinned_piece_allowed_moves.at(capture_sq) & attacks)))
            {
                // There's only one possible ep capture per pawn
                auto& move = emplace_move(move_list, BoardLocation(piece_sq), BoardLocation(bit_scan_forward(attacks)));
                move.set_is_en_passant_capture();
            }
        }
        
        moving_pieces &= moving_pieces - 1;
    }

    return attacks;
}


uint64_t BitBoard::get_knight_moves(std::list<Move>& move_list, bool white_to_move, uint64_t pinned) const
{
    return get_moves(move_list, m_knights & ~pinned, white_to_move, [&](uint8_t sq) {

        uint64_t attacks = knight_attack_lut[sq];

        if (attacks & (m_kings & pieces_to_move(!white_to_move)))
        {
            m_new_allowed_moves &= (1ULL << sq);
        }

        return attacks; 
    });
}

uint64_t BitBoard::get_king_moves(std::list<Move>& move_list, bool white_to_move) const
{
    return get_moves(move_list, m_kings, white_to_move, [&](uint8_t sq) { 
        return king_attack_lut[sq] & ~m_opposite_attacks; 
    });
}

void BitBoard::get_castling_moves(std::list<Move>& move_list, bool white_to_move) const
{
    if (pieces_to_move(white_to_move) & m_kings & m_opposite_attacks)
    {
        // king is in check, no castling for him
        return;
    }

    // Check thaat king won't move through check to castle and add to list
    if (white_to_move)
    {
        if (has_castling_rights(CastlingRights::WHITE_KINGSIDE) && 
            !((m_occupied | m_opposite_attacks) & 0x00000000'00000060))
        {
            emplace_move(move_list, BoardLocation(4, 0), BoardLocation(6, 0));
        }
        if (has_castling_rights(CastlingRights::WHITE_QUEENSIDE) &&
            !((m_occupied | m_opposite_attacks) & 0x00000000'0000000c) &&
            !(m_occupied & 0x00000000'00000002))
        {
            emplace_move(move_list, BoardLocation(4, 0), BoardLocation(2, 0));
        }
    }
    else
    {
        
        if (has_castling_rights(CastlingRights::BLACK_KINGSIDE) &&
            !((m_occupied | m_opposite_attacks) & 0x60000000'00000000))
        {
            emplace_move(move_list, BoardLocation(4, 7), BoardLocation(6, 7));
        }
        if (has_castling_rights(CastlingRights::BLACK_QUEENSIDE) &&
            !((m_occupied | m_opposite_attacks) & 0x0c000000'00000000) &&
            !(m_occupied & 0x02000000'00000000))
        {
            emplace_move(move_list, BoardLocation(4, 7), BoardLocation(2, 7));
        }
    }
}


IBoard::Mate BitBoard::get_mate(PieceColour col) const
{
    return Mate();
}

PieceColour BitBoard::get_colour_to_move() const
{
    return m_white_to_move ? PieceColour::WHITE : PieceColour::BLACK;
}

void BitBoard::set_colour_to_move(PieceColour colour)
{
    m_white_to_move = colour == PieceColour::WHITE ? 1 : 0;
}

void BitBoard::delete_all_pieces()
{
}

void BitBoard::populate_squares_properties()
{

}

std::list<Move> BitBoard::get_all_legal_moves(PieceColour col) const
{
    std::list<Move> ret;

    bool white_to_move = col == PieceColour::WHITE;

    // if there's no king the game is over
    if (!(pieces_to_move(white_to_move) & m_kings))
    {
        return ret;
    }

    std::unordered_map<uint8_t, uint64_t> dummy;
    BitboardRayAttacks enemy_ray_attacks(*this, !white_to_move, dummy);

    m_opposite_attacks = 0;
    m_allowed_moves = 0xffffffff'ffffffff;

    uint64_t new_opposite_attacks = 0;
    m_new_allowed_moves = 0xffffffff'ffffffff;

    new_opposite_attacks |= get_knight_moves(ret, !white_to_move, 0);
    
    new_opposite_attacks |= enemy_ray_attacks.get_bishop_moves(ret);
    new_opposite_attacks |= enemy_ray_attacks.get_rook_moves(ret);
    new_opposite_attacks |= enemy_ray_attacks.get_queen_moves(ret);

    new_opposite_attacks |= enemy_ray_attacks.get_king_attacks();

    new_opposite_attacks |= get_pawn_moves(ret, !white_to_move, {});
    new_opposite_attacks |= get_king_moves(ret, !white_to_move);
    ret.clear();

    m_opposite_attacks = new_opposite_attacks;

    uint64_t pinned = enemy_ray_attacks.get_pinned();

    uint64_t test = enemy_ray_attacks.get_allowed_next_move_mask();
    m_allowed_moves = m_new_allowed_moves & enemy_ray_attacks.get_allowed_next_move_mask();


    BitboardRayAttacks friendly_ray_attacks(*this, white_to_move, enemy_ray_attacks.get_pinned_allowed());
    
    m_current_attacks = 0;

    m_current_attacks |= get_knight_moves(ret, white_to_move, pinned);

    m_current_attacks |= friendly_ray_attacks.get_bishop_moves(ret);
    m_current_attacks |= friendly_ray_attacks.get_rook_moves(ret);
    m_current_attacks |= friendly_ray_attacks.get_queen_moves(ret);

    m_current_attacks |= friendly_ray_attacks.get_king_attacks();

    m_current_attacks |= get_pawn_moves(ret, white_to_move, enemy_ray_attacks.get_pinned_allowed());
    if (m_en_passant_col.has_value() && !enemy_ray_attacks.is_enpassant_pinned())
    {
        m_current_attacks |= get_en_passant_pawn_moves(ret, white_to_move, enemy_ray_attacks.get_pinned_allowed());
    }

    // set m_allowed_moves so king can move out of check
    m_allowed_moves = ~m_opposite_attacks;

    m_current_attacks |= get_king_moves(ret, white_to_move);
    get_castling_moves(ret, white_to_move);

    return ret;
}

bool BitBoard::add_piece(PieceType type, PieceColour col, BoardLocation loc)
{    
    auto mask = loc.to_bitboard_mask();
    
    if (m_occupied & mask)
    {
        return false;
    }

    m_occupied |= mask;
    
    if (col == PieceColour::BLACK)
    {
        m_black_pieces |= mask;
    }
    else
    {
        m_white_pieces |= mask;
    }

    switch (type)
    {
    case PieceType::KING:
        m_kings |= mask;
        break;

    case PieceType::QUEEN:
        m_queens |= mask;
        break;

    case PieceType::ROOK:
        m_rooks |= mask;
        break;

    case PieceType::BISHOP:
        m_bishops |= mask;
        break;

    case PieceType::KNIGHT:
        m_knights |= mask;
        break;

    case PieceType::PAWN:
        m_pawns |= mask;
        break;
    }

    return true;
}

bool BitBoard::has_castling_rights(CastlingRights castling_rights) const
{
    return m_castling_rights & static_cast<uint8_t>(castling_rights);
};

void BitBoard::set_castling_rights(const std::vector<CastlingRights>& castling_rights)
{    
    for (const auto cr : castling_rights)
    {
        m_castling_rights |= static_cast<uint8_t>(cr);
    }
}

std::optional<uint8_t> BitBoard::get_enpassant_column() const
{
    return m_en_passant_col;
}

void BitBoard::set_enpassant_column(std::optional<uint8_t> col)
{
    m_en_passant_col = col;
}

bool BitBoard::make_move(const Move& move)
{
    const auto& new_loc = move.get_to_loc();
    const auto new_loc_mask = new_loc.to_bitboard_mask();

    const auto& curr_loc = move.get_from_loc();
    const auto curr_loc_mask = curr_loc.to_bitboard_mask();

    if (!(m_occupied & curr_loc_mask))
    {
        return false;
    }

    // Remove piece at new_loc
    uint64_t was_occupied = m_occupied;
    for (auto piece_set : { &m_occupied, &m_white_pieces, &m_black_pieces, &m_pawns, &m_knights, &m_bishops, &m_rooks, &m_queens, &m_kings })
    {
        if (*piece_set & new_loc_mask)
        {
            *piece_set &= ~new_loc_mask;
        }

        // Handle removal of castling rights if rook is taken
        if (new_loc_mask == 0x01000000'00000000)
        {
            m_castling_rights &= ~static_cast<uint8_t>(CastlingRights::BLACK_QUEENSIDE);
        }
        else if (new_loc_mask == 0x80000000'00000000)
        {
            m_castling_rights &= ~static_cast<uint8_t>(CastlingRights::BLACK_KINGSIDE);
        }
        else if (new_loc_mask == 0x00000000'00000001)
        {
            m_castling_rights &= ~static_cast<uint8_t>(CastlingRights::WHITE_QUEENSIDE);
        }
        else if (new_loc_mask == 0x80000000'00000080)
        {
            m_castling_rights &= ~static_cast<uint8_t>(CastlingRights::WHITE_KINGSIDE);
        }
    }

    // Move piece to new_loc
    for (auto piece_set : { &m_occupied, &m_white_pieces, &m_black_pieces, &m_knights, &m_bishops, &m_queens})
    {
        if (*piece_set & curr_loc_mask)
        {
            *piece_set ^= (new_loc_mask | curr_loc_mask);
        }
    }

    if (m_pawns & curr_loc_mask)
    {
        // en passant rules
        if (!(was_occupied & new_loc_mask) && (curr_loc.get_x() != new_loc.get_x()))
        {
            uint64_t capture_mask = ((m_white_to_move ? 0x00000001'00000000 : 0x00000000'01000000) << new_loc.get_x());
            m_pawns &= ~capture_mask;
            m_occupied &= ~capture_mask;
            if (m_white_to_move)
            {
                m_black_pieces &= ~capture_mask;
            }
            else
            {
                m_white_pieces &= ~capture_mask;
            }
        }

        if ((curr_loc_mask & 0x00ff0000'0000ff00) && (new_loc_mask & 0x000000ff'ff000000))
        {
            m_en_passant_col = curr_loc.get_x();
        }        
        
        m_pawns ^= (new_loc_mask | curr_loc_mask);
    }

    if (m_rooks & curr_loc_mask)
    {
        if (curr_loc_mask & 0x80000000'00000080)
        {
            m_castling_rights &= m_white_to_move ?
                ~(static_cast<uint8_t>(CastlingRights::WHITE_KINGSIDE)) :
                ~(static_cast<uint8_t>(CastlingRights::BLACK_KINGSIDE));
        }
        else if (curr_loc_mask & 0x01000000'00000001)
        {
            m_castling_rights &= m_white_to_move ?
                ~(static_cast<uint8_t>(CastlingRights::WHITE_QUEENSIDE)) :
                ~(static_cast<uint8_t>(CastlingRights::BLACK_QUEENSIDE));
        }

        m_rooks ^= (new_loc_mask | curr_loc_mask);
    }

    if (m_kings & curr_loc_mask)
    {
        // castling rules
        if (curr_loc_mask & 0x10000000'00000010)
        {
            // kings side
            uint64_t rook_mask = 0;

            if (new_loc_mask & 0x40000000'00000040)
            {
                rook_mask = (m_white_to_move ? 0x00000000'000000a0 : 0xa0000000'00000000);
            }
            // queens side
            if (new_loc_mask & 0x04000000'00000004)
            {
                rook_mask = (m_white_to_move ? 0x00000000'00000009 : 0x09000000'00000000);
            }
            m_rooks ^= rook_mask;
            m_occupied ^= rook_mask;
            if (m_white_to_move)
            {
                m_white_pieces ^= rook_mask;
            }
            else
            {
                m_black_pieces ^= rook_mask;
            }

            m_castling_rights &= m_white_to_move ?
                ~(static_cast<uint8_t>(CastlingRights::WHITE_KINGSIDE) |
                  static_cast<uint8_t>(CastlingRights::WHITE_QUEENSIDE)) :
                ~(static_cast<uint8_t>(CastlingRights::BLACK_KINGSIDE) |
                  static_cast<uint8_t>(CastlingRights::BLACK_QUEENSIDE));
        }

        m_kings ^= (new_loc_mask | curr_loc_mask);
    }

    // Check promotion
    if (move.get_promotion_type().has_value())
    {
        m_pawns &= ~new_loc_mask;

        switch (*move.get_promotion_type())
        {
        case Move::PromotionType::QUEEN:
            m_queens |= new_loc_mask;
            break;

        case Move::PromotionType::ROOK:
            m_rooks |= new_loc_mask;
            break;

        case Move::PromotionType::BISHOP:
            m_bishops |= new_loc_mask;
            break;

        case Move::PromotionType::KNIGHT:
            m_knights |= new_loc_mask;
            break;
        }
    }

    m_white_to_move = !m_white_to_move;

    if (!(m_occupied & new_loc_mask))
    {
        return false;
    }

    if ((m_white_pieces | m_black_pieces) != m_occupied)
    {
        return false;
    }
    return true;
}

bool BitBoard::unmake_move(const Move& move)
{
    const auto& new_loc = move.get_to_loc();
    const auto new_loc_mask = new_loc.to_bitboard_mask();

    const auto& curr_loc = move.get_from_loc();
    const auto curr_loc_mask = curr_loc.to_bitboard_mask();

    if (!(m_occupied & new_loc_mask))
    {
        return false;
    }

    // en passant rules
    if (move.is_en_passant_capture())
    {
        m_en_passant_col = new_loc.get_x();

        uint64_t capture_mask = ((m_white_to_move ? 0x00000000'01000000 : 0x00000001'00000000) << new_loc.get_x());
        m_pawns |= capture_mask;
        m_occupied |= capture_mask;

        if (m_white_to_move)
        {
            m_white_pieces |= capture_mask;
        }
        else
        {
            m_black_pieces |= capture_mask;
        }
    }
    else
    {
        m_en_passant_col = std::nullopt;
    }

    // Move piece back to curr_loc
    for (auto piece_set : { &m_occupied, &m_white_pieces, &m_black_pieces })
    {
        if (*piece_set & new_loc_mask)
        {
            *piece_set ^= (new_loc_mask | curr_loc_mask);
        }
    }

    // Move piece back to curr_loc - if this is a promotion we just remove the piece
    for (auto piece_set : { &m_pawns, &m_knights, &m_bishops, &m_rooks, &m_queens })
    {
        if (*piece_set & new_loc_mask)
        {
            *piece_set ^= (move.get_promotion_type().has_value()) ? new_loc_mask : (new_loc_mask | curr_loc_mask);
        }
    }

    if (m_kings & new_loc_mask)
    {
        // castling rules
        if (curr_loc_mask & 0x10000000'00000010)
        {
            // kings side
            uint64_t rook_mask = 0;

            if (new_loc_mask & 0x40000000'00000040)
            {
                rook_mask = (!m_white_to_move ? 0x00000000'000000a0 : 0xa0000000'00000000);
            }
            // queens side
            if (new_loc_mask & 0x04000000'00000004)
            {
                rook_mask = (!m_white_to_move ? 0x00000000'00000009 : 0x09000000'00000000);
            }
            m_rooks ^= rook_mask;
            m_occupied ^= rook_mask;
            if (!m_white_to_move)
            {
                m_white_pieces ^= rook_mask;
            }
            else
            {
                m_black_pieces ^= rook_mask;
            }
        }

        m_kings ^= (new_loc_mask | curr_loc_mask);
    }

    // Check castling
    m_castling_rights = move.get_old_castling_rights();

    if (move.get_promotion_type().has_value())
    {
        m_pawns |= curr_loc_mask;
    }

    // Replace captured piece
    auto captured_piece_type = move.get_captured_piece_type();

    // For en passant captures we've already replaced the captured piece
    if (captured_piece_type.has_value() && !move.is_en_passant_capture())
    {    
        switch (captured_piece_type.value())
        {
        case PieceType::PAWN:
            m_pawns |= new_loc_mask;
            break;

        case PieceType::KNIGHT:
            m_knights |= new_loc_mask;
            break;

        case PieceType::BISHOP:
            m_bishops |= new_loc_mask;
            break;

        case PieceType::ROOK:
            m_rooks |= new_loc_mask;
            break;

        case PieceType::QUEEN:
            m_queens |= new_loc_mask;
            break;

        case PieceType::KING:
            m_kings |= new_loc_mask;
            break;
        }

        m_occupied |= new_loc_mask;
        if (m_white_to_move)
        {
            m_white_pieces |= new_loc_mask;
        }
        else
        {
            m_black_pieces |= new_loc_mask;
        }
    }

    m_white_to_move = !m_white_to_move;

    if (!(m_occupied & curr_loc_mask) || !(captured_piece_type.has_value() ? (m_occupied & new_loc_mask) : 1))
    {
        return false;
    }

    if ((m_white_pieces | m_black_pieces) != m_occupied)
    {
        return false;
    }

    return true;
}

std::unique_ptr<IBoard> BitBoard::clone() const
{
    return std::unique_ptr<IBoard>();
}

std::unique_ptr<IBoard> BitBoard::clone_moved(const Move& move) const
{
    return std::unique_ptr<IBoard>();
}

void BitBoard::for_all_pieces(std::function<void(const Piece& piece)> fn) const
{
}

std::shared_ptr<const Piece> BitBoard::piece_on_square(int x, int y) const
{
    return std::shared_ptr<const Piece>();
}
