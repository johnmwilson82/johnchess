#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>
#include <iostream>

class Board;
class DynMove;
class Piece;

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
    BoardLocation();
    BoardLocation(const BoardLocation& boardloc);
    BoardLocation(std::string loc_str, Board* board);
    BoardLocation(int x, int y, Board* board);
    ~BoardLocation();

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

private:
    void register_captured(Piece* piece);

private: // properties
    int m_x, m_y;
    bool m_on_board, m_valid;
    Board* m_board;
};

#include "pieces.h"

/*! /brief Represents a square on a board
 *
 * This class holds information on each square, whether it's occupied
 * and what piece, if any, is occupying it.
 */
class Square
{
public:
    Square() :
        m_piece(NULL)
    { }

    Square(Piece* piece) :
        m_piece(piece)
    { }

    //! Return true is the square is empty, false if occupied
    inline bool is_empty() const
    {
        return m_piece == NULL ? true : false;
    }

    //! Return a pointer to the occupying piece or NULL
    inline Piece* get_piece() const
    {
        return m_piece;
    }

    //! Remove any occupying piece from this square
    inline void remove_piece()
    {
        m_piece = NULL;
    }

    //! Set the occupying piece on this square
    inline void set_piece(Piece* piece)
    {
        m_piece = piece;
    }

private:
    Piece* m_piece;
};

/*! /brief Represents a chess board
 *
 * This class represents the chess board itself and is used to hold
 * instantaneous position data for a game
 */
class Board
{
public:
    enum Mate {
        CHECKMATE,
        STALEMATE,
        NO_MATE
    };

public:
    Board(int dim_x, int dim_y);
    ~Board()
    {
        delete_all_pieces();
    }
    Board(const Board& board);

    //! Set the board to the starting position
    /*!
     * Calling this function resets all the pieces and squares on a board and sets
     * all the pieces to their starting positions
     */
    void set_to_start_position();

    //! Return a ref to the square at the given x, y coordinates (0-7)
    inline Square& square(int x, int y)
    {
        return m_squares[x * m_dim_x + y];
    }

    //! Return a ref to the square at the given BoardLocation
    inline const Square& square(BoardLocation loc) const
    {
        return m_squares[loc.get_x() * m_dim_x + loc.get_y()];
    }

    //! Return a ref to the square at the given BoardLocation
    inline Square& square(BoardLocation loc)
    {
        return m_squares[loc.get_x() * m_dim_x + loc.get_y()];
    }

    //! Return true if the given x, y coordinates are on the board, false otherwise
    inline bool on_board(int x, int y) {
        return ((x >= 0) && (x < m_dim_x) && (y >= 0) && (y < m_dim_y)) ? true : false;
    }

    //! Apply a piece to the board at a given location
    /*!
     * \param type chess piece type
     * \param col chess piece colour
     * \param loc location as a string in standard notation
     * \return true on success
     */
    bool add_piece(Piece::Type type, Piece::Colour col, std::string loc);

    //! Apply a piece to the board at a given location
    /*!
     * \param type chess piece type
     * \param col chess piece colour
     * \param loc location as a BoardLocation
     * \return true on success
     */
    bool add_piece(Piece::Type type, Piece::Colour col, BoardLocation loc);

    //! Remove a piece from the board where location is given as a string in standard notation
    /*!
     * \return true on success and if there was a piece to remove
     */
    bool remove_piece(std::string loc);

    //! Remove a piece from the board where location is given as a BoardLocation
    /*!
     * \return true on success and if there was a piece to remove
     */
    bool remove_piece(BoardLocation loc);

    //! Move a piece on the board where move is given as a string in standard notation (e.g. a2a4)
    /*!
     * \return true on success and if there was a piece to remove
     */
    bool move_piece(std::string move_str, bool check_test=true);

    //! Move a piece on the board where move is defined by a current and new BoardLocation
    /*!
     * \return true on success and if there was a piece to remove
     */
    bool move_piece(BoardLocation curr_loc, BoardLocation new_loc, bool check_test=true);

    //! Return the number of columns
    int get_dim_x() { return m_dim_x; }

    //! Return the number of rows
    int get_dim_y() { return m_dim_y; }

    //! Load a board position from XBoard edit mode commands
    /*!
     * \param edit_mode_string vector of strings constituting the edit mode strings
     *                         (i.e. from 'edit' to '.')
     */
    void set_from_edit_mode(std::vector<std::string> edit_mode_strings);

    //! Return whether this board is in check
    /*!
     * \return true if either side is in check, false otherwise
     */
    inline bool get_in_check()
    {
        return get_in_check(Piece::WHITE) || get_in_check(Piece::BLACK);
    }

    //! Return whether given colour's king is in check
    /*!
     * \param col colour to test for check
     * \return true if parameter colour is in check, false otherwise
     */
    bool get_in_check(Piece::Colour col, bool get_valid_move_check_test=false);

    //! Return if/what type of mate the board it in
    /*!
     * \param col colour to test for being in mate
     * \return CHECKMATE, STALEMATE or NO_MATE
     */
    Mate get_mate(Piece::Colour col)
    {
        std::cout << "checking mate for colour" << (int) col << std::endl;;
        if(get_num_available_moves(col) == 0)
        {
            if (get_in_check(col))
                return CHECKMATE;
            else
                return STALEMATE;
        }
        return NO_MATE;
    }

    //! Return the number of available moves for a given colour
    int get_num_available_moves(Piece::Colour col);

    //! Return the colour whose move it is
    Piece::Colour get_colour_to_move()
    {
        return m_colour_to_move;
    }

private:
    //! Delete all the pieces from the board
    void delete_all_pieces();

    void register_captured(Piece* piece);

private:
    int m_dim_x, m_dim_y;
    std::vector<Square> m_squares; //!< board squares
    //std::vector< std::vector<Square*> > m_rows;
    //std::vector< std::vector<Square*> > m_cols;
    std::vector<Piece*> m_pieces;  //!< pieces on the board
    Piece::Colour m_colour_to_move;

};


#endif //BOARD_H
