#include "board_location.h"
//#include "pieces.h"


BoardLocation::BoardLocation(uint8_t bitboard_sq) :
    m_x(bitboard_sq % 8),
    m_y(bitboard_sq / 8)
{
}


BoardLocation::BoardLocation(const BoardLocation& board_loc) :
    m_x(board_loc.m_x),
    m_y(board_loc.m_y)
{
}


BoardLocation::BoardLocation(const std::string& loc_str)
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


BoardLocation::BoardLocation(int x, int y) :
    m_x(x),
    m_y(y)
{
}