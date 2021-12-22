#include "pieces.h"
#include "board.h"

#include <iostream>

Piece::~Piece()
{
}


template<std::size_t SIZE>
std::list<Move> Piece::get_all_slide_moves(const std::array<DynMove, SIZE>& dms, bool check_test) const
{
    std::list<Move> ret;
    for(const auto& move : dms)
    {
        BoardLocation curr_loc(m_loc);
        while(true)
        {
            if (curr_loc.apply_move_inplace(move))
            {
                if (m_board.square(curr_loc).is_empty())
                {
                    // Move
                    ret.emplace_back(*this, curr_loc);
                }
                else if (m_board.square(curr_loc).get_piece()->get_colour() != m_colour)
                {
                    // Capture
                    ret.emplace_back(*this, curr_loc);
                    break;
                }
                else // Can't move in given direction
                    break;
            }
            else
                break;
        }
    }
    return ret;
}

template<std::size_t SIZE>
std::list<Move> Piece::get_all_hop_moves(const std::array<DynMove, SIZE>& dms, bool check_test) const
{
    std::list<Move> ret;
    for(const auto& move : dms)
    {
        BoardLocation curr_loc(m_loc);

        if (curr_loc.apply_move_inplace(move) && 
            (m_board.square(curr_loc).is_empty() || (m_board.square(curr_loc).get_piece()->get_colour() != m_colour)))
        {
            ret.emplace_back(*this, curr_loc);
        }
    }
    return ret;
}


Pawn::Pawn(const Board& board, Colour colour, BoardLocation(loc)) :
    ClonablePiece<Pawn>(board, colour, loc, PAWN)
{
}


std::list<Move> Pawn::get_all_valid_moves() const
{
    int dir = m_colour == Piece::WHITE ? 1 : -1;
    std::list<Move> ret;

    // Promotion
    auto emplace_pawn_move = [&] (const BoardLocation& adv_loc)
    {
        if((m_colour == Piece::WHITE && adv_loc.get_y() == 7) ||
           (m_colour == Piece::BLACK && adv_loc.get_y() == 0))
        {
            for(auto promote_to : { Move::PromotionType::QUEEN, Move::PromotionType::ROOK, Move::PromotionType::BISHOP, Move::PromotionType::KNIGHT })
            {
                auto& move = ret.emplace_back(*this, adv_loc);
                move.set_promotion_type(promote_to);
            }
        }
        else
        {
            ret.emplace_back(*this, adv_loc);
        }
    };

    // Advance
    int j = ((m_colour == Piece::WHITE) && (m_loc.get_y() == 1)) ||
            ((m_colour == Piece::BLACK) && (m_loc.get_y() == 6)) ? 2 : 1;
    for(int i = 0; i < j; i++)
    {
        BoardLocation adv_loc(m_loc.apply_move(0, dir * (i+1)));
        if (adv_loc.get_valid() && m_board.square(adv_loc).is_empty())
        {
            emplace_pawn_move(adv_loc);
        }
        else
            break;
    }

    // Take right
    BoardLocation tr_loc = m_loc.apply_move(1, dir);
    if (tr_loc.get_valid() && !m_board.square(tr_loc).is_empty() &&
        m_board.square(tr_loc).get_piece()->get_colour() != m_colour)
    {
        emplace_pawn_move(tr_loc);
    }

    // Take left
    BoardLocation tl_loc = m_loc.apply_move(-1, dir);
    if (tl_loc.get_valid() && !m_board.square(tl_loc).is_empty() &&
        m_board.square(tl_loc).get_piece()->get_colour() != m_colour)
    {
        emplace_pawn_move(tl_loc);
    }

    // Check en passant
    if((m_colour == Piece::WHITE && m_loc.get_y() == 4) ||
       (m_colour == Piece::BLACK && m_loc.get_y() == 3))
    {
        BoardLocation enpassant_tr_loc = m_loc.apply_move(1, 0);
        if (tr_loc.get_valid() && 
            !m_board.square(enpassant_tr_loc).is_empty() &&
            m_board.square(enpassant_tr_loc).get_piece()->get_colour() != m_colour &&
            m_board.square(enpassant_tr_loc).get_piece()->capturable_en_passant())
        {
            emplace_pawn_move(tr_loc);
        }

        BoardLocation enpassant_tl_loc = m_loc.apply_move(-1, 0);
        if (tl_loc.get_valid() && 
            !m_board.square(enpassant_tl_loc).is_empty() &&
            m_board.square(enpassant_tl_loc).get_piece()->get_colour() != m_colour &&
            m_board.square(enpassant_tl_loc).get_piece()->capturable_en_passant())
        {
            emplace_pawn_move(tl_loc);
        }
    }
    
    return ret;
}


