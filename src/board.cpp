#include <stdexcept>

#include "board.h"


Board::Board() :
    m_squares(BOARD_DIM*BOARD_DIM),
    m_pieces(2)
{
    //set_to_start_position();
}

Board::Board(const Board& orig) :
    m_squares(BOARD_DIM*BOARD_DIM),
    m_colour_to_move(orig.m_colour_to_move),
    m_pieces(2)
{
    for(const auto& piece : orig.m_pieces)
    {
        if(piece->get_on_board())
        {
            add_piece(piece->get_type(), piece->get_colour(), piece->get_loc());
        }
    }

    populate_squares_properties();
}

Board::Board(const Board& orig, const Move& move) :
    m_squares(BOARD_DIM*BOARD_DIM),
    m_colour_to_move(orig.m_colour_to_move),
    m_pieces(2)
{
    for(const auto& piece : orig.m_pieces)
    {
        if(piece->get_on_board())
        {
            add_piece(piece->get_type(), piece->get_colour(), piece->get_loc());
        }
    }

    move_piece(move.get_from_loc(), move.get_to_loc());

    populate_squares_properties();
}

Board::Board(const Board& orig, const std::string& move_str) :
    m_squares(BOARD_DIM*BOARD_DIM),
    m_colour_to_move(orig.m_colour_to_move),
    m_pieces(2)
{
    for(const auto& piece : orig.m_pieces)
    {
        if(piece->get_on_board())
        {
            add_piece(piece->get_type(), piece->get_colour(), piece->get_loc());
        }
    }

    std::string curr_loc = move_str.substr(0, 2);
    std::string new_loc = move_str.substr(2);

    move_piece(BoardLocation(curr_loc, *this), BoardLocation(new_loc, *this));

    populate_squares_properties();
}

void Board::populate_squares_properties()
{
    for(const auto& piece : m_pieces)
    {
        for(const auto& move : piece->get_all_valid_moves(*this))
        {
            square(move.get_to_loc()).add_attacker(piece);
        }
    }
}

void Board::delete_all_pieces()
{
    for(auto& sq : m_squares)
    {
        sq.remove_piece();
    }
    m_pieces.clear();
    m_pieces.resize(2);
}

bool Board::add_piece(Piece::Type type, Piece::Colour col, std::string loc)
{
    return add_piece(type, col, BoardLocation(loc, *this));
}

bool Board::add_piece(Piece::Type type, Piece::Colour col, BoardLocation loc)
{
    if (!square(loc).is_empty())
        return false;

    std::shared_ptr<Piece> new_piece;
    switch (type)
    {
        case Piece::KING:
            // Insert kings so we have [0] -> white king and [1] -> black king
            new_piece = std::make_shared<King>(col, loc);
            m_pieces[col == Piece::WHITE ? 0 : 1] = new_piece;
            break;
        case Piece::QUEEN:
            new_piece = m_pieces.emplace_back(std::make_shared<Queen>(col, loc));
            break;
        case Piece::ROOK:
            new_piece = m_pieces.emplace_back(std::make_shared<Rook>(col, loc));
            break;
        case Piece::KNIGHT:
            new_piece = m_pieces.emplace_back(std::make_shared<Knight>(col, loc));
            break;
        case Piece::BISHOP:
            new_piece = m_pieces.emplace_back(std::make_shared<Bishop>(col, loc));
            break;
        case Piece::PAWN:
            new_piece = m_pieces.emplace_back(std::make_shared<Pawn>(col, loc));
            break;
    }

    square(loc).set_piece(new_piece);
    return true;
}

bool Board::remove_piece(std::string loc)
{
    return remove_piece(BoardLocation(loc, *this));
}

bool Board::remove_piece(BoardLocation loc)
{
    if(square(loc).is_empty())
        return false;

    auto piece = square(loc).get_piece();
    piece->set_on_board(false);
    return true;
}

