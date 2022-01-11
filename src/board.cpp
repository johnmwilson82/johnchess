#include <stdexcept>

#include "board.h"

Board::Board() :
    m_colour_to_move(PieceColour::WHITE),
    m_pieces(2)
{
}

Board::Board(const Board& orig, const Move& move) :
    m_colour_to_move(orig.m_colour_to_move),
    m_en_passant_column(orig.m_en_passant_column),
    m_castling_rights(orig.m_castling_rights),
    m_pieces(2)
{
    for(const auto& piece : orig.m_pieces)
    {
        if(piece->get_on_board())
        {
            add_piece(*piece);
        }
    }

    make_move(move);

    populate_squares_properties();
}


Board::Board(const Board& orig) :
    m_colour_to_move(orig.m_colour_to_move),
    m_en_passant_column(orig.m_en_passant_column),
    m_castling_rights(orig.m_castling_rights),
    m_pieces(2),
    m_squares(orig.m_squares)
{
    for (auto& square : m_squares)
    {
        square.remove_piece();
    }

    for (const auto& piece : orig.m_pieces)
    {
        if (piece->get_on_board())
        {
            add_piece(*piece);
        }
    }
}

Board::Board(const Board& orig, const std::string& move_str) :
    m_colour_to_move(orig.m_colour_to_move),
    m_en_passant_column(orig.m_en_passant_column),
    m_castling_rights(orig.m_castling_rights),
    m_pieces(2)
{
    for(const auto& piece : orig.m_pieces)
    {
        if(piece->get_on_board())
        {
            add_piece(*piece);
        }
    }

    make_move(move_str);

    populate_squares_properties();
}

