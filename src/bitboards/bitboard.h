#pragma once

#include <functional>

#include <move.h>
#include <piece_types.h>

#include "iboard.h"

class BitBoard : public IBoard
{
private:
    std::unordered_map<uint8_t, uint64_t> m_dummy_map;

    uint64_t m_pawns, m_knights, m_bishops, m_rooks, m_queens, m_kings;
    uint64_t m_black_pieces, m_white_pieces, m_occupied;

    mutable uint64_t m_opposite_attacks, m_current_attacks, m_allowed_moves, m_new_allowed_moves;
    mutable MoveList m_move_list;

    uint8_t m_white_to_move; // 1 or 0
    uint8_t m_castling_rights = 0;

    std::optional<uint8_t> m_en_passant_col;

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

    static constexpr uint64_t king_attack_lut[64] =
    {
        0x0000000000000302, 0x0000000000000705, 0x0000000000000e0a, 0x0000000000001c14, 0x0000000000003828, 0x0000000000007050, 0x000000000000e0a0, 0x000000000000c040,
        0x0000000000030203, 0x0000000000070507, 0x00000000000e0a0e, 0x00000000001c141c, 0x0000000000382838, 0x0000000000705070, 0x0000000000e0a0e0, 0x0000000000c040c0,
        0x0000000003020300, 0x0000000007050700, 0x000000000e0a0e00, 0x000000001c141c00, 0x0000000038283800, 0x0000000070507000, 0x00000000e0a0e000, 0x00000000c040c000,
        0x0000000302030000, 0x0000000705070000, 0x0000000e0a0e0000, 0x0000001c141c0000, 0x0000003828380000, 0x0000007050700000, 0x000000e0a0e00000, 0x000000c040c00000,
        0x0000030203000000, 0x0000070507000000, 0x00000e0a0e000000, 0x00001c141c000000, 0x0000382838000000, 0x0000705070000000, 0x0000e0a0e0000000, 0x0000c040c0000000,
        0x0003020300000000, 0x0007050700000000, 0x000e0a0e00000000, 0x001c141c00000000, 0x0038283800000000, 0x0070507000000000, 0x00e0a0e000000000, 0x00c040c000000000,
        0x0302030000000000, 0x0705070000000000, 0x0e0a0e0000000000, 0x1c141c0000000000, 0x3828380000000000, 0x7050700000000000, 0xe0a0e00000000000, 0xc040c00000000000,
        0x0203000000000000, 0x0507000000000000, 0x0a0e000000000000, 0x141c000000000000, 0x2838000000000000, 0x5070000000000000, 0xa0e0000000000000, 0x40c0000000000000, 
    };

    uint64_t get_pawn_moves(MoveList& move_list, bool white_to_move, const std::unordered_map<uint8_t, uint64_t>& pinned_piece_allowed_moves) const;
    uint64_t get_en_passant_pawn_moves(MoveList& move_list, bool white_to_move, const std::unordered_map<uint8_t, uint64_t>& pinned_piece_allowed_moves) const;
    uint64_t get_knight_moves(MoveList& move_list, bool white_to_move, uint64_t pinned) const;
    uint64_t get_king_moves(MoveList& move_list, bool white_to_move) const;

    void get_castling_moves(MoveList& move_list, bool white_to_move) const;

public:
    inline uint64_t get_occupied() const { return m_occupied; }
    inline uint64_t get_rooks() const { return m_rooks; }
    inline uint64_t get_bishops() const { return m_bishops; }
    inline uint64_t get_knights() const { return m_knights; }
    inline uint64_t get_pawns() const { return m_pawns; }
    inline uint64_t get_queens() const { return m_queens; }
    inline uint64_t get_kings() const { return m_kings; }

    uint64_t get_moves(MoveList& move_list, uint64_t pieces, bool white_to_move, std::function<uint64_t(uint8_t)> attacks_fn) const;
    uint64_t pieces_to_move(bool white_to_move) const;

    Move& emplace_move(MoveList& move_list, const BoardLocation& from_loc, const BoardLocation& to_loc) const;

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
    MoveList& get_all_legal_moves(PieceColour col) const;

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