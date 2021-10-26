#include "pieces.h"
#include "board.h"
#include <iostream>

Piece::~Piece()
{
}



std::list<Move> Piece::get_all_slide_moves(const std::vector<DynMove>& dms, const Board& board, bool check_test) const
{
    std::list<Move> ret;
    for(const auto& move : dms)
    {
        BoardLocation curr_loc(m_loc);
        while(true)
        {
            if (curr_loc.apply_move_inplace(move))
            {
                if (board.square(curr_loc).is_empty())
                {
                    // Move
                    ret.emplace_back(*this, curr_loc);
                }
                else if (board.square(curr_loc).get_piece()->get_colour() != m_colour)
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


std::list<Move> Piece::get_all_hop_moves(const std::vector<DynMove>& dms, const Board& board, bool check_test) const
{
    std::list<Move> ret;
    for(const auto& move : dms)
    {
        BoardLocation curr_loc(m_loc);

        if (curr_loc.apply_move_inplace(move) && 
            (board.square(curr_loc).is_empty() || (board.square(curr_loc).get_piece()->get_colour() != m_colour)))
        {
            ret.emplace_back(*this, curr_loc);
        }
    }
    return ret;
}


Pawn::Pawn(Colour colour, BoardLocation(loc)) :
    Piece(colour, loc, PAWN)
{
}


std::list<Move> Pawn::get_all_valid_moves(const Board& board) const
{
    int dir = m_colour == Piece::WHITE ? 1 : -1;
    std::list<Move> ret;

    // Advance
    int j = ((m_colour == Piece::WHITE) && (m_loc.get_y() == 1)) ||
            ((m_colour == Piece::BLACK) && (m_loc.get_y() == 6)) ? 2 : 1;
    for(int i = 0; i < j; i++)
    {
        BoardLocation adv_loc(m_loc.apply_move(0, dir * (i+1)));
        if (adv_loc.get_valid() && board.square(adv_loc).is_empty())
        {
            ret.emplace_back(*this, adv_loc);
        }
        else
            break;
    }

    // Take right
    BoardLocation tr_loc = m_loc.apply_move(1, dir);
    if (tr_loc.get_valid() && !board.square(tr_loc).is_empty() &&
        board.square(tr_loc).get_piece()->get_colour() != m_colour)
    {
        ret.emplace_back(*this, tr_loc);
    }

    // Take left
    BoardLocation tl_loc = m_loc.apply_move(-1, dir);
    if (tl_loc.get_valid() && !board.square(tl_loc).is_empty() &&
        board.square(tl_loc).get_piece()->get_colour() != m_colour)
    {
        ret.emplace_back(*this, tl_loc);
    }

    return ret;
}


std::vector<std::string> Pawn::get_symbol_list() const
{
    std::vector<std::string> symbs;
    symbs.push_back("P");
    return symbs;
}


King::King(Colour colour, BoardLocation(loc)) :
    Piece(colour, loc, KING)
{
    std::vector<DynMove> dm_list;
    dm_list.push_back(DynMove(-1, -1));
    dm_list.push_back(DynMove(-1, 0));
    dm_list.push_back(DynMove(-1, 1));
    dm_list.push_back(DynMove(0, 1));
    dm_list.push_back(DynMove(1, 1));
    dm_list.push_back(DynMove(1, 0));
    dm_list.push_back(DynMove(1, -1));
    dm_list.push_back(DynMove(0, -1));
    m_dm_list = dm_list;
}


std::list<Move> King::get_all_valid_moves(const Board& board) const
{
    return get_all_hop_moves(m_dm_list, board, true);
}


std::vector<std::string> King::get_symbol_list() const
{
    std::vector<std::string> symbs;
    symbs.push_back("K");
    return symbs;
}


Queen::Queen(Colour colour, BoardLocation(loc)) :
    Piece(colour, loc, QUEEN)
{
    std::vector<DynMove> dm_list;
    dm_list.push_back(DynMove(-1, -1));
    dm_list.push_back(DynMove(-1, 0));
    dm_list.push_back(DynMove(-1, 1));
    dm_list.push_back(DynMove(0, 1));
    dm_list.push_back(DynMove(1, 1));
    dm_list.push_back(DynMove(1, 0));
    dm_list.push_back(DynMove(1, -1));
    dm_list.push_back(DynMove(0, -1));
    m_dm_list = dm_list;
}


std::list<Move> Queen::get_all_valid_moves(const Board& board) const
{

    return get_all_slide_moves(m_dm_list, board, true);
}


std::vector<std::string> Queen::get_symbol_list() const
{
    std::vector<std::string> symbs;
    symbs.push_back("Q");
    return symbs;
}


Rook::Rook(Colour colour, BoardLocation(loc)) :
    Piece(colour, loc, ROOK)
{
    std::vector<DynMove> dm_list;
    dm_list.push_back(DynMove(-1, 0));
    dm_list.push_back(DynMove(0, 1));
    dm_list.push_back(DynMove(1, 0));
    dm_list.push_back(DynMove(0, -1));
    m_dm_list = dm_list;
}


std::list<Move> Rook::get_all_valid_moves(const Board& board) const
{
    return get_all_slide_moves(m_dm_list, board, true);
}


std::vector<std::string> Rook::get_symbol_list() const
{
    std::vector<std::string> symbs;
    symbs.push_back("R");
    return symbs;
}


Bishop::Bishop(Colour colour, BoardLocation(loc)) :
    Piece(colour, loc, BISHOP)
{
    std::vector<DynMove> dm_list;
    dm_list.push_back(DynMove(-1, -1));
    dm_list.push_back(DynMove(-1, 1));
    dm_list.push_back(DynMove(1, 1));
    dm_list.push_back(DynMove(1, -1));
    m_dm_list = dm_list;
}


std::list<Move> Bishop::get_all_valid_moves(const Board& board) const
{
    return get_all_slide_moves(m_dm_list, board, true);
}


std::vector<std::string> Bishop::get_symbol_list() const
{
    std::vector<std::string> symbs;
    symbs.push_back("B");
    return symbs;
}


Knight::Knight(Colour colour, BoardLocation(loc)) :
    Piece(colour, loc, KNIGHT)
{
    std::vector<DynMove> dm_list;
    dm_list.push_back(DynMove(-1, 2));
    dm_list.push_back(DynMove(-1, -2));
    dm_list.push_back(DynMove(-2, 1));
    dm_list.push_back(DynMove(-2, -1));
    dm_list.push_back(DynMove(1, 2));
    dm_list.push_back(DynMove(1, -2));
    dm_list.push_back(DynMove(2, 1));
    dm_list.push_back(DynMove(2, -1));
    m_dm_list = dm_list;
}


std::list<Move> Knight::get_all_valid_moves(const Board& board) const
{

    return get_all_hop_moves(m_dm_list, board, true);
}


std::vector<std::string> Knight::get_symbol_list() const
{
    std::vector<std::string> symbs;
    symbs.push_back("N");
    symbs.push_back("Kn");
    return symbs;
}