void Board::populate_squares_properties()
{
    for(const auto& piece : m_pieces)
    {
        if(piece->get_on_board())
        {
            for(const auto& move : piece->get_all_valid_moves())
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
bool Board::add_piece(PieceColour col, std::string loc)
{
    return add_piece<T>(col, BoardLocation(loc, *this));
}

template<typename T>
bool Board::add_piece(PieceColour col, BoardLocation loc)
{
    if (!square(loc).is_empty())
        return false;

    auto new_piece = m_pieces.emplace_back(std::make_shared<T>(*this, col, loc));

    square(loc).set_piece(new_piece);
    return true;
}

template<>
bool Board::add_piece<King>(PieceColour col, BoardLocation loc)
{
    if (!square(loc).is_empty())
        return false;

    auto new_piece = std::make_shared<King>(*this, col, loc);
    m_pieces[col == PieceColour::WHITE ? 0 : 1] = new_piece;

    square(loc).set_piece(new_piece);
    return true;
}

bool Board::add_piece(PieceType type, PieceColour col, BoardLocation loc)
{
    switch (type)
    {
    case PieceType::PAWN:
        return add_piece<Pawn>(col, loc);

    case PieceType::KNIGHT:
        return add_piece<Knight>(col, loc);

    case PieceType::BISHOP:
        return add_piece<Bishop>(col, loc);

    case PieceType::ROOK:
        return add_piece<Rook>(col, loc);

    case PieceType::QUEEN:
        return add_piece<Queen>(col, loc);

    case PieceType::KING:
        return add_piece<King>(col, loc);
    }

    return false;
}

bool Board::add_piece(PieceType type, PieceColour col, std::string loc)
{
    return add_piece(type, col, BoardLocation(loc, *this));
}

bool Board::add_piece(const Piece& piece)
{
    if (!square(piece.get_loc()).is_empty())
        return false;

    std::shared_ptr<Piece> new_piece;
    if(piece.get_type() == PieceType::KING)
    {
        new_piece = piece.clone(*this);
        m_pieces[piece.get_colour() == PieceColour::WHITE ? 0 : 1] = new_piece;
    }
    else
    {
        new_piece = m_pieces.emplace_back(piece.clone(*this));
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
    if(moving_piece.get_type() != PieceType::KING)
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

std::optional<std::pair<std::shared_ptr<Piece>, BoardLocation>> Board::check_unmake_castling_rook_move(const Piece& moving_piece, const BoardLocation& prev_loc)
{
    if (moving_piece.get_type() != PieceType::KING)
    {
        // Not a king move
        return {};
    }

    // White king
    if (prev_loc == BoardLocation("e1", *this))
    {
        if (moving_piece.get_loc() == BoardLocation("g1", *this))
        {
            // King's side castling
            return std::make_pair(square(5, 0).get_piece(), BoardLocation("h1", *this));
        }

        if (moving_piece.get_loc() == BoardLocation("c1", *this))
        {
            // Queens's side castling
            return std::make_pair(square(3, 0).get_piece(), BoardLocation("a1", *this));
        }
    }

    // Black king
    if (prev_loc == BoardLocation("e8", *this))
    {
        if (moving_piece.get_loc() == BoardLocation("g8", *this))
        {
            // King's side castling
            return std::make_pair(square(5, 7).get_piece(), BoardLocation("h8", *this));
        }

        if (moving_piece.get_loc() == BoardLocation("c8", *this))
        {
            // Queens's side castling
            return std::make_pair(square(3, 7).get_piece(), BoardLocation("a8", *this));
        }
    }

    return {};
}

bool Board::make_move(std::string move_str)
{
    return make_move(Move(*this, move_str));
}

uint8_t Board::get_castling_rights_to_remove(const Piece& moving_piece) const
{
    uint8_t castling_rights_to_remove = 0;

    if (moving_piece.get_type() == PieceType::KING)
    {
        if (moving_piece.get_colour() == PieceColour::WHITE)
        {
            castling_rights_to_remove |= static_cast<uint8_t>(CastlingRights::WHITE_KINGSIDE);
            castling_rights_to_remove |= static_cast<uint8_t>(CastlingRights::WHITE_QUEENSIDE);
        }
        else
        {
            castling_rights_to_remove |= static_cast<uint8_t>(CastlingRights::BLACK_KINGSIDE);
            castling_rights_to_remove |= static_cast<uint8_t>(CastlingRights::BLACK_QUEENSIDE);
        }
    }

    if (moving_piece.get_type() == PieceType::ROOK)
    {
        const auto& curr_loc = moving_piece.get_loc();

        if (curr_loc.get_x() == 0)
        {
            if (moving_piece.get_colour() == PieceColour::WHITE)
            {
                castling_rights_to_remove |= static_cast<uint8_t>(CastlingRights::WHITE_QUEENSIDE);
            }
            else
            {
                castling_rights_to_remove |= static_cast<uint8_t>(CastlingRights::BLACK_QUEENSIDE);
            }
        }
        else if (curr_loc.get_x() == 7)
        {
            if (moving_piece.get_colour() == PieceColour::WHITE)
            {
                castling_rights_to_remove |= static_cast<uint8_t>(CastlingRights::WHITE_KINGSIDE);
            }
            else
            {
                castling_rights_to_remove |= static_cast<uint8_t>(CastlingRights::BLACK_KINGSIDE);
            }
        }
    }

    return castling_rights_to_remove;
}

void Board::process_castling_rook_move(std::optional<std::pair<std::shared_ptr<Piece>, BoardLocation>> castling_rook_move)
{
    // Implement castling
    if (castling_rook_move.has_value())
    {
        // Move is a castle move
        auto rook = castling_rook_move->first;
        auto rook_new_loc = castling_rook_move->second;
        auto rook_curr_loc = rook->get_loc();

        rook->move(rook_new_loc);
        square(rook_new_loc).set_piece(rook);
        square(rook_curr_loc).remove_piece();
    }
}

bool Board::make_move(const Move& move)
{
    const auto& new_loc = move.get_to_loc();
    const auto& curr_loc = move.get_from_loc();

    auto captured_piece = square(new_loc).get_piece();
    auto moving_piece = square(curr_loc).get_piece();

    if(!moving_piece)
        return false;

    // en passant rules
    if((!captured_piece && moving_piece->get_type() == PieceType::PAWN) &&
        (curr_loc.get_x() != new_loc.get_x()))
    {
        captured_piece = square(
                new_loc.apply_move(DynMove(0, moving_piece->get_colour() == PieceColour::WHITE ? -1 : 1))
            ).get_piece();
    }

    if((moving_piece->get_type() == PieceType::PAWN) &&
        ((curr_loc.get_y() == 1 && new_loc.get_y() == 3) ||
         (curr_loc.get_y() == 6 && new_loc.get_y() == 4)))
    {
        m_en_passant_column = curr_loc.get_x();
    }
    else
    {
        m_en_passant_column = std::nullopt;
    }

    // Check castling
    auto castling_rook_move = check_castling_rook_move(*moving_piece, new_loc);

    uint8_t castling_rights_to_remove = get_castling_rights_to_remove(*moving_piece);
    m_castling_rights &= ~castling_rights_to_remove;

    if(captured_piece)
    {
        captured_piece->set_on_board(false);
        square(new_loc).remove_piece();
    }

    moving_piece->move(new_loc);
    square(new_loc).set_piece(moving_piece);

    // Check promotion
    if(move.get_promotion_type().has_value())
    {
        switch(*move.get_promotion_type())
        {
            case Move::PromotionType::QUEEN:
                moving_piece = std::make_shared<Queen>(*this, moving_piece->get_colour(), new_loc);
                break;

            case Move::PromotionType::ROOK:
                moving_piece = std::make_shared<Rook>(*this, moving_piece->get_colour(), new_loc);
                break;

            case Move::PromotionType::BISHOP:
                moving_piece = std::make_shared<Bishop>(*this, moving_piece->get_colour(), new_loc);
                break;

            case Move::PromotionType::KNIGHT:
                moving_piece = std::make_shared<Knight>(*this, moving_piece->get_colour(), new_loc);
                break;
        }

        remove_piece(moving_piece->get_loc());
    }
    
    square(curr_loc).remove_piece();

    // Implement castling
    process_castling_rook_move(castling_rook_move);

    m_colour_to_move = opposite_colour(m_colour_to_move);

    return true;
}

bool Board::unmake_move(const Move& move)
{
    const auto& curr_loc = move.get_to_loc();
    const auto& new_loc = move.get_from_loc();

    auto moving_piece = square(curr_loc).get_piece();

    if (!moving_piece)
        return false;

    // en passant rules
    if (move.is_en_passant_capture())
    {
        //auto ep_capture_sq = curr_loc.apply_move(DynMove(0, moving_piece->get_colour() == PieceColour::WHITE ? -1 : 1));
        //add_piece<Pawn>(opposite_colour(moving_piece->get_colour()), ep_capture_sq);
        m_en_passant_column = curr_loc.get_x();
    }
    else
    {
        m_en_passant_column = std::nullopt;
    }

    
    // Check castling
    auto castling_rook_move = check_unmake_castling_rook_move(*moving_piece, new_loc);

    m_castling_rights = move.get_old_castling_rights();

    // Move piece back
    moving_piece->move(new_loc);
    square(new_loc).set_piece(moving_piece);

    if (move.get_promotion_type() != std::nullopt)
    {
        moving_piece = std::make_shared<Pawn>(*this, moving_piece->get_colour(), new_loc);
    }

    square(curr_loc).remove_piece();

    // Replace captured piece
    auto captured_piece = move.get_captured_piece();
    // For en passant captures we've already replaced the captured piece
    if (captured_piece && !move.is_en_passant_capture())
    {
        captured_piece->set_on_board(true);
        square(captured_piece->get_loc()).set_piece(captured_piece);
    }

    // Implement castling
    process_castling_rook_move(castling_rook_move);

    m_colour_to_move = opposite_colour(m_colour_to_move);

    return true;
}

void Board::set_to_start_position()
{
    delete_all_pieces();
    // Kings will occupy the first two elements of m_pieces for
    // quick lookup
    add_piece<King>(PieceColour::WHITE, "e1");
    add_piece<King>(PieceColour::BLACK, "e8");

    add_piece<Pawn>(PieceColour::WHITE, "a2");
    add_piece<Pawn>(PieceColour::WHITE, "b2");
    add_piece<Pawn>(PieceColour::WHITE, "c2");
    add_piece<Pawn>(PieceColour::WHITE, "d2");
    add_piece<Pawn>(PieceColour::WHITE, "e2");
    add_piece<Pawn>(PieceColour::WHITE, "f2");
    add_piece<Pawn>(PieceColour::WHITE, "g2");
    add_piece<Pawn>(PieceColour::WHITE, "h2");
    add_piece<Rook>(PieceColour::WHITE, "a1");
    add_piece<Knight>(PieceColour::WHITE, "b1");
    add_piece<Bishop>(PieceColour::WHITE, "c1");
    add_piece<Queen>(PieceColour::WHITE, "d1");
    add_piece<Bishop>(PieceColour::WHITE, "f1");
    add_piece<Knight>(PieceColour::WHITE, "g1");
    add_piece<Rook>(PieceColour::WHITE, "h1");

    add_piece<Pawn>(PieceColour::BLACK, "a7");
    add_piece<Pawn>(PieceColour::BLACK, "b7");
    add_piece<Pawn>(PieceColour::BLACK, "c7");
    add_piece<Pawn>(PieceColour::BLACK, "d7");
    add_piece<Pawn>(PieceColour::BLACK, "e7");
    add_piece<Pawn>(PieceColour::BLACK, "f7");
    add_piece<Pawn>(PieceColour::BLACK, "g7");
    add_piece<Pawn>(PieceColour::BLACK, "h7");
    add_piece<Rook>(PieceColour::BLACK, "a8");
    add_piece<Knight>(PieceColour::BLACK, "b8");
    add_piece<Bishop>(PieceColour::BLACK, "c8");
    add_piece<Queen>(PieceColour::BLACK, "d8");
    add_piece<Bishop>(PieceColour::BLACK, "f8");
    add_piece<Knight>(PieceColour::BLACK, "g8");
    add_piece<Rook>(PieceColour::BLACK, "h8");

    m_colour_to_move = PieceColour::WHITE;
    m_castling_rights = INIT_CASTLING_RIGHTS;

    populate_squares_properties();
}

bool Board::get_in_check(PieceColour col) const
{
    PieceColour opp_col = col == PieceColour::WHITE ? PieceColour::BLACK : PieceColour::WHITE;

    // Optimisation based on fact that white king is always m_pieces[0]
    // and black king is always m_pieces[1]
    auto king = m_pieces[col == PieceColour::WHITE ? 0 : 1];

    return square(king->get_loc()).get_attackers(opp_col) > 0;
}

std::list<Move> Board::get_all_legal_moves(PieceColour col) const
{
    std::list<Move> ret;

    for(const auto piece : m_pieces)
    {
        if (piece->get_colour() == col && piece->get_on_board())
        {
            auto moves = piece->get_all_valid_moves();
            ret.splice(ret.end(), moves);
        }
    }

    ret.remove_if([&](const Move& move){

        Board test_board(*this, move);
        return test_board.get_in_check(col);
    });

    for (auto& move : ret)
    {
        move.set_old_castling_rights(m_castling_rights);
    }

    return ret;
}

void Board::set_from_edit_mode(std::vector<std::string> in)
{
    PieceColour col = PieceColour::WHITE;
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
        	if (col == PieceColour::WHITE)
        		col = PieceColour::BLACK;
        	else
        		col = PieceColour::WHITE;
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

std::shared_ptr<const Piece> Board::piece_on_square(int x, int y) const
{
    return square(x, y).get_piece();
}

std::unique_ptr<IBoard> Board::clone() const
{
    return std::make_unique<Board>(*this);
}

std::unique_ptr<IBoard> Board::clone_moved(const Move& move) const
{
    return std::make_unique<Board>(*this, move);
}
