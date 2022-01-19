#include "bitboard.h"

#include "bitboard_utils.h"

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

uint64_t BitBoard::get_ray_mask(uint8_t sq, RayDir dir) const
{
    return ray_masks[static_cast<size_t>(dir)][sq];
}

uint64_t BitBoard::get_positive_ray_attacks(uint8_t sq, RayDir dir, bool white_to_move) const
{
    uint64_t blocked_ray = m_occupied & get_ray_mask(sq, dir);
    return get_ray_mask(sq, dir) ^ ((blocked_ray == 0) ? 0 : get_ray_mask(bit_scan_forward(blocked_ray), dir));
}

uint64_t BitBoard::get_negative_ray_attacks(uint8_t sq, RayDir dir, bool white_to_move) const
{
    uint64_t blocked_ray = m_occupied & get_ray_mask(sq, dir);
    return get_ray_mask(sq, dir) ^ ((blocked_ray == 0) ? 0 : get_ray_mask(bit_scan_reverse(blocked_ray), dir));
}

uint64_t BitBoard::get_ray_attacks(uint8_t sq, RayDir dir, bool white_to_move) const
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

    return ret & ~pieces_to_move(white_to_move);
}

void BitBoard::get_moves(std::list<Move>& move_list, uint64_t pieces, bool white_to_move, std::function<uint64_t(uint8_t)> attacks_fn) const
{
    uint64_t friendly_pieces = pieces_to_move(white_to_move);
    uint64_t moving_pieces = pieces & friendly_pieces;

    while (moving_pieces)
    {
        auto piece_sq = bit_scan_forward(moving_pieces);

        uint64_t attacks = attacks_fn(piece_sq);

        attacks &= ~friendly_pieces;

        while (attacks)
        {
            auto attack_sq = bit_scan_forward(attacks);
            attacks &= attacks - 1;
            move_list.emplace_back(BoardLocation(piece_sq), BoardLocation(attack_sq));
        }

        moving_pieces &= moving_pieces - 1;
    }
}

void BitBoard::get_pawn_moves(std::list<Move>& move_list, bool white_to_move) const
{
    uint64_t friendly_pieces = pieces_to_move(white_to_move);
    uint64_t enemy_pieces = pieces_to_move(!white_to_move);

    uint64_t moving_pieces = m_pawns & friendly_pieces;
    
    uint64_t first_rank = white_to_move ? 0x00000000'0000ff00 : 0x00ff0000'00000000;
    uint64_t last_rank = white_to_move ? 0xff000000'00000000 : 0x00000000'000000ff;

    int8_t forward = white_to_move ? 8 : -8;

    uint64_t en_passant_from_row = white_to_move ? 0x000000ff'00000000 : 0x00000000'ff000000;
    uint64_t en_passant_from_sqs = m_en_passant_col.has_value() ?
        en_passant_from_row & ((white_to_move ? 0x00000002'80000000 : 0x00000000'02800000) << *m_en_passant_col) : 0;

    while (moving_pieces)
    {
        auto piece_sq = bit_scan_forward(moving_pieces);

        //forward_moves
        uint64_t attacks = 1ULL << (piece_sq + forward) | ((first_rank & (1ULL << piece_sq)) << (2 * forward));
        attacks &= ~(m_occupied | ((m_occupied & ~(1ULL << piece_sq)) << forward));

        // capture left
        uint64_t left_attack = ((1ULL << piece_sq) & 0x00010101'01010100) ? 0 : 1ULL << (piece_sq + forward - 1);
        attacks |= enemy_pieces & left_attack;

        // capture right
        uint64_t right_attack = ((1ULL << piece_sq) & 0x00808080'80808000) ? 0 : 1ULL << (piece_sq + forward + 1);
        attacks |= enemy_pieces & right_attack;

        // en passant capture
        /*uint64_t test = ((1ULL << piece_sq) & en_passant_from_sqs);
        uint64_t test3 = (en_passant_from_row << forward);
        uint64_t test2 = (en_passant_from_row << forward) & (0x00000100'00010000 << *m_en_passant_col);*/

        attacks |= ((1ULL << piece_sq) & en_passant_from_sqs) ?
            (en_passant_from_row << forward) & (0x00000100'00010000 << *m_en_passant_col) : 0;

        attacks &= ~friendly_pieces;

        while (attacks)
        {
            auto attack_sq = bit_scan_forward(attacks);
            attacks &= attacks - 1;
            move_list.emplace_back(BoardLocation(piece_sq), BoardLocation(attack_sq));
        }

        moving_pieces &= moving_pieces - 1;
    }
}

void BitBoard::get_knight_moves(std::list<Move>& move_list, bool white_to_move) const
{
    return get_moves(move_list, m_knights, white_to_move, [](uint8_t sq) { return knight_attack_lut[sq]; });
}

void BitBoard::get_king_moves(std::list<Move>& move_list, bool white_to_move) const
{
    return get_moves(move_list, m_kings, white_to_move, [](uint8_t sq) { return king_attack_lut[sq]; });
}

void BitBoard::get_bishop_moves(std::list<Move>& move_list, bool white_to_move) const
{
    return get_moves(move_list, m_bishops, white_to_move, [&](uint8_t sq) { 
        return get_ray_attacks(sq, RayDir::NE, white_to_move) |
               get_ray_attacks(sq, RayDir::SE, white_to_move) |
               get_ray_attacks(sq, RayDir::NW, white_to_move) |
               get_ray_attacks(sq, RayDir::SW, white_to_move);
    });
}

void BitBoard::get_rook_moves(std::list<Move>& move_list, bool white_to_move) const
{
    return get_moves(move_list, m_rooks, white_to_move, [&](uint8_t sq) {
        return get_ray_attacks(sq, RayDir::N, white_to_move) |
               get_ray_attacks(sq, RayDir::S, white_to_move) |
               get_ray_attacks(sq, RayDir::E, white_to_move) |
               get_ray_attacks(sq, RayDir::W, white_to_move);
    });
}

void BitBoard::get_queen_moves(std::list<Move>& move_list, bool white_to_move) const
{
    return get_moves(move_list, m_queens, white_to_move, [&](uint8_t sq) {
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
    get_knight_moves(ret, col == PieceColour::WHITE);
    get_king_moves(ret, col == PieceColour::WHITE);
    get_bishop_moves(ret, col == PieceColour::WHITE);
    get_rook_moves(ret, col == PieceColour::WHITE);
    get_queen_moves(ret, col == PieceColour::WHITE);
    get_pawn_moves(ret, col == PieceColour::WHITE);
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
    return false;
}

void BitBoard::set_castling_rights(const std::vector<CastlingRights>& castling_rights)
{
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
