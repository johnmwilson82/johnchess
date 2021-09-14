#include "move.h"

#include "pieces.h"

Move::Move(const Piece &piece, const DynMove& dm) :
    m_piece(piece),
    m_new_loc(piece.get_loc().apply_move(dm))
{}

Move::Move(const Piece &piece, const BoardLocation& new_loc) :
    m_piece(piece),
    m_new_loc(new_loc)
{
}

bool Move::operator== (const Move& m) const
{
    return (std::addressof(m_piece) == std::addressof(m.m_piece)) && (m_new_loc == m.m_new_loc);
}

std::string Move::to_string() const
{
    return m_piece.get_loc().to_string() + m_new_loc.to_string();
}

const BoardLocation& Move::get_to_loc() const
{
    return m_new_loc;
}

const BoardLocation Move::get_from_loc() const
{
    return m_piece.get_loc();
}

size_t MoveHashFunction::operator()(const Move& move) const
{
    return  move.m_piece.get_loc().get_x() + 
            move.m_piece.get_loc().get_y() * BOARD_DIM +
            move.m_new_loc.get_x() * BOARD_DIM * BOARD_DIM + 
            move.m_new_loc.get_y() * BOARD_DIM * BOARD_DIM * BOARD_DIM;
}