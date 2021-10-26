#pragma once

#include <optional>
#include <functional>

#include "pieces.h"

/*! /brief Represents a square on a board
 *
 * This class holds information on each square, whether it's occupied
 * and what piece, if any, is occupying it.
 */
class Square
{
public:
    Square()
    { }

    Square(std::shared_ptr<Piece> piece) :
        m_piece(piece)
    { }

    //! Return true is the square is empty, false if occupied
    inline bool is_empty() const
    {
        return !m_piece;
    }

    //! Return a ref to the occupying piece or NULL
    inline std::shared_ptr<Piece> get_piece() const
    {
        return m_piece;
    }

    //! Remove any occupying piece from this square
    inline void remove_piece()
    {
        m_piece.reset();
    }

    //! Set the occupying piece on this square
    inline void set_piece(std::shared_ptr<Piece> piece)
    {
        m_piece = piece;
    }

    inline void add_attacker(std::shared_ptr<Piece> attacker)
    {
        if(attacker->get_colour() == Piece::WHITE)
        {
            m_white_attackers.push_back(attacker);
        }
        else
        {
            m_black_attackers.push_back(attacker);
        }
    }

    inline const std::vector<std::shared_ptr<Piece>>& get_attackers(Piece::Colour col) const
    {
        return (col == Piece::WHITE) ? m_white_attackers : m_black_attackers;
    }
    
private:
    std::shared_ptr<Piece> m_piece;
    std::vector<std::shared_ptr<Piece>> m_white_attackers;
    std::vector<std::shared_ptr<Piece>> m_black_attackers;
};
