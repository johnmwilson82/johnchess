#include "zobrist_hash.h"
#include <random>


size_t ZobristHash::piece_to_index(const Piece& piece) const
{
    size_t piece_idx = piece.get_type();

    if(piece_idx == Piece::PAWN && piece.capturable_en_passant())
    {
        ++piece_idx;
    }
    else if(piece_idx == Piece::KING && !piece.has_moved())
    {
        piece_idx = static_cast<size_t>(PieceIndex::WHITE_KING_CAN_CASTLE);
    }
    else if(piece_idx == Piece::ROOK && !piece.has_moved())
    {
        piece_idx = static_cast<size_t>(PieceIndex::WHITE_ROOK_CAN_CASTLE);
    }

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
        for(int j = 0; j < 18; ++j)
        {
            table[i][j] = dis(gen);
        }
    }
}

uint64_t ZobristHash::get_hash(const Board& board) const
{
    uint64_t ret = 0;
    for(int i = 0; i < 64; ++i)
    {
        auto piece = board.square(i & 0x7, i >> 3).get_piece();
        if(piece)
        {
            auto j = piece_to_index(*piece);
            ret ^= table[i][j];
        }
    }

    return ret;
}

uint64_t ZobristHash::get_hash(const Board& board, const Move& move) const
{
    Board new_board(board);

    new_board.move_piece(move);

    return get_hash(new_board);
}
