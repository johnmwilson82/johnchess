#pragma once

#include <string>
#include <sstream>
#include <cassert>

#include "../iboard.h"

namespace utils
{
    template<typename T>
    concept IsBoard = std::is_base_of<IBoard, T>::value;

    template<IsBoard T>
    static void read_fen_props_line(const std::string& line, T& board)
    {
        size_t pos = 0;
        std::vector<std::string> tokens;

        std::string line_copy(line);

        while ((pos = line_copy.find(" ")) != std::string::npos) {
            tokens.push_back(line_copy.substr(0, pos));
            line_copy.erase(0, pos + 1);
        }
        tokens.push_back(line_copy);

        assert(tokens.size() == 5);

        board.set_colour_to_move(tokens[0] == "w" ? PieceColour::WHITE : PieceColour::BLACK);

        std::vector<IBoard::CastlingRights> castling_rights;
        for (auto chr : tokens[1])
        {
            switch (chr)
            {
            case 'K':
                castling_rights.push_back(IBoard::CastlingRights::WHITE_KINGSIDE);
                break;
            case 'Q':
                castling_rights.push_back(IBoard::CastlingRights::WHITE_QUEENSIDE);
                break;
            case 'k':
                castling_rights.push_back(IBoard::CastlingRights::BLACK_KINGSIDE);
                break;
            case 'q':
                castling_rights.push_back(IBoard::CastlingRights::BLACK_QUEENSIDE);
                break;
            }
        }
        board.set_castling_rights(castling_rights);

        if (tokens[2] != "-")
        {
            board.set_enpassant_column(tokens[2][0] - '0');
        }

        // fullmove clock and halfmove clock are tokens[3] and tokens[4] respectively
    }

    static std::string write_fen_props_line(const IBoard& board)
    {
        std::ostringstream oss;

        oss << (board.get_colour_to_move() == PieceColour::WHITE ? "w " : "b ");

        bool no_castling_rights = true;
        if (board.has_castling_rights(IBoard::CastlingRights::WHITE_KINGSIDE))
        {
            oss << "K";
            no_castling_rights = false;
        }
        if (board.has_castling_rights(IBoard::CastlingRights::WHITE_QUEENSIDE))
        {
            oss << "Q";
            no_castling_rights = false;
        }
        if (board.has_castling_rights(IBoard::CastlingRights::BLACK_KINGSIDE))
        {
            oss << "k";
            no_castling_rights = false;
        }
        if (board.has_castling_rights(IBoard::CastlingRights::BLACK_QUEENSIDE))
        {
            oss << "q";
            no_castling_rights = false;
        }

        if (no_castling_rights)
        {
            oss << "-";
        }

        oss << " ";

        const auto ep_col = board.get_enpassant_column();

        if (ep_col.has_value())
        {
            oss << ep_col.value() + "a";
            oss << (board.get_colour_to_move() == PieceColour::WHITE ? "6" : "3");
        }
        else
        {
            oss << "-";
        }

        oss << " 0 0";

        return oss.str();
    }

    template<IsBoard T>
    static T board_from_string_repr(const std::string& board_str)
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

        int row_idx = 8;

        T ret;

        for (auto& line : lines)
        {
            --row_idx;

            if (row_idx == -1)
            {
                read_fen_props_line(line, ret);
            }

            std::string sq;
            int col_idx = 0;
            while ((pos = line.find(" ")) != std::string::npos) {
                sq = line.substr(pos + 1, pos + 1);

                BoardLocation bl(col_idx, row_idx);

                if (sq == "p")
                {
                    ret.add_piece(PieceType::PAWN, PieceColour::BLACK, bl);
                }
                else if (sq == "r")
                {
                    ret.add_piece(PieceType::ROOK, PieceColour::BLACK, bl);
                }
                else if (sq == "n")
                {
                    ret.add_piece(PieceType::KNIGHT, PieceColour::BLACK, bl);
                }
                else if (sq == "b")
                {
                    ret.add_piece(PieceType::BISHOP, PieceColour::BLACK, bl);
                }
                else if (sq == "q")
                {
                    ret.add_piece(PieceType::QUEEN, PieceColour::BLACK, bl);
                }
                else if (sq == "k")
                {
                    ret.add_piece(PieceType::KING, PieceColour::BLACK, bl);
                }
                else if (sq == "P")
                {
                    ret.add_piece(PieceType::PAWN, PieceColour::WHITE, bl);
                }
                else if (sq == "R")
                {
                    ret.add_piece(PieceType::ROOK, PieceColour::WHITE, bl);
                }
                else if (sq == "N")
                {
                    ret.add_piece(PieceType::KNIGHT, PieceColour::WHITE, bl);
                }
                else if (sq == "B")
                {
                    ret.add_piece(PieceType::BISHOP, PieceColour::WHITE, bl);
                }
                else if (sq == "Q")
                {
                    ret.add_piece(PieceType::QUEEN, PieceColour::WHITE, bl);
                }
                else if (sq == "K")
                {
                    ret.add_piece(PieceType::KING, PieceColour::WHITE, bl);
                }

                line.erase(0, pos + 2);
                ++col_idx;
            }
        }
        std::cout << s << std::endl;

        ret.populate_squares_properties();

        return ret;
    }

    template<IsBoard T>
    static std::string board_to_string_repr(const T& board)
    {
        std::ostringstream oss;

        for (int y = 7; y >= 0; --y)
        {
            for (int x = 0; x < 8; ++x)
            {
                const auto& sq = board.square(x, y);
                if (sq.is_empty())
                {
                    oss << " _";
                }
                else
                {
                    switch (sq.get_piece()->get_type())
                    {
                    case PieceType::KING:
                        oss << ((sq.get_piece()->get_colour() == PieceColour::WHITE) ? " K" : " k");
                        break;

                    case PieceType::QUEEN:
                        oss << ((sq.get_piece()->get_colour() == PieceColour::WHITE) ? " Q" : " q");
                        break;

                    case PieceType::ROOK:
                        oss << ((sq.get_piece()->get_colour() == PieceColour::WHITE) ? " R" : " r");
                        break;

                    case PieceType::BISHOP:
                        oss << ((sq.get_piece()->get_colour() == PieceColour::WHITE) ? " B" : " b");
                        break;

                    case PieceType::KNIGHT:
                        oss << ((sq.get_piece()->get_colour() == PieceColour::WHITE) ? " N" : " n");
                        break;

                    case PieceType::PAWN:
                        oss << ((sq.get_piece()->get_colour() == PieceColour::WHITE) ? " P" : " p");
                        break;
                    }
                }

            }
            oss << std::endl;
        }

        oss << write_fen_props_line(board) << std::endl;

        return oss.str();
    }
};