#include "board_location.h"
#include "board.h"
#include "pieces.h"


BoardLocation::BoardLocation(const BoardLocation& board_loc) :
    m_x(board_loc.m_x),
    m_y(board_loc.m_y),
    m_on_board(board_loc.m_on_board),
    m_valid(board_loc.m_valid),
    m_board(board_loc.m_board)
{
}


BoardLocation::BoardLocation(const std::string& loc_str, const Board& board) :
    m_on_board(true),
    m_valid(true),
    m_board(board)
{
    if (loc_str.size() != 2)
        throw(std::runtime_error(std::string("Invalid loc string ") + loc_str));

    int row = (int)(loc_str[1] - '1');
    if(row < 0 || row > 7)
        throw(std::runtime_error(std::string("Invalid loc string (row) ") + loc_str));

    int col = (int)loc_str[0] >= 'a' ? (int)(loc_str[0] - 'a') : (int)(loc_str[0] - 'A');
    if(col < 0 || col > 7)
        throw(std::runtime_error(std::string("Invalid loc string (col) ") + loc_str));

    m_x = col; m_y = row;
}


BoardLocation::BoardLocation(int x, int y, const Board& board) :
    m_x(x),
    m_y(y),
    m_on_board(true),
    m_valid(true),
    m_board(board)
{
}


BoardLocation::~BoardLocation()
{
}


BoardLocation BoardLocation::apply_move(const DynMove& dm) const
{
    return apply_move(dm.get_dx(), dm.get_dy());
}


BoardLocation BoardLocation::apply_move(int dx, int dy) const
{
    // move a location by a number of squares in the x and y directions
    // and return the result as a new BoardLocation

    // copy location
    BoardLocation new_loc(*this);
    new_loc.m_valid = new_loc.apply_move_inplace(dx, dy);
    return new_loc;
}


bool BoardLocation::apply_move_inplace(const DynMove& dm)
{
    return apply_move_inplace(dm.get_dx(), dm.get_dy());
}


bool BoardLocation::apply_move_inplace(int dx, int dy)
{
    // move a location by a number of squares in the x and y directions
    // if the resulting square is not on the board then the location is not
    // updated and false is returned

    if(!m_valid || !m_board.on_board(m_x+dx, m_y+dy))
        return false;

    m_x += dx;
    m_y += dy;
    return true;
}