#include <stdexcept>

#include "board.h"


Board::Board() :
    m_pieces(2)
{
}

Board::Board(const Board& orig, const Move& move) :
    m_colour_to_move(orig.m_colour_to_move),
    m_pieces(2)
{
    for(const auto& piece : orig.m_pieces)
    {
        if(piece->get_on_board())
        {
            add_piece(*piece);
        }
    }

    move_piece(move);

    populate_squares_properties();
}

Board::Board(const Board& orig, const std::string& move_str) :
    m_colour_to_move(orig.m_colour_to_move),
    m_pieces(2)
{
    for(const auto& piece : orig.m_pieces)
    {
        if(piece->get_on_board())
        {
            add_piece(*piece);
        }
    }

    move_piece(move_str);

    populate_squares_properties();
}

void Board::populate_squares_properties()
{
    for(const auto& piece : m_pieces)
    {
        if(piece->get_on_board())
        {
            for(const auto& move : piece->get_all_valid_moves(*this))
            {
                square(move.get_to_loc()).add_attacker(piece);
            }
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

template<typename T>
bool Board::add_piece(Piece::Colour col, std::string loc)
{
    return add_piece<T>(col, BoardLocation(loc, *this));
}

template<typename T>
bool Board::add_piece(Piece::Colour col, BoardLocation loc)
{
    if (!square(loc).is_empty())
        return false;

    auto new_piece = m_pieces.emplace_back(std::make_shared<T>(col, loc));

    square(loc).set_piece(new_piece);
    return true;
}


template<>
bool Board::add_piece<King>(Piece::Colour col, BoardLocation loc)
{
    if (!square(loc).is_empty())
        return false;

    auto new_piece = std::make_shared<King>(col, loc);
    m_pieces[col == Piece::WHITE ? 0 : 1] = new_piece;

    square(loc).set_piece(new_piece);
    return true;
}

bool Board::add_piece(const Piece& piece)
{
    if (!square(piece.get_loc()).is_empty())
        return false;

    std::shared_ptr<Piece> new_piece;
    if(piece.get_type() == Piece::KING)
    {
        new_piece = piece.clone();
        m_pieces[piece.get_colour() == Piece::WHITE ? 0 : 1] = new_piece;
    }
    else
    {
        new_piece = m_pieces.emplace_back(piece.clone());
    }

    square(piece.get_loc()).set_piece(new_piece);
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
    return move_piece(Move(*this, move_str));
}

bool Board::move_piece(const Move& move)
{
    const auto& new_loc = move.get_to_loc();
    const auto& curr_loc = move.get_from_loc();

    auto captured_piece = square(new_loc).get_piece();
    auto moving_piece = square(curr_loc).get_piece();

    if(!moving_piece)
        return false;

    // en passant rules
    if((!captured_piece && moving_piece->get_type() == Piece::PAWN) &&
        (curr_loc.get_x() != new_loc.get_x()))
    {
        captured_piece = square(
                new_loc.apply_move(DynMove(0, moving_piece->get_colour() == Piece::WHITE ? -1 : 1))
            ).get_piece();
    }

    if((moving_piece->get_type() == Piece::PAWN) &&
        ((curr_loc.get_y() == 1 && new_loc.get_y() == 3) ||
         (curr_loc.get_y() == 6 && new_loc.get_y() == 4)))
    {
        moving_piece->set_capturable_en_passant(true);
    }

    // Check castling
    auto castling_rook_move = check_castling_rook_move(*moving_piece, new_loc);

    if(captured_piece)
    {
        remove_piece(captured_piece->get_loc());
        square(new_loc).remove_piece();
    }

    if(move.get_promotion_type().has_value())
    {
        switch(*move.get_promotion_type())
        {
            case Move::PromotionType::QUEEN:
                add_piece<Queen>(moving_piece->get_colour(), new_loc);
                break;

            case Move::PromotionType::ROOK:
                add_piece<Rook>(moving_piece->get_colour(), new_loc);
                break;

            case Move::PromotionType::BISHOP:
                add_piece<Bishop>(moving_piece->get_colour(), new_loc);
                break;

            case Move::PromotionType::KNIGHT:
                add_piece<Knight>(moving_piece->get_colour(), new_loc);
                break;
        }

        remove_piece(moving_piece->get_loc());
    }
    else
    {
        moving_piece->move(new_loc);
        square(new_loc).set_piece(moving_piece);
    }
    
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
    add_piece<King>(Piece::WHITE, "e1");
    add_piece<King>(Piece::BLACK, "e8");

    add_piece<Pawn>(Piece::WHITE, "a2");
    add_piece<Pawn>(Piece::WHITE, "b2");
    add_piece<Pawn>(Piece::WHITE, "c2");
    add_piece<Pawn>(Piece::WHITE, "d2");
    add_piece<Pawn>(Piece::WHITE, "e2");
    add_piece<Pawn>(Piece::WHITE, "f2");
    add_piece<Pawn>(Piece::WHITE, "g2");
    add_piece<Pawn>(Piece::WHITE, "h2");
    add_piece<Rook>(Piece::WHITE, "a1");
    add_piece<Knight>(Piece::WHITE, "b1");
    add_piece<Bishop>(Piece::WHITE, "c1");
    add_piece<Queen>(Piece::WHITE, "d1");
    add_piece<Bishop>(Piece::WHITE, "f1");
    add_piece<Knight>(Piece::WHITE, "g1");
    add_piece<Rook>(Piece::WHITE, "h1");

    add_piece<Pawn>(Piece::BLACK, "a7");
    add_piece<Pawn>(Piece::BLACK, "b7");
    add_piece<Pawn>(Piece::BLACK, "c7");
    add_piece<Pawn>(Piece::BLACK, "d7");
    add_piece<Pawn>(Piece::BLACK, "e7");
    add_piece<Pawn>(Piece::BLACK, "f7");
    add_piece<Pawn>(Piece::BLACK, "g7");
    add_piece<Pawn>(Piece::BLACK, "h7");
    add_piece<Rook>(Piece::BLACK, "a8");
    add_piece<Knight>(Piece::BLACK, "b8");
    add_piece<Bishop>(Piece::BLACK, "c8");
    add_piece<Queen>(Piece::BLACK, "d8");
    add_piece<Bishop>(Piece::BLACK, "f8");
    add_piece<Knight>(Piece::BLACK, "g8");
    add_piece<Rook>(Piece::BLACK, "h8");

    m_colour_to_move = Piece::WHITE;

    populate_squares_properties();
}

bool Board::get_in_check(Piece::Colour col) const
{
    Piece::Colour opp_col = col == Piece::WHITE ? Piece::BLACK : Piece::WHITE;

    // Optimisation based on fact that white king is always m_pieces[0]
    // and black king is always m_pieces[1]
    auto king = m_pieces[col == Piece::WHITE ? 0 : 1];

    return square(king->get_loc()).get_attackers(opp_col).size() > 0;
}

std::list<Move> Board::get_all_legal_moves(Piece::Colour col) const
{
    std::list<Move> ret;

    for(const auto piece : m_pieces)
    {
        if (piece->get_colour() == col && piece->get_on_board())
        {
            auto moves = piece->get_all_valid_moves(*this);
            ret.splice(ret.end(), moves);
        }
    }

    ret.remove_if([&](const Move& move){

        Board test_board(*this, move);
        return test_board.get_in_check(col);
    });

    return ret;
}

std::list<Move> Board::get_all_legal_moves() const
{
    return get_all_legal_moves(m_colour_to_move);
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
        	add_piece<Pawn>(col, sq);
        	break;
        case 'N':
        	add_piece<Knight>(col, sq);
        	break;
        case 'B':
        	add_piece<Bishop>(col, sq);
        	break;
        case 'R':
        	add_piece<Rook>(col, sq);
        	break;
        case 'Q':
        	add_piece<Queen>(col, sq);
        	break;
        case 'K':
        	add_piece<King>(col, sq);
        	break;
        default:
        	throw std::runtime_error(std::string("Invalid edit string: ") + *it);
        }
    }
}

