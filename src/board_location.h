#pragma once

#include <string>


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
        m_loc = other.m_loc;

        return *this;
    }

public: // methods
    //! Equals operator
    inline bool operator== (const BoardLocation& bl) const
    {
        return m_loc == bl.m_loc;
    }

    //! Return string representation of BoardLocation (using standard notation)
    std::string to_string() const
    {
        std::string ret = "a1";
        ret[0] += get_x();
        ret[1] += get_y();
        return ret;
    }

    inline uint64_t to_bitboard_mask() const
    {
        return 1ULL << m_loc;
    }

    inline uint8_t get_raw() const
    {
        return m_loc;
    }

public: // inlines

    //! Get board column
    /*!
     * \return 0 for col a -> 7 for col h
     */
    inline int get_x() const { return m_loc & 0x7; }

    //! Get board row
    /*!
     * \return 0 for row 1 -> 7 for row 8
     */
    inline int get_y() const { return m_loc >> 3; }

private: // properties
    uint8_t m_loc; // == y * 8 + x;
};
