#pragma once

#include <vector>
#include <string>
#include <iostream>


class BitBoard;
class DynMove;
class Piece;

constexpr uint8_t BOARD_DIM = 8;

/*! /brief Board location
 *
 * Defines a location on (or off) a given board, each piece will have a
 * BoardLocation. BoardLocations are used to perform moves on and
 * have flags to define whether the piece who's location they represent
 * are on the board or not, and whether the result of any moves is valid
 * or not.
 */
class BoardLocation
{
public: //ctor + dtor
    BoardLocation(const BoardLocation& boardloc);
    BoardLocation(uint8_t bitboard_sq);

    BoardLocation(const std::string& loc_str);
    BoardLocation(int x, int y);

    BoardLocation& operator=(const BoardLocation& other)
    {
        m_x = other.m_x;
        m_y = other.m_y;

        return *this;
    }

public: // methods
    //! Equals operator
    bool operator== (const BoardLocation& bl) const
    {
        return m_x == bl.m_x && m_y == bl.m_y;
    }

    //! Return string representation of BoardLocation (using standard notation)
    std::string to_string() const
    {
        std::string ret = "a1";
        ret[0] += m_x;
        ret[1] += m_y;
        return ret;
    }

    uint64_t to_bitboard_mask() const
    {
        return 1ULL << (m_y * 8 + m_x);
    }

public: // inlines

    //! Get board column
    /*!
     * \return 0 for col a -> 7 for col h
     */
    inline int get_x() const { return m_x; }

    //! Get board row
    /*!
     * \return 0 for row 1 -> 7 for row 8
     */
    inline int get_y() const { return m_y; }

private: // properties
    int m_x, m_y;
};