std::vector<std::string> Pawn::get_symbol_list() const
{
    std::vector<std::string> symbs;
    symbs.push_back("P");
    return symbs;
}


King::King(const Board& board, Colour colour, BoardLocation(loc)) :
    ClonablePiece<King>(board, colour, loc, KING)
{
}


std::list<Move> King::get_all_valid_moves() const
{
    bool can_castle_ks = false;
    bool can_castle_qs = false;

    if(!m_moved)
    {
        const auto& right_sq1 = m_board.square(m_loc.apply_move(1, 0));
        const auto& right_sq2 = m_board.square(m_loc.apply_move(2, 0));
        const auto& right_rook_sq = m_board.square(m_loc.apply_move(3, 0));

        if(right_sq1.is_empty() && right_sq1.get_attackers(opposite_colour(m_colour)) == 0 &&
           right_sq2.is_empty() && right_sq2.get_attackers(opposite_colour(m_colour)) == 0 &&
           !right_rook_sq.is_empty() && !right_rook_sq.get_piece()->has_moved())
        {
            can_castle_ks = true;
        }

        const auto& left_sq1 = m_board.square(m_loc.apply_move(-1, 0));
        const auto& left_sq2 = m_board.square(m_loc.apply_move(-2, 0));
        const auto& left_sq3 = m_board.square(m_loc.apply_move(-3, 0));
        const auto& left_rook_sq = m_board.square(m_loc.apply_move(-4, 0));

        if(left_sq1.is_empty() && left_sq1.get_attackers(opposite_colour(m_colour)) == 0 &&
           left_sq2.is_empty() && left_sq2.get_attackers(opposite_colour(m_colour)) == 0 &&
           left_sq3.is_empty() && left_sq3.get_attackers(opposite_colour(m_colour)) == 0 &&
           !left_rook_sq.is_empty() && !left_rook_sq.get_piece()->has_moved())
        {
            can_castle_qs = true;
        }
    }

    std::list<Move> ret = get_all_hop_moves(m_dm_list, true);

    if(can_castle_qs)
    {
        if(m_colour == WHITE)
        {
            ret.emplace_back(*this, BoardLocation("c1", m_board));
        }
        else
        {
            ret.emplace_back(*this, BoardLocation("c8", m_board));
        }
    }

    if(can_castle_ks)
    {
        if(m_colour == WHITE)
        {
            ret.emplace_back(*this, BoardLocation("g1", m_board));
        }
        else
        {
            ret.emplace_back(*this, BoardLocation("g8", m_board));
        }
    }

    return ret;
}


std::vector<std::string> King::get_symbol_list() const
{
    std::vector<std::string> symbs;
    symbs.push_back("K");
    return symbs;
}


Queen::Queen(const Board& board, Colour colour, BoardLocation(loc)) :
    ClonablePiece<Queen>(board, colour, loc, QUEEN)
{
}


std::list<Move> Queen::get_all_valid_moves() const
{

    return get_all_slide_moves(m_dm_list, true);
}


std::vector<std::string> Queen::get_symbol_list() const
{
    std::vector<std::string> symbs;
    symbs.push_back("Q");
    return symbs;
}


Rook::Rook(const Board& board, Colour colour, BoardLocation(loc)) :
    ClonablePiece<Rook>(board, colour, loc, ROOK)
{
}


std::list<Move> Rook::get_all_valid_moves() const
{
    return get_all_slide_moves(m_dm_list, true);
}


std::vector<std::string> Rook::get_symbol_list() const
{
    std::vector<std::string> symbs;
    symbs.push_back("R");
    return symbs;
}


Bishop::Bishop(const Board& board, Colour colour, BoardLocation(loc)) :
    ClonablePiece<Bishop>(board, colour, loc, BISHOP)
{
}


std::list<Move> Bishop::get_all_valid_moves() const
{
    return get_all_slide_moves(m_dm_list, true);
}


std::vector<std::string> Bishop::get_symbol_list() const
{
    std::vector<std::string> symbs;
    symbs.push_back("B");
    return symbs;
}


Knight::Knight(const Board& board, Colour colour, BoardLocation(loc)) :
    ClonablePiece<Knight>(board, colour, loc, KNIGHT)
{
}


std::list<Move> Knight::get_all_valid_moves() const
{

    return get_all_hop_moves(m_dm_list, true);
}


std::vector<std::string> Knight::get_symbol_list() const
{
    std::vector<std::string> symbs;
    symbs.push_back("N");
    symbs.push_back("Kn");
    return symbs;
}
