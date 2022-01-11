#pragma once

#include <functional>

#include <move.h>
#include <piece_types.h>

#include "iboard.h"

class BitBoard : public IBoard
{

public:
    //! Set the board to the starting position
    /*!
     * Calling this function resets all the pieces and squares on a board and sets
     * all the pieces to their starting positions
     */
    void set_to_start_position();
    //! Load a board position from XBoard edit mode commands
    /*!
     * \param edit_mode_string vector of strings constituting the edit mode strings
     *                         (i.e. from 'edit' to '.')
     */
    void set_from_edit_mode(std::vector<std::string> edit_mode_strings);

    //! Return whether given colour's king is in check
    /*!
     * \param col colour to test for check
     * \return true if parameter colour is in check, false otherwise
     */
    bool get_in_check(PieceColour col) const;

    //! Return if/what type of mate the board it in
    /*!
     * \param col colour to test for being in mate
     * \return CHECKMATE, STALEMATE or NO_MATE
     */
    Mate get_mate(PieceColour col) const;

    //! Return the colour whose move it is
    PieceColour get_colour_to_move() const;

    void set_colour_to_move(PieceColour colour);

    //! Delete all the pieces from the board
    void delete_all_pieces();

    //! This shouldn't really be public TODO: refactor the board_from_string_repr stuff
    void populate_squares_properties();

    //! Return the number of available moves for a given colour
    std::list<Move> get_all_legal_moves(PieceColour col) const;

    bool add_piece(PieceType type, PieceColour col, BoardLocation loc);

    bool has_castling_rights(CastlingRights castling_rights) const;

    std::optional<uint8_t> get_enpassant_column() const;

    bool make_move(const Move& move);
    bool unmake_move(const Move& move);

    std::unique_ptr<IBoard> clone() const;
    std::unique_ptr<IBoard> clone_moved(const Move& move) const;

    //!< pieces on the board
    void for_all_pieces(std::function<void(const Piece& piece)> fn) const;

    std::shared_ptr<const Piece> piece_on_square(int x, int y) const;
};