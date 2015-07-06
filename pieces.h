#include "board.h"

#ifndef PIECES_H
#define PIECES_H

#include <vector>
#include <string>


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
    inline int get_dx() { return m_dx; }
    inline int get_dy() { return m_dy; }

private:
    int m_dx, m_dy;
};


class Move
{
// Move: Describes a move, which is the new location of a given piece
public:
    Move(Piece *piece, DynMove dm);
    Move(Piece *piece, BoardLocation new_loc) :
        m_piece(piece),
        m_new_loc(new_loc)
    {
    }

    bool operator== (Move& m)
    {
        return m_piece == m.m_piece && m_new_loc == m.m_new_loc;
    }

private:
    BoardLocation m_new_loc;
    Piece *m_piece;
};

class Piece
{
// Piece: Describes a chess piece, including its colour and location
// Subclass this class to implement individual pieces with their different rules
public:
    enum Type {
        KING,
        QUEEN,
        ROOK,
        BISHOP,
        KNIGHT,
        PAWN
    };

    enum Colour {
        WHITE,
        BLACK
    };

public: //ctor/dtor
    Piece(Colour colour, BoardLocation loc) :
        m_colour(colour),
        m_loc(loc)
    {
    }
    virtual ~Piece();

public: //public interface
    virtual std::vector<Move> get_all_valid_moves(Board& board) = 0;
    virtual bool check_valid_move(Board& board, Move move) {
        std::vector<Move> valid_moves = get_all_valid_moves(board);
        for (std::vector<Move>::iterator it = valid_moves.begin(); it != valid_moves.end(); ++it)
        {
            if(*it == move)
                return true;
        }
        return false;
    }
    virtual std::vector<std::string> get_symbol_list() = 0;

public: // public methods
    inline Colour get_colour() { return m_colour; }
    inline BoardLocation get_loc() { return m_loc; }
    inline std::string get_symbol() { return get_symbol_list()[0]; }
    inline void set_on_board(bool val) { m_loc.set_on_board(val); }
    inline bool move(Board& board, BoardLocation new_loc)
    {
        if(!check_valid_move(board, Move(this, new_loc)))
            return false;
        m_loc = new_loc;
    }

protected:
    std::vector<Move> get_all_slide_moves(std::vector<DynMove> dms, Board& board);
    std::vector<Move> get_all_hop_moves(std::vector<DynMove> dms, Board& board);

protected:
    BoardLocation m_loc;
    Piece::Colour m_colour;
};

class King : public Piece
{
public:
    King(Colour colour, BoardLocation loc);
    ~King() {}
    std::vector<Move> get_all_valid_moves(Board& board);
    std::vector<std::string> get_symbol_list();
private:
    std::vector<DynMove> m_dm_list;
};

class Queen : public Piece
{
public:
    Queen(Colour colour, BoardLocation loc);
    ~Queen() {}
    std::vector<Move> get_all_valid_moves(Board& board);
    std::vector<std::string> get_symbol_list();
private:
    std::vector<DynMove> m_dm_list;
};


class Rook : public Piece
{
public:
    Rook(Colour colour, BoardLocation loc);
    ~Rook() {}
    std::vector<Move> get_all_valid_moves(Board& board);
    std::vector<std::string> get_symbol_list();
private:
    std::vector<DynMove> m_dm_list;
};


class Bishop : public Piece
{
public:
    Bishop(Colour colour, BoardLocation loc);
    ~Bishop() {}
    std::vector<Move> get_all_valid_moves(Board& board);
    std::vector<std::string> get_symbol_list();
private:
    std::vector<DynMove> m_dm_list;
};


class Knight : public Piece
{
public:
    Knight(Colour colour, BoardLocation loc);
    ~Knight() {}
    std::vector<Move> get_all_valid_moves(Board& board);
    std::vector<std::string> get_symbol_list();
private:
    std::vector<DynMove> m_dm_list;
};

class Pawn : public Piece
{
public:
    Pawn(Colour colour, BoardLocation loc);
    ~Pawn() {}
    std::vector<Move> get_all_valid_moves(Board& board);
    std::vector<std::string> get_symbol_list();
};


#endif
