#pragma once

#include <memory>
#include <optional>

#include "board_location.h"
#include "piece_types.h"

class BitBoard;
class Piece;

class MoveHashFunction;

class Move
{
private:
    /*
     * m_data bits:
     *      0 - 5    from_loc
     *      6 - 11   to_loc
     *      12 - 15  old_castling_rights
     *      16 - 18  captured_piece_type
     *      19 - 21  promotion_type
     *      22       is_en_passant_capture
     */

    static constexpr uint32_t FROM_LOC_SHIFT = 0;
    static constexpr uint32_t FROM_LOC_MASK = 0x3F << FROM_LOC_SHIFT;

    static constexpr uint32_t TO_LOC_SHIFT = 6;
    static constexpr uint32_t TO_LOC_MASK = 0x3F << TO_LOC_SHIFT;

    static constexpr uint32_t OLD_CASTLING_RIGHTS_SHIFT = 12;
    static constexpr uint32_t OLD_CASTLING_RIGHTS_MASK = 0xF << OLD_CASTLING_RIGHTS_SHIFT;

    static constexpr uint32_t CAPTURED_PIECE_TYPE_SHIFT = 16;
    static constexpr uint32_t CAPTURED_PIECE_TYPE_MASK = 0x7 << CAPTURED_PIECE_TYPE_SHIFT;

    static constexpr uint32_t PROMOTION_TYPE_SHIFT = 19;
    static constexpr uint32_t PROMOTION_TYPE_MASK = 0x7 << PROMOTION_TYPE_SHIFT;

    static constexpr uint32_t IS_EN_PASSANT_CAPTURE_SHIFT = 22;
    static constexpr uint32_t IS_EN_PASSANT_CAPTURE_MASK = 0x1 << IS_EN_PASSANT_CAPTURE_SHIFT;

    static constexpr uint32_t INIT_DATA = PROMOTION_TYPE_MASK | CAPTURED_PIECE_TYPE_MASK;

// Move: Describes a move, which is the new location of a given piece
public:
    enum class PromotionType : uint8_t {
        QUEEN,
        ROOK,
        BISHOP,
        KNIGHT
    };

    //Move(const BoardLocation& from_loc, const DynMove& dm);
    Move(const std::string& move_str);
    Move(const BoardLocation& from_loc, const BoardLocation& to_loc);

    bool operator== (const Move& m) const;
    
    std::string to_string() const;

    const BoardLocation get_to_loc() const
    {
        return (m_data & TO_LOC_MASK) >> TO_LOC_SHIFT;
    }

    const BoardLocation get_from_loc() const
    {
        return (m_data & FROM_LOC_MASK) >> FROM_LOC_SHIFT;
    }

    void set_is_en_passant_capture() {
        m_data |= IS_EN_PASSANT_CAPTURE_MASK;
    }

    bool is_en_passant_capture() const {
        return m_data & IS_EN_PASSANT_CAPTURE_MASK;
    }

    void set_promotion_type(PromotionType promotion_type) {
        m_data &= ~PROMOTION_TYPE_MASK;
        m_data |= static_cast<uint8_t>(promotion_type) << PROMOTION_TYPE_SHIFT;
    }

    const std::optional<PromotionType> get_promotion_type() const { 
        uint8_t ret = (m_data & PROMOTION_TYPE_MASK) >> PROMOTION_TYPE_SHIFT;
        return ret == (PROMOTION_TYPE_MASK >> PROMOTION_TYPE_SHIFT) ? 
            std::nullopt : std::optional<PromotionType>(static_cast<PromotionType>(ret));
    }

    void set_captured_piece_type(PieceType piece_type) {
        m_data &= ~CAPTURED_PIECE_TYPE_MASK;
        m_data |= static_cast<uint8_t>(piece_type) << CAPTURED_PIECE_TYPE_SHIFT;
    }

    const std::optional<PieceType> get_captured_piece_type() const {
        uint8_t ret = (m_data & CAPTURED_PIECE_TYPE_MASK) >> CAPTURED_PIECE_TYPE_SHIFT;
        return ret == (CAPTURED_PIECE_TYPE_MASK >> CAPTURED_PIECE_TYPE_SHIFT) ? 
            std::nullopt : std::optional<PieceType>(static_cast<PieceType>(ret));
    }

    void set_old_castling_rights(uint8_t castling_rights) {
        m_data &= ~OLD_CASTLING_RIGHTS_MASK;
        m_data |= castling_rights << OLD_CASTLING_RIGHTS_SHIFT;
    }

    uint8_t get_old_castling_rights() const{
        return static_cast<uint8_t>((m_data & OLD_CASTLING_RIGHTS_MASK) >> OLD_CASTLING_RIGHTS_SHIFT);
    }

private:
    uint32_t m_data;
};