std::optional<std::pair<std::shared_ptr<Piece>, BoardLocation>> Board::check_castling_rook_move(const Piece& moving_piece, const BoardLocation& new_loc)
{
    if(moving_piece.get_type() != Piece::KING)
    {
        // Not a king move
        return {};
    }

    // White king
    if(moving_piece.get_loc() == BoardLocation("e1", *this))
    {
        if(new_loc == BoardLocation("g1", *this))
        {
            // King's side castling
            return std::make_pair(square(7, 0).get_piece(), BoardLocation("f1", *this));
        }

        if(new_loc == BoardLocation("c1", *this))
        {
            // Queens's side castling
            return std::make_pair(square(0, 0).get_piece(), BoardLocation("d1", *this));
        }
    }

    // Black king
    if(moving_piece.get_loc() == BoardLocation("e8", *this))
    {
        if(new_loc == BoardLocation("g8", *this))
        {
            // King's side castling
            return std::make_pair(square(7, 7).get_piece(), BoardLocation("f8", *this));
        }

        if(new_loc == BoardLocation("c8", *this))
        {
            // Queens's side castling
            return std::make_pair(square(0, 7).get_piece(), BoardLocation("d8", *this));
        }
    }

    return {};
}

bool Board::move_piece(std::string move_str)
{
    std::string curr_loc = move_str.substr(0, 2);
    std::string new_loc = move_str.substr(2);

    m_colour_to_move = (m_colour_to_move == Piece::WHITE ? Piece::BLACK : Piece::WHITE);
    return move_piece(BoardLocation(curr_loc, *this), BoardLocation(new_loc, *this));
}

bool Board::move_piece(const BoardLocation& curr_loc, const BoardLocation& new_loc)
{
    auto captured_piece = square(new_loc).get_piece();
    auto moving_piece = square(curr_loc).get_piece();

    if(!moving_piece)
        return false;

    // Check castling
    auto castling_rook_move = check_castling_rook_move(*moving_piece, new_loc);

    moving_piece->move(new_loc);

    if(captured_piece)
    {
        register_captured(captured_piece);
        remove_piece(new_loc);
    }

    square(new_loc).set_piece(moving_piece);
    square(curr_loc).remove_piece();

    // Implement castling
    if(castling_rook_move.has_value())
    {
        // Move is a castle move
        auto rook = castling_rook_move->first;
        auto rook_new_loc = castling_rook_move->second;
        auto rook_curr_loc = rook->get_loc();

        rook->move(rook_new_loc);
        square(rook_new_loc).set_piece(rook);
        square(rook_curr_loc).remove_piece();
    }

    m_colour_to_move = (m_colour_to_move == Piece::WHITE ? Piece::BLACK : Piece::WHITE);
    return true;
}

