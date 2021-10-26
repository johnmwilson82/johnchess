#pragma once

#include <vector>
#include <string>
#include <iostream>

class Board;
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
    BoardLocation(const std::string& loc_str, const Board& board);
    BoardLocation(int x, int y, const Board& board);
    ~BoardLocation();

    BoardLocation& operator=(const BoardLocation& other)
    {
        m_x = other.m_x;
        m_y = other.m_y;
        m_on_board = other.m_on_board;
        m_valid = other.m_valid;
        
        return *this;
    }

public: // methods
    //! Apply a move to a BoardLocation
    /*!
     * \param dm Relative move to be added to the current location
     * \return New BoardLocation with the relative move applied
     */
    BoardLocation apply_move(const DynMove& dm) const;

    //! Apply a move to a BoardLocation
    /*!
     * \param dx Relative shift in columns
     * \param dy Relative shift in rows
     * \return New BoardLocation with the relative move applied
     */
    BoardLocation apply_move(int dx, int dy) const;

    //! Apply a move in-place to a BoardLocation
    /*!
     * \param dm Relative move to be added to the current location
     * \return true is move if valid and resultant position is on the board, false otherwise
     */
    bool apply_move_inplace(const DynMove& dm);

    //! Apply a move in-place to a BoardLocation
    /*!
     * \param dx Relative shift in columns
     * \param dy Relative shift in rows
     * \return true is move if valid and resultant position is on the board, false otherwise
     */
    bool apply_move_inplace(int dx, int dy);

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

    //! Return true if BoardLocation is on the board
    inline bool get_on_board() const { return m_on_board; }

    //! Return true if BoardLocation is a valid location
    inline bool get_valid() const { return m_valid; }

    //! Set whether the BoardLocation is on the board or not (e.g. captured piece)
    inline void set_on_board(bool val) { m_on_board = val; }

    //! Set whether the BoardLocation is on the board or not (e.g. captured piece)
    inline bool get_on_board() {return m_on_board; }

private:
    void register_captured(Piece* piece);

private: // properties
    int m_x, m_y;
    bool m_on_board, m_valid;
    const Board& m_board;
};
