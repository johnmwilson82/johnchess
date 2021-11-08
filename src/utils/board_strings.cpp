#include <sstream>
#include <cassert>

#include "board_strings.h"

namespace utils
{

Board board_from_string_repr(const std::string& board_str)
{
    auto s = board_str;
    std::vector<std::string> lines;

    size_t pos = 0;
    std::string line;
    while ((pos = s.find("\n")) != std::string::npos) {
        line = s.substr(0, pos);
        lines.push_back(line);

        s.erase(0, pos + 1);
    }

    size_t row_idx = 8;

    Board ret;
    
    for(auto& line : lines)
    {
        assert(row_idx);
        --row_idx;
        
        std::string sq;
        size_t col_idx = 0;
        while ((pos = line.find(" ")) != std::string::npos) {
            sq = line.substr(pos+1, pos+1);

            BoardLocation bl (col_idx, row_idx, ret);

            if(sq == "p")
            {
                ret.add_piece<Pawn>(Piece::BLACK, bl);
            }
            else if(sq == "r")
            {
                ret.add_piece<Rook>(Piece::BLACK, bl);
            }
            else if(sq == "n")
            {
                ret.add_piece<Knight>(Piece::BLACK, bl);
            }
            else if(sq == "b")
            {
                ret.add_piece<Bishop>(Piece::BLACK, bl);
            }
            else if(sq == "q")
            {
                ret.add_piece<Queen>(Piece::BLACK, bl);
            }
            else if(sq == "k")
            {
                ret.add_piece<King>(Piece::BLACK, bl);
            }
            else if(sq == "P")
            {
                ret.add_piece<Pawn>(Piece::WHITE, bl);
            }
            else if(sq == "R")
            {
                ret.add_piece<Rook>(Piece::WHITE, bl);
            }
            else if(sq == "N")
            {
                ret.add_piece<Knight>(Piece::WHITE, bl);
            }
            else if(sq == "B")
            {
                ret.add_piece<Bishop>(Piece::WHITE, bl);
            }
            else if(sq == "Q")
            {
                ret.add_piece<Queen>(Piece::WHITE, bl);
            }
            else if(sq == "K")
            {
                ret.add_piece<King>(Piece::WHITE, bl);
            }

            line.erase(0, pos + 2);
            ++col_idx;
        }

    }
    std::cout << s << std::endl;

    ret.populate_squares_properties();
    
    return ret;
}

std::string board_to_string_repr(const Board& board)
{
    std::ostringstream oss;

    for(int y = 7; y >= 0; --y)
    {
        for (int x = 0; x < 8; ++x)
        {
            const auto& sq = board.square(x, y);
            if(sq.is_empty())
            {
                oss << " _";
            }
            else
            {
                switch(sq.get_piece()->get_type())
                {
                    case Piece::KING:
                        oss << ((sq.get_piece()->get_colour() == Piece::WHITE) ? " K" : " k");
                        break;

                    case Piece::QUEEN:
                        oss << ((sq.get_piece()->get_colour() == Piece::WHITE) ? " Q" : " q");
                        break;

                    case Piece::ROOK:
                        oss << ((sq.get_piece()->get_colour() == Piece::WHITE) ? " R" : " r");
                        break;

                    case Piece::BISHOP:
                        oss << ((sq.get_piece()->get_colour() == Piece::WHITE) ? " B" : " b");
                        break;

                    case Piece::KNIGHT:
                        oss << ((sq.get_piece()->get_colour() == Piece::WHITE) ? " N" : " n");
                        break;

                    case Piece::PAWN:
                        oss << ((sq.get_piece()->get_colour() == Piece::WHITE) ? " P" : " p");
                        break;
                }
            }
            
        }
        oss << std::endl;
    }

    return oss.str();
}
}