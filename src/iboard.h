#pragma once
#include <functional>

#include "piece_types.h"

class IBoard
{
public:
    enum Mate {
        CHECKMATE,
        STALEMATE,
        NO_MATE
    };

    enum class CastlingRights : uint8_t {
        WHITE_KINGSIDE = 0x1,
        WHITE_QUEENSIDE = 0x2,
        BLACK_KINGSIDE = 0x4,
        BLACK_QUEENSIDE = 0x8
    };

public:
    //! Set the board to the starting position
    /*!
     * Calling this function resets all the pieces and squares on a board and sets
     * all the pieces to their starting positions
     */
    virtual void set_to_start_position() = 0;
    //! Load a board position from XBoard edit mode commands
    /*!
     * \param edit_mode_string vector of strings constituting the edit mode strings
     *                         (i.e. from 'edit' to '.')
     */
    virtual void set_from_edit_mode(std::vector<std::string> edit_mode_strings) = 0;

    //! Return whether given colour's king is in check
    /*!
     * \param col colour to test for check
     * \return true if parameter colour is in check, false otherwise
     */
    virtual bool get_in_check(PieceColour col) const = 0;

    //! Return if/what type of mate the board it in
    /*!
     * \param col colour to test for being in mate
     * \return CHECKMATE, STALEMATE or NO_MATE
     */
    virtual Mate get_mate(PieceColour col) const = 0;

    //!< pieces on the board
    virtual void for_all_pieces(std::function<void(const Piece& piece)> fn) const = 0;

    //! Return the colour whose move it is
    virtual PieceColour get_colour_to_move() const = 0;

    virtual void set_colour_to_move(PieceColour colour) = 0;

    //! Delete all the pieces from the board
    virtual void delete_all_pieces() = 0;

    //! This shouldn't really be public TODO: refactor the board_from_string_repr stuff
    virtual void populate_squares_properties() = 0;

    //! Return the number of available moves for a given colour
    virtual std::list<Move> get_all_legal_moves(PieceColour col) const = 0;

    virtual bool add_piece(PieceType type, PieceColour col, BoardLocation loc) = 0;

    virtual bool has_castling_rights(CastlingRights castling_rights) const = 0;
    virtual std::optional<uint8_t> get_enpassant_column() const = 0;

    virtual bool make_move(const Move& move) = 0;
    virtual bool unmake_move(const Move& move) = 0;

    virtual std::unique_ptr<IBoard> clone() const = 0;
    virtual std::unique_ptr<IBoard> clone_moved(const Move& move) const = 0;

    virtual std::shared_ptr<const Piece> piece_on_square(int x, int y) const = 0;
};