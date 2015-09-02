#include "pieces.h"
#include <iostream>

Piece::~Piece()
{
}

Move::Move(Piece *piece, DynMove dm) :
    m_piece(piece)
{
    m_new_loc = piece->get_loc().apply_move(dm);
}

std::string Move::to_string()
{
    return m_piece->get_loc().to_string() + m_new_loc.to_string();
}

std::vector<Move> Piece::get_all_slide_moves(std::vector<DynMove> dms, Board& board)
{
    std::vector<Move> ret;
    for(std::vector<DynMove>::iterator it = dms.begin(); it != dms.end(); ++it)
    {
        BoardLocation curr_loc = m_loc;
        while(true)
        {
            BoardLocation new_loc = curr_loc.apply_move(*it);
            //std::cout << new_loc.get_x() << ", " << new_loc.get_y() << " : " << m_loc.get_x() << ", " << m_loc.get_y() << std::endl;
            if (new_loc.get_valid())
            {
                if (board.square(new_loc).is_empty())
                {
                    ret.push_back(Move(this, new_loc));
                    curr_loc = new_loc;
                }
                else if (board.square(new_loc).get_piece()->get_colour() != m_colour)
                {
                    ret.push_back(Move(this, new_loc));
                    break;
                }
                else
                    break;
            }
            else
                break;
        }
    }
    return ret;
}

std::vector<Move> Piece::get_all_hop_moves(std::vector<DynMove> dms, Board& board)
{
    std::vector<Move> ret;
    for(std::vector<DynMove>::iterator it = dms.begin(); it != dms.end(); ++it)
    {
        BoardLocation new_loc = m_loc.apply_move(*it);
        if (new_loc.get_valid() && (board.square(new_loc).is_empty() || (board.square(new_loc).get_piece()->get_colour() != m_colour)))
            ret.push_back(Move(this, new_loc));
    }
    return ret;
}

Pawn::Pawn(Colour colour, BoardLocation(loc)) :
    Piece(colour, loc)
{
}

std::vector<Move> Pawn::get_all_valid_moves(Board& board)
{
    int dir = m_colour == Piece::WHITE ? 1 : -1;
    std::vector<Move> ret;
    BoardLocation new_loc;
    BoardLocation curr_loc = m_loc;
    int j = ((m_colour == Piece::WHITE) && (m_loc.get_y() == 1)) ||
            ((m_colour == Piece::BLACK) && (m_loc.get_y() == 6)) ? 2 : 1;
    for(int i = 0; i < j; i++)
    {
        new_loc = curr_loc.apply_move(0, dir);
        if (new_loc.get_valid() && board.square(new_loc).is_empty())
        {
            ret.push_back(Move(this, new_loc));
            curr_loc = new_loc;
        }
        else
            break;
    }
    new_loc = m_loc.apply_move(1, dir);
    if (new_loc.get_valid() && !board.square(new_loc).is_empty() && board.square(new_loc).get_piece()->get_colour() != m_colour)
        ret.push_back(Move(this, new_loc));

    new_loc = m_loc.apply_move(-1, dir);
    if (new_loc.get_valid() && !board.square(new_loc).is_empty() && board.square(new_loc).get_piece()->get_colour() != m_colour)
        ret.push_back(Move(this, new_loc));

    /*std::cout << "piece location = " << m_loc.get_x() << ", " << m_loc.get_y() << std::endl;
    for (int i = 0; i < ret.size(); i++)
    {
    std::cout << "move " << i+1 << ": " << ret[i].get_board_loc().get_x() << ", " << ret[i].get_board_loc().get_y() << std::endl;
    }*/

    return ret;
}

std::vector<std::string> Pawn::get_symbol_list()
{
    std::vector<std::string> symbs;
    symbs.push_back("P");
    return symbs;
}

King::King(Colour colour, BoardLocation(loc)) :
    Piece(colour, loc)
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

std::vector<Move> King::get_all_valid_moves(Board& board)
{
    return get_all_hop_moves(m_dm_list, board);
}

std::vector<std::string> King::get_symbol_list()
{
    std::vector<std::string> symbs;
    symbs.push_back("K");
    return symbs;
}

Queen::Queen(Colour colour, BoardLocation(loc)) :
    Piece(colour, loc)
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

std::vector<Move> Queen::get_all_valid_moves(Board& board)
{

    return get_all_slide_moves(m_dm_list, board);
}

std::vector<std::string> Queen::get_symbol_list()
{
    std::vector<std::string> symbs;
    symbs.push_back("Q");
    return symbs;
}

Rook::Rook(Colour colour, BoardLocation(loc)) :
    Piece(colour, loc)
{
    std::vector<DynMove> dm_list;
    dm_list.push_back(DynMove(-1, 0));
    dm_list.push_back(DynMove(0, 1));
    dm_list.push_back(DynMove(1, 0));
    dm_list.push_back(DynMove(0, -1));
    m_dm_list = dm_list;
}

std::vector<Move> Rook::get_all_valid_moves(Board& board)
{
    return get_all_slide_moves(m_dm_list, board);
}

std::vector<std::string> Rook::get_symbol_list()
{
    std::vector<std::string> symbs;
    symbs.push_back("R");
    return symbs;
}

Bishop::Bishop(Colour colour, BoardLocation(loc)) :
    Piece(colour, loc)
{
    std::vector<DynMove> dm_list;
    dm_list.push_back(DynMove(-1, -1));
    dm_list.push_back(DynMove(-1, 1));
    dm_list.push_back(DynMove(1, 1));
    dm_list.push_back(DynMove(1, -1));
    m_dm_list = dm_list;
}


std::vector<Move> Bishop::get_all_valid_moves(Board& board)
{
    return get_all_slide_moves(m_dm_list, board);
}

std::vector<std::string> Bishop::get_symbol_list()
{
    std::vector<std::string> symbs;
    symbs.push_back("B");
    return symbs;
}

Knight::Knight(Colour colour, BoardLocation(loc)) :
    Piece(colour, loc)
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

std::vector<Move> Knight::get_all_valid_moves(Board& board)
{

    return get_all_hop_moves(m_dm_list, board);
}

std::vector<std::string> Knight::get_symbol_list()
{
    std::vector<std::string> symbs;
    symbs.push_back("N");
    symbs.push_back("Kn");
    return symbs;
}
