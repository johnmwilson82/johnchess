#include "move.h"

#include "pieces.h"
#include "iboard.h"


Move::Move(const BoardLocation& from_loc, const DynMove& dm) :
    m_from_loc(from_loc),
    m_to_loc(from_loc.apply_move(dm))
{
}

Move::Move(const BoardLocation& from_loc, const BoardLocation& to_loc) :
    m_from_loc(from_loc),
    m_to_loc(to_loc)
{
}

Move::Move(const IBoard& board, const std::string& move_str) :
    m_from_loc(move_str.substr(0, 2)),
    m_to_loc(move_str.substr(2, 2))
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
    return (m_from_loc == m.m_from_loc) && (m_to_loc == m.m_to_loc);
}

std::string Move::to_string() const
{
    std::string move_str = m_from_loc.to_string() + m_to_loc.to_string();

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

const BoardLocation& Move::get_to_loc() const
{
    return m_to_loc;
}

const BoardLocation Move::get_from_loc() const
{
    return m_from_loc;
}