#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "board_location.h"

#include "move.h"

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
    Piece(const Colour& colour, const BoardLocation& loc, const Type& type) :
        m_colour(colour),
        m_loc(loc),
        m_type(type)
    {
    }
    virtual ~Piece();

public: //public interface
    virtual std::vector<Move> get_all_valid_moves(const Board& board) const = 0;
    virtual bool check_valid_move(const Board& board, const Move& move) const {
        for (const auto valid_move : get_all_valid_moves(board))
        {
            if(valid_move == move)
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

    inline void move(const BoardLocation& new_loc) { m_loc = new_loc; }

protected:
    std::vector<Move> get_all_slide_moves(const std::vector<DynMove>& dms, const Board& board, bool check_test=true) const;
    std::vector<Move> get_all_hop_moves(const std::vector<DynMove>& dms, const Board& board, bool check_test=true) const;

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
    std::vector<Move> get_all_valid_moves(const Board& board) const override;
    std::vector<std::string> get_symbol_list() const override;;
private:
    std::vector<DynMove> m_dm_list;
};

class Queen : public Piece
{
public:
    Queen(Colour colour, BoardLocation loc);
    ~Queen() {}
    std::vector<Move> get_all_valid_moves(const Board& board) const override;;
    std::vector<std::string> get_symbol_list() const override;;
private:
    std::vector<DynMove> m_dm_list;
};


class Rook : public Piece
{
public:
    Rook(Colour colour, BoardLocation loc);
    ~Rook() {}
    std::vector<Move> get_all_valid_moves(const Board& board) const override;;
    std::vector<std::string> get_symbol_list() const override;;
private:
    std::vector<DynMove> m_dm_list;
};


class Bishop : public Piece
{
public:
    Bishop(Colour colour, BoardLocation loc);
    ~Bishop() {}
    std::vector<Move> get_all_valid_moves(const Board& board) const override;;
    std::vector<std::string> get_symbol_list() const override;;
private:
    std::vector<DynMove> m_dm_list;
};


class Knight : public Piece
{
public:
    Knight(Colour colour, BoardLocation loc);
    ~Knight() {}
    std::vector<Move> get_all_valid_moves(const Board& board) const override;;
    std::vector<std::string> get_symbol_list() const override;;
private:
    std::vector<DynMove> m_dm_list;
};

class Pawn : public Piece
{
public:
    Pawn(Colour colour, BoardLocation loc);
    ~Pawn() {}
    std::vector<Move> get_all_valid_moves(const Board& board) const override;;
    std::vector<std::string> get_symbol_list() const override;;
};