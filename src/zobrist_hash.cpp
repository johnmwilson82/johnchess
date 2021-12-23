#include "zobrist_hash.h"
#include <random>


size_t ZobristHash::piece_to_index(const Piece& piece) const
{
    size_t piece_idx = piece.get_type();

    if(piece.get_colour() == Piece::BLACK)
    {
        piece_idx += static_cast<size_t>(PieceIndex::BLACK_KING);
    }

    return piece_idx;
}


ZobristHash::ZobristHash()
{
    // Initialise the table with random bitstrings
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    for(int i = 0; i < 64; ++i)
    {
        for(int j = 0; j < static_cast<int>(PieceIndex::SIZE); ++j)
        {
            piece_table[i][j] = dis(gen);
        }
    }

    for (int i = 0; i < static_cast<int>(BoardPropsIndex::SIZE); ++i)
    {
        props_table[i] = dis(gen);
    }
}

uint64_t ZobristHash::get_hash(const IBoard& board) const
{
    uint64_t ret = 0;
    for(int i = 0; i < 64; ++i)
    {
        auto piece = board.piece_on_square(i & 0x7, i >> 3);
        if(piece)
        {
            auto j = piece_to_index(*piece);
            ret ^= piece_table[i][j];
        }
    }

    if (board.get_colour_to_move() == Piece::BLACK)
    {
        ret ^= props_table[static_cast<size_t>(BoardPropsIndex::BLACK_TO_MOVE)];
    }

    if (board.has_castling_rights(IBoard::CastlingRights::BLACK_KINGSIDE))
    {
        ret ^= props_table[static_cast<size_t>(BoardPropsIndex::BLACK_CAN_CASTLE_KINGSIDE)];
    }

    if (board.has_castling_rights(IBoard::CastlingRights::BLACK_QUEENSIDE))
    {
        ret ^= props_table[static_cast<size_t>(BoardPropsIndex::BLACK_CAN_CASTLE_QUEENSIDE)];
    }

    if (board.has_castling_rights(IBoard::CastlingRights::WHITE_KINGSIDE))
    {
        ret ^= props_table[static_cast<size_t>(BoardPropsIndex::WHITE_CAN_CASTLE_KINGSIDE)];
    }

    if (board.has_castling_rights(IBoard::CastlingRights::WHITE_QUEENSIDE))
    {
        ret ^= props_table[static_cast<size_t>(BoardPropsIndex::WHITE_CAN_CASTLE_QUEENSIDE)];
    }

    const auto& ep_col = board.get_enpassant_column();

    if (ep_col.has_value())
    {
        ret ^= props_table[static_cast<size_t>(BoardPropsIndex::EN_PASSANT_POSSIBLE_FILE_A) + *ep_col];
    }

    return ret;
}

uint64_t ZobristHash::get_hash(const IBoard& board, const Move& move) const
{
    std::unique_ptr<IBoard> new_board = board.clone();

    new_board->move_piece(move);

    return get_hash(*new_board);
}
