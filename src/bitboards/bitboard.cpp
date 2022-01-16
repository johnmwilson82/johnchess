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
    m_black_pieces(0),
    m_occupied(0)
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
    m_occupied = 0xffff0000'0000ffff;
}

void BitBoard::set_from_edit_mode(std::vector<std::string> edit_mode_strings)
{
}

bool BitBoard::get_in_check(PieceColour col) const
{
    return false;
}

uint64_t BitBoard::pieces_to_move() const
{
    return m_occupied ^ (m_black_pieces * m_white_to_move);
}

void BitBoard::get_knight_moves(std::list<Move>& move_list) const
{
    uint64_t knights = m_knights & pieces_to_move();

    while (knights)
    {
        auto knight_sq = bit_scan_forward(knights);

        uint64_t attacks = knight_attack_lut[knight_sq];

        while (attacks)
        {
            auto attack_sq = bit_scan_forward(attacks);
            attacks &= attacks - 1;
            move_list.emplace_back(BoardLocation(knight_sq), BoardLocation(attack_sq));
        }

        knights &= knights - 1;
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
    get_knight_moves(ret);

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
    return std::optional<uint8_t>();
}

void BitBoard::set_enpassant_column(std::optional<uint8_t> col)
{

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
