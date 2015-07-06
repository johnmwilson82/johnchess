#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>

class Board;
class DynMove;
class Piece;

class BoardLocation
{
public: //ctor + dtor
    BoardLocation();
    BoardLocation(const BoardLocation& board);
    BoardLocation(std::string loc_str, Board* board);
    BoardLocation(int x, int y, Board* board);
    ~BoardLocation();

public: // methods
    BoardLocation apply_move(DynMove& dm);
    BoardLocation apply_move(int dx, int dy);
    bool apply_move_inplace(int dx, int dy);

    bool operator== (BoardLocation& bl)
    {
        return m_x == bl.m_x && m_y == bl.m_y;
    }

    std::string to_string()
    {
        std::string ret = "a1";
        ret[0] += m_x;
        ret[1] += m_y;
        return ret;
    }

public: // inlines

    inline int get_x() { return m_x; }
    inline int get_y() { return m_y; }
    inline bool get_on_board() { return m_on_board; }
    inline bool get_valid() { return m_valid; }
    inline void set_on_board(bool val) { m_on_board = false; }

private:
    void register_captured(Piece* piece);

private: // properties
    int m_x, m_y;
    bool m_on_board, m_valid;
    Board* m_board;
};

#include "pieces.h"

class Square
{
public:
    Square() :
        m_piece(NULL)
    { }

    Square(Piece* piece) :
        m_piece(piece)
    { }

    inline bool is_empty()
    {
        return m_piece == NULL ? true : false;
    }

    inline Piece* get_piece()
    {
        return m_piece;
    }

    inline void remove_piece()
    {
        m_piece = NULL;
    }

    inline void set_piece(Piece* piece)
    {
        m_piece = piece;
    }

private:
    Piece* m_piece;
};

class Board
{
public:
    Board(int dim_x, int dim_y);
    ~Board()
    {
        delete_all_pieces();
    }
    void set_to_start_position();
    inline Square& square(int x, int y)
    {
        return m_squares[x * m_dim_x + y];
    }
    inline Square& square(BoardLocation loc)
    {
        return m_squares[loc.get_x() * m_dim_x + loc.get_y()];
    }
    inline bool on_board(int x, int y) {
        return ((x >= 0) && (x < m_dim_x) && (y >= 0) && (y < m_dim_y)) ? true : false;
    }

    bool add_piece(Piece::Type, Piece::Colour, std::string loc);
    bool add_piece(Piece::Type, Piece::Colour, BoardLocation loc);
    bool remove_piece(std::string loc);
    bool remove_piece(BoardLocation loc);
    bool move_piece(std::string move_str);
    bool move_piece(BoardLocation curr_loc, BoardLocation new_loc);
    int get_dim_x() { return m_dim_x; }
    int get_dim_y() { return m_dim_y; }

private:
    void delete_all_pieces();
    void register_captured(Piece* piece);

private:
    int m_dim_x, m_dim_y;
    std::vector<Square> m_squares;
    //std::vector< std::vector<Square*> > m_rows;
    //std::vector< std::vector<Square*> > m_cols;
    std::vector<Piece*> m_pieces;

};


#endif //BOARD_H
