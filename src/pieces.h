#include "board.h"

#ifndef PIECES_H
#define PIECES_H

#include <vector>
#include <string>
#include <iostream>

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


class Move
{
// Move: Describes a move, which is the new location of a given piece
public:
    Move(const Piece *piece, DynMove dm);
    Move(const Piece *piece, BoardLocation new_loc) :
        m_piece(piece),
        m_new_loc(new_loc)
    {
    }

    bool operator== (Move& m) const
    {
        //std::cout << (size_t)m_piece << " = " << (size_t)m.m_piece << " : " << m_new_loc.get_x() << " = " << m.m_new_loc.get_x() << " : " << m_new_loc.get_y() << " = " << m.m_new_loc.get_y() << ((m_new_loc == m.m_new_loc) ? " true" : " false") << std::endl;
        return (m_piece == m.m_piece) && (m_new_loc == m.m_new_loc);
    }

    BoardLocation get_board_loc() const { return m_new_loc; }
    std::string to_string() const;
private:
    BoardLocation m_new_loc;
    const Piece *m_piece;
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
    Piece(Colour colour, BoardLocation loc, Type type) :
        m_colour(colour),
        m_loc(loc),
        m_type(type)
    {
    }
    virtual ~Piece();

public: //public interface
    virtual std::vector<Move> get_all_valid_moves(const Board& board, bool check_test=true) const = 0;
    virtual bool check_valid_move(const Board& board, Move move, bool check_test=true) {
        std::vector<Move> valid_moves = get_all_valid_moves(board, check_test);
        for (std::vector<Move>::iterator it = valid_moves.begin(); it != valid_moves.end(); it++)
        {
            if(*it == move)
                return true;
        }
        return false;
    }
    virtual std::vector<std::string> get_symbol_list() const = 0;

public: // public methods
    static inline Colour opposite_colour(Colour col) { return col == Piece::WHITE ? Piece::BLACK : Piece::WHITE; }
    inline Colour get_colour() const { return m_colour; }
    inline Type get_type() const { return m_type; }
    inline BoardLocation get_loc() const { return m_loc; }
    inline std::string get_symbol() const { return get_symbol_list()[0]; }
    inline void set_on_board(bool val) { m_loc.set_on_board(val); }
    inline bool move(Board& board, BoardLocation new_loc, bool get_valid_move_check_test=true)
    {
        if(!check_valid_move(board, Move(this, new_loc), get_valid_move_check_test))
            return false;
        m_loc = new_loc;
        return true;
    }

protected:
    std::vector<Move> get_all_slide_moves(const std::vector<DynMove>& dms, const Board& board, bool check_test=true) const;
    std::vector<Move> get_all_hop_moves(const std::vector<DynMove>& dms, const Board& board, bool check_test=true) const;

    bool push_move_with_check_test(bool check_test, const Board &board, BoardLocation &new_loc, std::vector<Move> &move_list) const;

protected:
    BoardLocation m_loc;
    Piece::Colour m_colour;
    Piece::Type m_type;
};

class King : public Piece
{
public:
    King(Colour colour, BoardLocation loc);
    ~King() {}
    std::vector<Move> get_all_valid_moves(const Board& board, bool check_test) const;
    std::vector<std::string> get_symbol_list() const;
private:
    std::vector<DynMove> m_dm_list;
};

class Queen : public Piece
{
public:
    Queen(Colour colour, BoardLocation loc);
    ~Queen() {}
    std::vector<Move> get_all_valid_moves(const Board& board, bool check_test) const;
    std::vector<std::string> get_symbol_list() const;
private:
    std::vector<DynMove> m_dm_list;
};


class Rook : public Piece
{
public:
    Rook(Colour colour, BoardLocation loc);
    ~Rook() {}
    std::vector<Move> get_all_valid_moves(const Board& board, bool check_test) const;
    std::vector<std::string> get_symbol_list() const;
private:
    std::vector<DynMove> m_dm_list;
};


class Bishop : public Piece
{
public:
    Bishop(Colour colour, BoardLocation loc);
    ~Bishop() {}
    std::vector<Move> get_all_valid_moves(const Board& board, bool check_test) const;
    std::vector<std::string> get_symbol_list() const;
private:
    std::vector<DynMove> m_dm_list;
};


class Knight : public Piece
{
public:
    Knight(Colour colour, BoardLocation loc);
    ~Knight() {}
    std::vector<Move> get_all_valid_moves(const Board& board, bool check_test) const;
    std::vector<std::string> get_symbol_list() const;
private:
    std::vector<DynMove> m_dm_list;
};

class Pawn : public Piece
{
public:
    Pawn(Colour colour, BoardLocation loc);
    ~Pawn() {}
    std::vector<Move> get_all_valid_moves(const Board& board, bool check_test) const;
    std::vector<std::string> get_symbol_list() const;
};


#endif
