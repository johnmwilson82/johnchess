#include "move.h"

#include "bitboards/bitboard.h"


Move::Move(const BoardLocation& from_loc, const BoardLocation& to_loc) :
    m_data((from_loc.get_raw() << FROM_LOC_SHIFT) | (to_loc.get_raw() << TO_LOC_SHIFT) | INIT_DATA)
{
}


Move::Move(const std::string& move_str) :
    m_data((BoardLocation(move_str.substr(0, 2)).get_raw() << FROM_LOC_SHIFT) | 
           (BoardLocation(move_str.substr(2, 2)).get_raw() << TO_LOC_SHIFT) | INIT_DATA)
{
    if(move_str.length() == 5)
    {
        switch(move_str[4])
        {
            case 'q':
            case 'Q':
                set_promotion_type(PromotionType::QUEEN);
                break;

            case 'R':
            case 'r':
                set_promotion_type(PromotionType::ROOK);
                break;

            case 'B':
            case 'b':
                set_promotion_type(PromotionType::BISHOP);
                break;

            case 'N':
            case 'n':
                set_promotion_type(PromotionType::KNIGHT);
                break;
        }
    }
}

bool Move::operator== (const Move& m) const
{
    constexpr uint32_t compare_mask = FROM_LOC_MASK | TO_LOC_MASK | PROMOTION_TYPE_MASK;
    return (m_data & compare_mask) == (m.m_data & compare_mask);
}

std::string Move::to_string() const
{
    auto thing1 = BoardLocation(get_from_loc());
    auto thing2 = BoardLocation(get_to_loc());

    std::string move_str = BoardLocation(get_from_loc()).to_string() + BoardLocation(get_to_loc()).to_string();

    auto promotion_type = get_promotion_type();

    if(promotion_type.has_value())
    {
        switch (*promotion_type)
        {
        case PromotionType::BISHOP:
            move_str += "b";
            break;
        
        case PromotionType::KNIGHT:
            move_str += "n";
            break;
        
        case PromotionType::QUEEN:
            move_str += "q";
            break;
        
        case PromotionType::ROOK:
            move_str += "r";
            break;
        }
    }

    return move_str;
}