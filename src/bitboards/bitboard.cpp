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
    m_white_to_move(1)
{}

void BitBoard::set_to_start_position()
{
    m_pawns   = 0x00ff0000'0000ff00;
    m_knights = 0x42000000'00000042;
    m_bishops = 0x24000000'00000024;
    m_rooks   = 0x81000000'00000081;
    m_queens  = 0x10000000'00000010;
    m_kings   = 0x08000000'00000010;

    m_black_pieces = 0xffff0000'00000000;
    m_white_pieces = 0x00000000'0000ffff;

    m_occupied = 0xffff0000'0000ffff;
}

void BitBoard::set_from_edit_mode(std::vector<std::string> edit_mode_strings)
{
}

bool BitBoard::get_in_check(PieceColour col) const
{
    return false;
}

uint64_t BitBoard::pieces_to_move(bool white_to_move) const
{
    return m_white_pieces * white_to_move | m_black_pieces * !white_to_move;
}

uint64_t BitBoard::get_moves(std::list<Move>& move_list, uint64_t pieces, bool white_to_move, std::function<uint64_t(uint8_t)> attacks_fn) const
{
    uint64_t friendly_pieces = pieces_to_move(white_to_move);
    uint64_t moving_pieces = pieces & friendly_pieces;
    uint64_t attacks = 0;

    while (moving_pieces)
    {
        auto piece_sq = bit_scan_forward(moving_pieces);

        attacks |= attacks_fn(piece_sq);

        attacks &= ~friendly_pieces;

        uint64_t attacks_to_add = attacks;

        while (attacks_to_add)
        {
            auto attack_sq = bit_scan_forward(attacks_to_add);
            attacks_to_add &= attacks_to_add - 1;
            move_list.emplace_back(BoardLocation(piece_sq), BoardLocation(attack_sq));
        }

        moving_pieces &= moving_pieces - 1;
    }

    return attacks;
}

uint64_t BitBoard::get_pawn_moves(std::list<Move>& move_list, bool white_to_move, uint64_t pinned) const
{
    // Flip the board vertically to calculate the black moves
    uint64_t friendly_pieces = white_to_move ? pieces_to_move(white_to_move) : mirror_vertical(pieces_to_move(white_to_move));
    uint64_t occupied = white_to_move ? m_occupied : mirror_vertical(m_occupied);
    uint64_t enemy_pieces = occupied ^ friendly_pieces;

    uint64_t moving_pieces = (white_to_move ? m_pawns : mirror_vertical(m_pawns)) & friendly_pieces;
    moving_pieces &= ~pinned;

    uint64_t first_rank = 0x00000000'0000ff00;

    int8_t forward = 8;

    uint64_t en_passant_from_row = 0x000000ff'00000000;
    uint64_t en_passant_from_sqs = m_en_passant_col.has_value() ?
        en_passant_from_row & (0x00000002'80000000 << *m_en_passant_col) : 0;

    uint64_t attacks = 0;

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

        // en passant capture
        attacks |= ((1ULL << piece_sq) & en_passant_from_sqs) ?
            (en_passant_from_row << forward) & (0x00000100'00010000 << *m_en_passant_col) : 0;

        // attacks can't move into friendly pieces
        attacks &= ~friendly_pieces;

        // flip back attacks
        attacks = white_to_move ? attacks : mirror_vertical(attacks);

        // Scalar square vertical mirror
        piece_sq = white_to_move ? piece_sq : piece_sq ^ 56;

        uint64_t attacks_to_add = attacks;

        while (attacks_to_add)
        {
            auto attack_sq = bit_scan_forward(attacks_to_add);
            attacks_to_add &= attacks_to_add - 1;
            
            if ((1ULL << attack_sq) & 0xff000000'000000ff)
            {
                for (auto promote_to : { Move::PromotionType::QUEEN, Move::PromotionType::ROOK, Move::PromotionType::BISHOP, Move::PromotionType::KNIGHT })
                {
                    auto& move = move_list.emplace_back(BoardLocation(piece_sq), BoardLocation(attack_sq));
                    move.set_promotion_type(promote_to);
                }
            }
            else
            {
                move_list.emplace_back(BoardLocation(piece_sq), BoardLocation(attack_sq));
            }
        }

        moving_pieces &= moving_pieces - 1;
    }

    return attacks;
}

uint64_t BitBoard::get_knight_moves(std::list<Move>& move_list, bool white_to_move, uint64_t pinned) const
{
    return get_moves(move_list, m_knights & ~pinned, white_to_move, [](uint8_t sq) { return knight_attack_lut[sq]; });
}

uint64_t BitBoard::get_king_moves(std::list<Move>& move_list, bool white_to_move, uint64_t opposite_attacks) const
{
    return get_moves(move_list, m_kings, white_to_move, [opposite_attacks](uint8_t sq) { return king_attack_lut[sq] & ~opposite_attacks; });
}

void BitBoard::get_castling_moves(std::list<Move>& move_list, bool white_to_move, uint64_t opposite_attacks) const
{
    // Todo: check for pieces attacking given squares
    if (white_to_move)
    {
        if (has_castling_rights(CastlingRights::WHITE_KINGSIDE) && 
            !((m_occupied | opposite_attacks) & 0x00000000'00000060))
        {
            move_list.emplace_back(BoardLocation(4, 0), BoardLocation(6, 0));
        }
        if (has_castling_rights(CastlingRights::WHITE_QUEENSIDE) &&
            !((m_occupied | opposite_attacks) & 0x00000000'0000000e))
        {
            move_list.emplace_back(BoardLocation(4, 0), BoardLocation(2, 0));
        }
    }
    else
    {
        
        if (has_castling_rights(CastlingRights::BLACK_KINGSIDE) &&
            !((m_occupied | opposite_attacks) & 0x60000000'00000000))
        {
            move_list.emplace_back(BoardLocation(4, 7), BoardLocation(6, 7));
        }
        if (has_castling_rights(CastlingRights::BLACK_QUEENSIDE) &&
            !((m_occupied | opposite_attacks) & 0x0e000000'00000000))
        {
            move_list.emplace_back(BoardLocation(4, 7), BoardLocation(2, 7));
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

    BitboardRayAttacks enemy_ray_attacks(*this);

    uint64_t opp_attacks = 0;
    opp_attacks |= get_knight_moves(ret, !white_to_move, 0);
    opp_attacks |= get_king_moves(ret, !white_to_move, 0);
    opp_attacks |= enemy_ray_attacks.get_bishop_moves(ret, !white_to_move, 0);
    opp_attacks |= enemy_ray_attacks.get_rook_moves(ret, !white_to_move, 0);
    opp_attacks |= enemy_ray_attacks.get_queen_moves(ret, !white_to_move, 0);
    opp_attacks |= get_pawn_moves(ret, !white_to_move, 0);

    ret.clear();

    uint64_t pinned = enemy_ray_attacks.get_pinned();

    BitboardRayAttacks friendly_ray_attacks(*this);

    get_knight_moves(ret, white_to_move, pinned);
    get_king_moves(ret, white_to_move, opp_attacks);
    friendly_ray_attacks.get_bishop_moves(ret, white_to_move, pinned);
    friendly_ray_attacks.get_rook_moves(ret, white_to_move, pinned);
    friendly_ray_attacks.get_queen_moves(ret, white_to_move, pinned);
    get_pawn_moves(ret, white_to_move, pinned);
    get_castling_moves(ret, white_to_move, opp_attacks);

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
    return false;
}

bool BitBoard::unmake_move(const Move& move)
{
    return false;
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
