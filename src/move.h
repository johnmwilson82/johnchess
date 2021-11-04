#pragma once

#include <memory>
#include <optional>

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
    enum class PromotionType {
        QUEEN,
        ROOK,
        BISHOP,
        KNIGHT
    };

    Move(const Piece& piece, const DynMove& dm);
    Move(const Piece& piece, const BoardLocation& new_loc);
    Move(const Board& board, const std::string& move_str);

    bool operator== (const Move& m) const;
    
    std::string to_string() const;

    const BoardLocation& get_to_loc() const;
    const BoardLocation get_from_loc() const;
    const std::optional<PromotionType> get_queening_type() const { return m_queening_type; }

private:
    BoardLocation m_new_loc;
    const Piece& m_piece;
    std::optional<PromotionType> m_queening_type;
};

class MoveHashFunction
{
public:
    size_t operator()(const Move& move) const;
};