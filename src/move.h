#pragma once

#include <memory>

#include "board_location.h"

class Piece;

class DynMove
{
// DynMove: Describes the dynamics of a move (where a piece moves to 
// relative to it's current position)
public:
    DynMove(int dx, int dy) :
        m_dx(dx), m_dy(dy)
    {
    }
    inline int get_dx() const { return m_dx; }
    inline int get_dy() const { return m_dy; }

private:
    int m_dx, m_dy;
};

class MoveHashFunction;

class Move
{
    friend class MoveHashFunction;

// Move: Describes a move, which is the new location of a given piece
public:
    Move(const Piece& piece, const DynMove& dm);
    Move(const Piece& piece, const BoardLocation& new_loc);

    bool operator== (const Move& m) const;
    
    std::string to_string() const;

    const BoardLocation& get_to_loc() const;
    const BoardLocation get_from_loc() const;

private:
    BoardLocation m_new_loc;
    const Piece& m_piece;
};

class MoveHashFunction
{
public:
    size_t operator()(const Move& move) const;
};