#include <stdexcept>

#include "board.h"

BoardLocation::BoardLocation() :
    m_x(0),
    m_y(0),
    m_on_board(true),
    m_valid(true),
    m_board(NULL)
{
}

BoardLocation::BoardLocation(const BoardLocation& board) :
    m_x(board.m_x),
    m_y(board.m_y),
    m_on_board(board.m_on_board),
    m_valid(board.m_valid),
    m_board(board.m_board)
{
}

BoardLocation::BoardLocation(std::string loc_str, Board* board) :
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

BoardLocation::BoardLocation(int x, int y, Board* board) :
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

bool BoardLocation::apply_move_inplace(int dx, int dy)
{
    // move a location by a number of squares in the x and y directions
    // if the resulting square is not on the board then the location is not
    // updated and false is returned
    if(!m_on_board || !m_valid)
        return false;

    if(!m_board->on_board(m_x+dx, m_y+dy))
        return false;

    m_x += dx;
    m_y += dy;
    return true;
}

Board::Board(int dim_x, int dim_y) :
    m_dim_x(dim_x),
    m_dim_y(dim_y),
    m_squares(dim_x*dim_y)
{
    set_to_start_position();
}

Board::Board(const Board& orig) :
    m_dim_x(orig.m_dim_x),
    m_dim_y(orig.m_dim_y),
    m_squares(orig.m_dim_x*orig.m_dim_y),
    m_colour_to_move(orig.m_colour_to_move)
{
    for(const Piece* piece : orig.m_pieces) //std::vector<Piece*>::iterator it = orig.m_pieces.begin(); it != orig.m_pieces.end(); it++)
    {
        add_piece(piece->get_type(), piece->get_colour(), piece->get_loc());
    }
}

void Board::delete_all_pieces()
{
    for(std::vector<Piece*>::iterator it = m_pieces.begin(); it != m_pieces.end(); ++it)
    {
        square((*it)->get_loc()).remove_piece();
        delete *it;
    }
    m_pieces.clear();
}

bool Board::add_piece(Piece::Type type, Piece::Colour col, std::string loc)
{
    return add_piece(type, col, BoardLocation(loc, this));
}

bool Board::add_piece(Piece::Type type, Piece::Colour col, BoardLocation loc)
{
    if (!square(loc).is_empty())
        return false;

    Piece *new_piece;
    switch (type)
    {
        case Piece::KING:
            new_piece = new King(col, loc);
            break;
        case Piece::QUEEN:
            new_piece = new Queen(col, loc);
            break;
        case Piece::ROOK:
            new_piece = new Rook(col, loc);
            break;
        case Piece::KNIGHT:
            new_piece = new Knight(col, loc);
            break;
        case Piece::BISHOP:
            new_piece = new Bishop(col, loc);
            break;
        case Piece::PAWN:
            new_piece = new Pawn(col, loc);
            break;
    }
    m_pieces.push_back(new_piece);
    square(loc).set_piece(new_piece);
    return true;
}

bool Board::remove_piece(std::string loc)
{
    return remove_piece(BoardLocation(loc, this));
}

bool Board::remove_piece(BoardLocation loc)
{
    // This doesn't fully delete the pieces, just removes them from play
    if(square(loc).is_empty())
        return false;

    Piece* piece = square(loc).get_piece();
    piece->set_on_board(false);
    return true;
}

bool Board::move_piece(std::string move_str, bool check_test)
{
    std::string curr_loc = move_str.substr(0, 2);
    std::string new_loc = move_str.substr(2);

    m_colour_to_move = (m_colour_to_move == Piece::WHITE ? Piece::BLACK : Piece::WHITE);
    return move_piece(BoardLocation(curr_loc, this), BoardLocation(new_loc, this), check_test);
}

bool Board::move_piece(const BoardLocation& curr_loc, const BoardLocation& new_loc, bool check_test)
{
    Piece* captured_piece = square(new_loc).get_piece();
    Piece* moving_piece = square(curr_loc).get_piece();
    if(!moving_piece)
        return false;

    if(!moving_piece->move(*this, new_loc, check_test))
        return false;

    if(captured_piece)
    {
        register_captured(captured_piece);
        remove_piece(new_loc);
    }

    square(new_loc).set_piece(moving_piece);
    square(curr_loc).remove_piece();

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
}

void Board::register_captured(Piece* piece)
{

}

// TODO: Does get_valid_move_check_test have any function here?
bool Board::get_in_check(Piece::Colour col)
{
    Piece::Colour opp_col = col == Piece::WHITE ? Piece::BLACK : Piece::WHITE;

    // Optimisation based on fact that white king is always m_pieces[0]
    // and black king is always m_pieces[1]
    Piece* king = m_pieces[col == Piece::WHITE ? 0 : 1];

    for(std::vector<Piece*>::iterator it = m_pieces.begin()+2; it != m_pieces.end(); it++)
    {
        if ((*it)->get_colour() == opp_col &&
            (*it)->check_valid_move(*this, Move(*it, king->get_loc()), false))
            return true;
    }
    return false;
}

int Board::get_num_available_moves(Piece::Colour col)
{
    int total_num_moves = 0;
    for(const auto piece : m_pieces)
    {
        if (piece->get_colour() == col)
            total_num_moves += piece->get_all_valid_moves(*this, true).size();
    }
    return total_num_moves;
}

void Board::set_from_edit_mode(std::vector<std::string> in)
{
    Piece::Colour col = Piece::WHITE;
    Piece* black_king;
    Piece* white_king;

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
        	if (col == Piece::WHITE)
        	{
        	    white_king = m_pieces.back();
        	    m_pieces.pop_back();
        	}
        	else if (col == Piece::BLACK)
            {
                black_king = m_pieces.back();
                m_pieces.pop_back();
            }
        	break;
        default:
        	throw std::runtime_error(std::string("Invalid edit string: ") + *it);
        }
    }

    // Insert kings so we have [0] -> white king and [1] -> black king
    m_pieces.insert(m_pieces.begin(), black_king);
    m_pieces.insert(m_pieces.begin(), white_king);
}
