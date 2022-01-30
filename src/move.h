#pragma once

#include <memory>
#include <optional>

#include "board_location.h"
#include "piece_types.h"

class IBoard;
class Piece;

class DynMove
{
// DynMove: Describes the dynamics of a move (where a piece moves to 
// relative to it's current position)
public:
    constexpr DynMove(int dx, int dy) :
        m_dx(dx), m_dy(dy)
    {
    }
    inline int get_dx() const { return m_dx; }
    inline int get_dy() const { return m_dy; }

private:
    int m_dx, m_dy;
};

class MoveHashFunction;

class Move
{
private:
    static constexpr uint32_t CAPTURED_PIECE_TYPE = 0x700;
    static constexpr uint32_t CAPTURED_PIECE_OFFSET = 8;

    static constexpr uint32_t OLD_CASTLING_RIGHTS_MASK = 0xf0;
    static constexpr uint32_t OLD_CASTLING_RIGHTS_OFFSET = 4;

    static constexpr uint32_t PROMOTION_MASK = 0x8;
    static constexpr uint32_t EN_PASSANT_CAPTURE_MASK = 0x4;
    static constexpr uint32_t SPECIAL_MASK = 0x3;

// Move: Describes a move, which is the new location of a given piece
public:
    enum class SimpleMoveType : uint8_t {
        QUIET,
        DOUBLE_PAWN_PUSH,
        KING_SIDE_CASTLE,
        QUEEN_SIDE_CASTLE
    };

    enum class PromotionType : uint8_t {
        QUEEN,
        ROOK,
        BISHOP,
        KNIGHT
    };

    Move(const BoardLocation& from_loc, const DynMove& dm);
    Move(const std::string& move_str);
    Move(const BoardLocation& from_loc, const BoardLocation& to_loc);

    bool operator== (const Move& m) const;
    
    std::string to_string() const;

    const BoardLocation& get_to_loc() const;
    const BoardLocation get_from_loc() const;

    void set_is_en_passant_capture() {
        m_type |= EN_PASSANT_CAPTURE_MASK;
    }

    bool is_en_passant_capture() const {
        return m_type & EN_PASSANT_CAPTURE_MASK;
    }

    void set_promotion_type(PromotionType promotion_type) { 
        m_promotion_type = promotion_type;
    }

    const std::optional<PromotionType> get_promotion_type() const { 
        return m_promotion_type;
    }

    // TODO: deprecate following two 
    void set_captured_piece(std::shared_ptr<Piece> piece) {
        m_captured_piece = piece;
    }

    const std::shared_ptr<Piece> get_captured_piece() const {
        return m_captured_piece;
    }

    void set_captured_piece_type(PieceType piece_type) {
        m_captured_piece_type = piece_type;
    }

    const std::optional<PieceType> get_captured_piece_type() const {
        return m_captured_piece_type;
    }

    void set_old_castling_rights(uint8_t castling_rights) {
        m_old_castling_rights = castling_rights;
    }

    uint8_t get_old_castling_rights() const{
        return m_old_castling_rights;
    }

private:
    BoardLocation m_from_loc;
    BoardLocation m_to_loc;
    uint8_t m_old_castling_rights = 0;
    std::shared_ptr<Piece> m_captured_piece;
    std::optional<PieceType> m_captured_piece_type = std::nullopt;
    std::optional<PromotionType> m_promotion_type = std::nullopt;

    uint8_t m_type = 0;
};
