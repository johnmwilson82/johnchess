#include <stdexcept>
#include "board_location.h"

BoardLocation::BoardLocation(uint8_t bitboard_sq) :
    m_loc(bitboard_sq)
{
}


BoardLocation::BoardLocation(const BoardLocation& board_loc) :
    m_loc(board_loc.m_loc)
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

    m_loc = row * 8 + col;
}


BoardLocation::BoardLocation(int x, int y) :
    m_loc(y * 8 + x)
{
}