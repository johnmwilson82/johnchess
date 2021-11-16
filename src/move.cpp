#include "move.h"

#include "pieces.h"
#include "board.h"

Move::Move(const Piece &piece, const DynMove& dm) :
    m_piece(piece),
    m_new_loc(piece.get_loc().apply_move(dm))
{}

Move::Move(const Piece &piece, const BoardLocation& new_loc) :
    m_piece(piece),
    m_new_loc(new_loc)
{
}

Move::Move(const Board& board, const std::string& move_str) :
    m_piece(*board.square(BoardLocation(move_str.substr(0, 2), board)).get_piece()),
    m_new_loc(move_str.substr(2, 2), board)
{
    if(move_str.length() == 5)
    {
        switch(move_str[4])
        {
            case 'q':
            case 'Q':
                m_promotion_type = PromotionType::QUEEN;
                break;

            case 'R':
            case 'r':
                m_promotion_type = PromotionType::ROOK;
                break;

            case 'B':
            case 'b':
                m_promotion_type = PromotionType::BISHOP;
                break;

            case 'N':
            case 'n':
                m_promotion_type = PromotionType::KNIGHT;
                break;
        }
    }
}

bool Move::operator== (const Move& m) const
{
    return (std::addressof(m_piece) == std::addressof(m.m_piece)) && (m_new_loc == m.m_new_loc);
}

std::string Move::to_string() const
{
    std::string move_str = m_piece.get_loc().to_string() + m_new_loc.to_string();

    if(m_promotion_type.has_value())
    {
        switch (*m_promotion_type)
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
    return m_new_loc;
}

const BoardLocation Move::get_from_loc() const
{
    return m_piece.get_loc();
}