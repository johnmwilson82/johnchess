#pragma once

#include <array>
#include <vector>
#include <string>
#include <iostream>

class DynMove;

#include <boost/container/static_vector.hpp>


#include "board_location.h"
#include "square.h"
#include "pieces.h"

#include "iboard.h"

/*! /brief Represents a chess board
 *
 * This class represents the chess board itself and is used to hold
 * instantaneous position data for a game
 */
class Board : public IBoard
{
private:
    static constexpr uint8_t INIT_CASTLING_RIGHTS =
        static_cast<uint8_t>(CastlingRights::WHITE_KINGSIDE) |
        static_cast<uint8_t>(CastlingRights::BLACK_KINGSIDE) |
        static_cast<uint8_t>(CastlingRights::WHITE_QUEENSIDE) |
        static_cast<uint8_t>(CastlingRights::BLACK_QUEENSIDE);

    std::array<Square, BOARD_DIM*BOARD_DIM> m_squares; //!< board squares
    boost::container::static_vector<std::shared_ptr<Piece>, 40> m_pieces;  //!< pieces on the board
    PieceColour m_colour_to_move;
    uint8_t m_castling_rights = 0;
    std::optional<uint8_t> m_en_passant_column = std::nullopt;

public:
    Board();
    ~Board()
    {
        delete_all_pieces();
    }
    Board(const Board& board);
    Board(const Board& board, const Move& move);
    Board(const Board& board, const std::string& move_str);


    //! Return a ref to the square at the given x, y coordinates (0-7)
    inline const Square& square(int x, int y) const
    {
        return m_squares[x * BOARD_DIM + y];
    }

    //! Return a const ref to the square at the given BoardLocation
    inline const Square& square(BoardLocation loc) const
    {
        return m_squares[loc.get_x() * BOARD_DIM + loc.get_y()];
    }

    //! Return a const ref to the square at the given x, y coordinates (0-7)
    inline Square& square(int x, int y)
    {
        return m_squares[x * BOARD_DIM + y];
    }

    //! Return a ref to the square at the given BoardLocation
    inline Square& square(BoardLocation loc)
    {
        return m_squares[loc.get_x() * BOARD_DIM + loc.get_y()];
    }

    //! Return true if the given x, y coordinates are on the board, false otherwise
    inline bool on_board(int x, int y) const {
        return ((x >= 0) && (x < BOARD_DIM) && (y >= 0) && (y < BOARD_DIM)) ? true : false;
    }

    template<typename T>
    bool add_piece(PieceColour col, BoardLocation loc);

    template<typename T>
    bool add_piece(PieceColour col, std::string loc);

public: // IBoard

    //! Set the board to the starting position
    /*!
     * Calling this function resets all the pieces and squares on a board and sets
     * all the pieces to their starting positions
     */
    void set_to_start_position() override;
    //! Load a board position from XBoard edit mode commands
    /*!
     * \param edit_mode_string vector of strings constituting the edit mode strings
     *                         (i.e. from 'edit' to '.')
     */
    void set_from_edit_mode(std::vector<std::string> edit_mode_strings) override;

    //! Return whether given colour's king is in check
    /*!
     * \param col colour to test for check
     * \return true if parameter colour is in check, false otherwise
     */
    bool get_in_check(PieceColour col) const override;

    //! Return if/what type of mate the board it in
    /*!
     * \param col colour to test for being in mate
     * \return CHECKMATE, STALEMATE or NO_MATE
     */
    Mate get_mate(PieceColour col) const override
    {
        if(get_all_legal_moves(col).size() == 0)
        {
            if (get_in_check(col))
                return CHECKMATE;
            else
                return STALEMATE;
        }
        return NO_MATE;
    }

    //!< pieces on the board
    void for_all_pieces(std::function<void(const Piece& piece)> fn) const override
    {
        for(const auto& ppc : m_pieces)
        {
            if (ppc->get_on_board())
            {
                fn(*ppc);
            }
        }
    } 

    //! Return the colour whose move it is
    PieceColour get_colour_to_move() const override
    {
        return m_colour_to_move;
    }

    void set_colour_to_move(PieceColour colour) override
    {
        m_colour_to_move = colour;
    }

    //! Delete all the pieces from the board
    void delete_all_pieces() override;

    //! This shouldn't really be public TODO: refactor the board_from_string_repr stuff
    void populate_squares_properties() override;

    //! Return the number of available moves for a given colour
    std::list<Move> get_all_legal_moves(PieceColour col) const override;

    bool has_castling_rights(CastlingRights castling_rights) const override { 
        return m_castling_rights & static_cast<uint8_t>(castling_rights); 
    };

    void set_castling_rights(const std::vector<CastlingRights>& castling_rights) override
    {
        for (const auto cr : castling_rights)
        {
            m_castling_rights |= static_cast<uint8_t>(cr);
        }
    }

    std::optional<uint8_t> get_enpassant_column() const override { return m_en_passant_column; };
    void set_enpassant_column(std::optional<uint8_t> col) override { m_en_passant_column = col; }

    bool add_piece(PieceType type, PieceColour col, BoardLocation loc) override;

    bool make_move(const Move& move) override;
    bool unmake_move(const Move& move) override;

    std::unique_ptr<IBoard> clone() const override;

    std::shared_ptr<const Piece> piece_on_square(int x, int y) const override;

    std::unique_ptr<IBoard> clone_moved(const Move& move) const override;

private:
    bool add_piece(const Piece& piece);

    //! Remove a piece from the board where location is given as a string in standard notation
    /*!
     * \return true on success and if there was a piece to remove
     */
    bool remove_piece(std::string loc);

    void process_castling_rook_move(std::optional<std::pair<std::shared_ptr<Piece>, BoardLocation>> castling_rook_move);
    std::optional<std::pair<std::shared_ptr<Piece>, BoardLocation>> check_castling_rook_move(const Piece& moving_piece, const BoardLocation& new_loc);
    std::optional<std::pair<std::shared_ptr<Piece>, BoardLocation>> check_unmake_castling_rook_move(const Piece& moving_piece, const BoardLocation& prev_loc);

    //! Remove a piece from the board where location is given as a BoardLocation
    /*!
     * \return true on success and if there was a piece to remove
     */
    bool remove_piece(BoardLocation loc);

    bool add_piece(PieceType type, PieceColour col, std::string loc);

    uint8_t get_castling_rights_to_remove(const Piece& moving_piece) const;
};