void Board::set_to_start_position()
{
    delete_all_pieces();
    // Kings will occupy the first two elements of m_pieces for
    // quick lookup
    add_piece(Piece::KING,   Piece::WHITE, "e1");
    add_piece(Piece::KING,   Piece::BLACK, "e8");

    add_piece(Piece::PAWN,   Piece::WHITE, "a2");
    add_piece(Piece::PAWN,   Piece::WHITE, "b2");
    add_piece(Piece::PAWN,   Piece::WHITE, "c2");
    add_piece(Piece::PAWN,   Piece::WHITE, "d2");
    add_piece(Piece::PAWN,   Piece::WHITE, "e2");
    add_piece(Piece::PAWN,   Piece::WHITE, "f2");
    add_piece(Piece::PAWN,   Piece::WHITE, "g2");
    add_piece(Piece::PAWN,   Piece::WHITE, "h2");
    add_piece(Piece::ROOK,   Piece::WHITE, "a1");
    add_piece(Piece::KNIGHT, Piece::WHITE, "b1");
    add_piece(Piece::BISHOP, Piece::WHITE, "c1");
    add_piece(Piece::QUEEN,  Piece::WHITE, "d1");
    add_piece(Piece::BISHOP, Piece::WHITE, "f1");
    add_piece(Piece::KNIGHT, Piece::WHITE, "g1");
    add_piece(Piece::ROOK,   Piece::WHITE, "h1");

    add_piece(Piece::PAWN,   Piece::BLACK, "a7");
    add_piece(Piece::PAWN,   Piece::BLACK, "b7");
    add_piece(Piece::PAWN,   Piece::BLACK, "c7");
    add_piece(Piece::PAWN,   Piece::BLACK, "d7");
    add_piece(Piece::PAWN,   Piece::BLACK, "e7");
    add_piece(Piece::PAWN,   Piece::BLACK, "f7");
    add_piece(Piece::PAWN,   Piece::BLACK, "g7");
    add_piece(Piece::PAWN,   Piece::BLACK, "h7");
    add_piece(Piece::ROOK,   Piece::BLACK, "a8");
    add_piece(Piece::KNIGHT, Piece::BLACK, "b8");
    add_piece(Piece::BISHOP, Piece::BLACK, "c8");
    add_piece(Piece::QUEEN,  Piece::BLACK, "d8");
    add_piece(Piece::BISHOP, Piece::BLACK, "f8");
    add_piece(Piece::KNIGHT, Piece::BLACK, "g8");
    add_piece(Piece::ROOK,   Piece::BLACK, "h8");

    m_colour_to_move = Piece::WHITE;

    populate_squares_properties();
}

void Board::register_captured(std::shared_ptr<Piece> piece)
{

}

bool Board::get_in_check(Piece::Colour col) const
{
    Piece::Colour opp_col = col == Piece::WHITE ? Piece::BLACK : Piece::WHITE;

    // Optimisation based on fact that white king is always m_pieces[0]
    // and black king is always m_pieces[1]
    auto king = m_pieces[col == Piece::WHITE ? 0 : 1];

    return square(king->get_loc()).get_attackers(opp_col).size() > 0;
}

int Board::get_num_available_moves(Piece::Colour col) const
{
    int total_num_moves = 0;
    for(const auto piece : m_pieces)
    {
        if (piece->get_colour() == col)
            total_num_moves += piece->get_all_valid_moves(*this).size();
    }
    return total_num_moves;
}

void Board::set_from_edit_mode(std::vector<std::string> in)
{
    Piece::Colour col = Piece::WHITE;
    std::shared_ptr<Piece> black_king;
    std::shared_ptr<Piece> white_king;

    for(std::vector<std::string>::iterator it = in.begin(); it != in.end(); it++)
    {
    	// see http://www.gnu.org/software/xboard/engine-intf.html
        if (!it->compare("#"))
        {
            delete_all_pieces();
            continue;
        }
        else if (!it->compare("."))
        {
        	break;
        }
        else if (!it->compare("c"))
        {
        	if (col == Piece::WHITE)
        		col = Piece::BLACK;
        	else
        		col = Piece::WHITE;
        	continue;
        }

        // At this point we have a string in the form 'Tcr' where:
        // T = type (P, N, B, R, Q, K),
        // cr = col + row
        std::string sq = it->substr(1);

        switch ((*it)[0])
        {
        case 'P':
        	add_piece(Piece::PAWN, col, sq);
        	break;
        case 'N':
        	add_piece(Piece::KNIGHT, col, sq);
        	break;
        case 'B':
        	add_piece(Piece::BISHOP, col, sq);
        	break;
        case 'R':
        	add_piece(Piece::ROOK, col, sq);
        	break;
        case 'Q':
        	add_piece(Piece::QUEEN, col, sq);
        	break;
        case 'K':
        	add_piece(Piece::KING, col, sq);
        	break;
        default:
        	throw std::runtime_error(std::string("Invalid edit string: ") + *it);
        }
    }
}

