#pragma once

#include <vector>
#include <list>
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

    static constexpr Colour opposite_colour(Colour col) { return col == WHITE ? BLACK : WHITE; }

public: //ctor/dtor
    Piece(const Colour& colour, const BoardLocation& loc, const Type& type) :
        m_colour(colour),
        m_loc(loc),
        m_type(type),
        m_moved(false)
    {
    }

    Piece(const Piece& orig) :
        m_colour(orig.m_colour),
        m_loc(orig.m_loc),
        m_type(orig.m_type),
        m_moved(orig.m_moved)
    {}


    virtual ~Piece();

public: //public interface
    virtual std::list<Move> get_all_valid_moves(const Board& board) const = 0;
    virtual bool check_valid_move(const Board& board, const Move& move) const {
        for (const auto valid_move : get_all_valid_moves(board))
        {
            if(valid_move == move)
                return true;
        }
        return false;
    }
    virtual std::vector<std::string> get_symbol_list() const = 0;

    virtual std::shared_ptr<Piece> clone() const = 0;

public: // public methods
    inline Colour get_colour() const { return m_colour; }

    inline Type get_type() const { return m_type; }

    inline BoardLocation get_loc() const { return m_loc; }

    inline std::string get_symbol() const { return get_symbol_list()[0]; }

    inline bool get_on_board() { return m_loc.get_on_board(); }
    inline void set_on_board(bool val) { m_loc.set_on_board(val); }

    inline void move(const BoardLocation& new_loc) {
        m_moved = true;
        m_just_moved = true;
        m_loc = new_loc;
    }

    inline bool has_moved() { return m_moved; }

protected:
    std::list<Move> get_all_slide_moves(const std::vector<DynMove>& dms, const Board& board, bool check_test=true) const;
    std::list<Move> get_all_hop_moves(const std::vector<DynMove>& dms, const Board& board, bool check_test=true) const;

protected:
    bool m_moved = false;
    bool m_just_moved = false;
    BoardLocation m_loc;
    Piece::Colour m_colour;
    Piece::Type m_type;
};

template<typename PieceType>
class ClonablePiece : public Piece
{
public:
    std::shared_ptr<Piece> clone() const override
    {
        return std::make_shared<PieceType>(*static_cast<const PieceType*>(this));
    }

    using Piece::Piece;
};

class King : public ClonablePiece<King>
{
public:
    King(Colour colour, BoardLocation loc);
    King(const King&) = default;
    ~King() {}
    std::list<Move> get_all_valid_moves(const Board& board) const override;
    std::vector<std::string> get_symbol_list() const override;;
private:
    std::vector<DynMove> m_dm_list;
};

class Queen : public ClonablePiece<Queen>
{
public:
    Queen(Colour colour, BoardLocation loc);
    Queen(const Queen&) = default;
    ~Queen() {}
    std::list<Move> get_all_valid_moves(const Board& board) const override;;
    std::vector<std::string> get_symbol_list() const override;;
private:
    std::vector<DynMove> m_dm_list;
};


class Rook : public ClonablePiece<Rook>
{
public:
    Rook(Colour colour, BoardLocation loc);
    Rook(const Rook&) = default;
    ~Rook() {}
    std::list<Move> get_all_valid_moves(const Board& board) const override;;
    std::vector<std::string> get_symbol_list() const override;;
private:
    std::vector<DynMove> m_dm_list;
};


class Bishop : public ClonablePiece<Bishop>
{
public:
    Bishop(Colour colour, BoardLocation loc);
    Bishop(const Bishop&) = default;
    ~Bishop() {}
    std::list<Move> get_all_valid_moves(const Board& board) const override;;
    std::vector<std::string> get_symbol_list() const override;;
private:
    std::vector<DynMove> m_dm_list;
};


class Knight : public ClonablePiece<Knight>
{
public:
    Knight(Colour colour, BoardLocation loc);
    Knight(const Knight&) = default;
    ~Knight() {}
    std::list<Move> get_all_valid_moves(const Board& board) const override;;
    std::vector<std::string> get_symbol_list() const override;;
private:
    std::vector<DynMove> m_dm_list;
};

class Pawn : public ClonablePiece<Pawn>
{
public:
    Pawn(Colour colour, BoardLocation loc);
    Pawn(const Pawn&) = default;
    ~Pawn() {}
    std::list<Move> get_all_valid_moves(const Board& board) const override;;
    std::vector<std::string> get_symbol_list() const override;;
};