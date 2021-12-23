#pragma once

#include <vector>
#include <list>
#include <string>
#include <iostream>
#include <array>
#include <span>
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
    Piece(const Board& board, const Colour& colour, const BoardLocation& loc, const Type& type) :
        m_board(board),
        m_colour(colour),
        m_loc(loc),
        m_type(type)
    {
    }

    Piece(const Board& board, const Piece& orig) :
        m_board(board),
        m_colour(orig.m_colour),
        m_loc(orig.m_loc),
        m_type(orig.m_type)
    {}


    virtual ~Piece();

public: //public interface
    virtual std::list<Move> get_all_valid_moves() const = 0;
    virtual bool check_valid_move(const Move& move) const {
        for (const auto valid_move : get_all_valid_moves())
        {
            if(valid_move == move)
                return true;
        }
        return false;
    }
    virtual std::vector<std::string> get_symbol_list() const = 0;

    virtual std::shared_ptr<Piece> clone(const Board& board) const = 0;

public: // public methods
    inline Colour get_colour() const { return m_colour; }

    inline Type get_type() const { return m_type; }

    inline BoardLocation get_loc() const { return m_loc; }

    inline std::string get_symbol() const { return get_symbol_list()[0]; }

    inline bool get_on_board() { return m_loc.get_on_board(); }
    inline void set_on_board(bool val) { m_loc.set_on_board(val); }

    inline void move(const BoardLocation& new_loc) {
        m_loc = new_loc;
    }

protected:
    template<std::size_t SIZE>
    std::list<Move> get_all_slide_moves(const std::array<DynMove, SIZE>& dms, bool check_test=true) const;

    template<std::size_t SIZE>
    std::list<Move> get_all_hop_moves(const std::array<DynMove, SIZE>& dms, bool check_test=true) const;

protected:
    BoardLocation m_loc;
    const Board& m_board;
    Piece::Colour m_colour;
    Piece::Type m_type;
};

template<typename PieceType>
class ClonablePiece : public Piece
{
public:
    std::shared_ptr<Piece> clone(const Board& board) const override
    {
        return std::make_shared<PieceType>(board, *static_cast<const PieceType*>(this));
    }

    using Piece::Piece;
};

class King : public ClonablePiece<King>
{
public:
    King(const Board& board, Colour colour, BoardLocation loc);
    King(const Board& board, const King& orig) : ClonablePiece(board, orig) {}
    King(const King&) = delete;
    ~King() {}
    std::list<Move> get_all_valid_moves() const override;
    std::vector<std::string> get_symbol_list() const override;;
private:
    static constexpr std::array<DynMove, 8> m_dm_list = {
        DynMove(-1, -1),
        DynMove(-1, 0),
        DynMove(-1, 1),
        DynMove(0, 1),
        DynMove(1, 1),
        DynMove(1, 0),
        DynMove(1, -1),
        DynMove(0, -1) 
    };
};

class Queen : public ClonablePiece<Queen>
{
public:
    Queen(const Board& board, Colour colour, BoardLocation loc);
    Queen(const Board& board, const Queen& orig) : ClonablePiece(board, orig) {}
    Queen(const Queen&) = delete;
    ~Queen() {}
    std::list<Move> get_all_valid_moves() const override;;
    std::vector<std::string> get_symbol_list() const override;;
private:
    static constexpr std::array<DynMove, 8> m_dm_list = {
        DynMove(-1, -1),
        DynMove(-1, 0),
        DynMove(-1, 1),
        DynMove(0, 1),
        DynMove(1, 1),
        DynMove(1, 0),
        DynMove(1, -1),
        DynMove(0, -1)
    };
};


class Rook : public ClonablePiece<Rook>
{
public:
    Rook(const Board& board, Colour colour, BoardLocation loc);
    Rook(const Board& board, const Rook& orig) : ClonablePiece(board, orig) {}
    Rook(const Rook&) = delete;
    ~Rook() {}
    std::list<Move> get_all_valid_moves() const override;;
    std::vector<std::string> get_symbol_list() const override;;
private:
    static constexpr std::array<DynMove, 4> m_dm_list = {
        DynMove(-1, 0),
        DynMove(0, 1),
        DynMove(1, 0),
        DynMove(0, -1)
    };
};


class Bishop : public ClonablePiece<Bishop>
{
public:
    Bishop(const Board& board, Colour colour, BoardLocation loc);
    Bishop(const Board& board, const Bishop& orig) : ClonablePiece(board, orig) {}
    Bishop(const Bishop&) = delete;
    ~Bishop() {}
    std::list<Move> get_all_valid_moves() const override;;
    std::vector<std::string> get_symbol_list() const override;;
private:
    static constexpr std::array<DynMove, 4> m_dm_list = {
        DynMove(-1, -1),
        DynMove(-1, 1),
        DynMove(1, 1),
        DynMove(1, -1),
    };
};


class Knight : public ClonablePiece<Knight>
{
public:
    Knight(const Board& board, Colour colour, BoardLocation loc);
    Knight(const Board& board, const Knight& orig) : ClonablePiece(board, orig) {}
    Knight(const Knight&) = delete;
    ~Knight() {}
    std::list<Move> get_all_valid_moves() const override;;
    std::vector<std::string> get_symbol_list() const override;;
private:
    static constexpr std::array<DynMove, 8> m_dm_list = {
        DynMove(-1, 2),
        DynMove(-1, -2),
        DynMove(-2, 1),
        DynMove(-2, -1),
        DynMove(1, 2),
        DynMove(1, -2),
        DynMove(2, 1),
        DynMove(2, -1)
    };
};

class Pawn : public ClonablePiece<Pawn>
{
public:
    Pawn(const Board& board, Colour colour, BoardLocation loc);
    Pawn(const Board& board, const Pawn& orig) : ClonablePiece(board, orig) {}
    Pawn(const Pawn&) = delete;
    ~Pawn() {}
    std::list<Move> get_all_valid_moves() const override;;
    std::vector<std::string> get_symbol_list() const override;;
};