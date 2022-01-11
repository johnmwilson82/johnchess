#include "bitboard.h"

void BitBoard::set_to_start_position()
{
}

void BitBoard::set_from_edit_mode(std::vector<std::string> edit_mode_strings)
{
}

bool BitBoard::get_in_check(PieceColour col) const
{
    return false;
}

IBoard::Mate BitBoard::get_mate(PieceColour col) const
{
    return Mate();
}

PieceColour BitBoard::get_colour_to_move() const
{
    return PieceColour();
}

void BitBoard::set_colour_to_move(PieceColour colour)
{
}

void BitBoard::delete_all_pieces()
{
}

void BitBoard::populate_squares_properties()
{
}

std::list<Move> BitBoard::get_all_legal_moves(PieceColour col) const
{
    return std::list<Move>();
}

bool BitBoard::add_piece(PieceType type, PieceColour col, BoardLocation loc)
{
    return false;
}

bool BitBoard::has_castling_rights(CastlingRights castling_rights) const
{
    return false;
}

std::optional<uint8_t> BitBoard::get_enpassant_column() const
{
    return std::optional<uint8_t>();
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
