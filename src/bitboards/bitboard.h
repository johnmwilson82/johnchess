#pragma once

#include <functional>

#include <move.h>
#include <piece_types.h>

#include "iboard.h"

class BitBoard : public IBoard
{
private:
    uint64_t m_pawns, m_knights, m_bishops, m_rooks, m_queens, m_kings;
    uint64_t m_black_pieces, m_occupied;

    uint8_t m_white_to_move; // 1 or 0

    static constexpr uint64_t knight_attack_lut[64] =
    {
        0x0000000000020400, 0x0000000000050800, 0x00000000000a1100, 0x0000000000142200, 0x0000000000284400, 0x0000000000508800, 0x0000000000a01000, 0x0000000000402000,
        0x0000000002040004, 0x0000000005080008, 0x000000000a110011, 0x0000000014220022, 0x0000000028440044, 0x0000000050880088, 0x00000000a0100010, 0x0000000040200020,
        0x0000000204000402, 0x0000000508000805, 0x0000000a1100110a, 0x0000001422002214, 0x0000002844004428, 0x0000005088008850, 0x000000a0100010a0, 0x0000004020002040,
        0x0000020400040200, 0x0000050800080500, 0x00000a1100110a00, 0x0000142200221400, 0x0000284400442800, 0x0000508800885000, 0x0000a0100010a000, 0x0000402000204000,
        0x0002040004020000, 0x0005080008050000, 0x000a1100110a0000, 0x0014220022140000, 0x0028440044280000, 0x0050880088500000, 0x00a0100010a00000, 0x0040200020400000,
        0x0204000402000000, 0x0508000805000000, 0x0a1100110a000000, 0x1422002214000000, 0x2844004428000000, 0x5088008850000000, 0xa0100010a0000000, 0x4020002040000000,
        0x0400040200000000, 0x0800080500000000, 0x1100110a00000000, 0x2200221400000000, 0x4400442800000000, 0x8800885000000000, 0x100010a000000000, 0x2000204000000000,
        0x0004020000000000, 0x0008050000000000, 0x00110a0000000000, 0x0022140000000000, 0x0044280000000000, 0x0088500000000000, 0x0010a00000000000, 0x0020400000000000,
    };

    void get_knight_moves(std::list<Move>& move_list) const;

    inline uint64_t pieces_to_move() const;

public:
    BitBoard();

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

    bool has_castling_rights(CastlingRights castling_rights) const override;
    void set_castling_rights(const std::vector<CastlingRights>& castling_rights) override;

    std::optional<uint8_t> get_enpassant_column() const;
    void set_enpassant_column(std::optional<uint8_t> col) override;

    bool make_move(const Move& move);
    bool unmake_move(const Move& move);

    std::unique_ptr<IBoard> clone() const;
    std::unique_ptr<IBoard> clone_moved(const Move& move) const;

    //!< pieces on the board
    void for_all_pieces(std::function<void(const Piece& piece)> fn) const;

    std::shared_ptr<const Piece> piece_on_square(int x, int y) const;
};