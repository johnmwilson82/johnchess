#include "zobrist_hash.h"
#include <bitboards/bitboard_utils.h>
#include <random>


size_t ZobristHash::piece_to_index(PieceType type, PieceColour colour) const
{
    size_t piece_idx = static_cast<size_t>(type);

    if(colour == PieceColour::BLACK)
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

void ZobristHash::add_piece_mask_to_hash(PieceType type, PieceColour colour, uint64_t mask, uint64_t& hash) const
{
    while (mask)
    {
        auto i = bitboard_utils::bit_scan_forward(mask);
        auto j = piece_to_index(type, colour);
        hash ^= piece_table[i][j];

        mask &= mask - 1;
    }
}

uint64_t ZobristHash::get_hash(const BitBoard& board) const
{
    uint64_t ret = 0;

    add_piece_mask_to_hash(PieceType::KING, PieceColour::WHITE, board.get_kings() & board.pieces_to_move(1), ret);
    add_piece_mask_to_hash(PieceType::KING, PieceColour::BLACK, board.get_kings() & board.pieces_to_move(0), ret);
    add_piece_mask_to_hash(PieceType::QUEEN, PieceColour::WHITE, board.get_queens() & board.pieces_to_move(1), ret);
    add_piece_mask_to_hash(PieceType::QUEEN, PieceColour::BLACK, board.get_queens() & board.pieces_to_move(0), ret);
    add_piece_mask_to_hash(PieceType::ROOK, PieceColour::WHITE, board.get_rooks() & board.pieces_to_move(1), ret);
    add_piece_mask_to_hash(PieceType::ROOK, PieceColour::BLACK, board.get_rooks() & board.pieces_to_move(0), ret);
    add_piece_mask_to_hash(PieceType::BISHOP, PieceColour::WHITE, board.get_bishops() & board.pieces_to_move(1), ret);
    add_piece_mask_to_hash(PieceType::BISHOP, PieceColour::BLACK, board.get_bishops() & board.pieces_to_move(0), ret);
    add_piece_mask_to_hash(PieceType::KNIGHT, PieceColour::WHITE, board.get_knights() & board.pieces_to_move(1), ret);
    add_piece_mask_to_hash(PieceType::KNIGHT, PieceColour::BLACK, board.get_knights() & board.pieces_to_move(0), ret);
    add_piece_mask_to_hash(PieceType::PAWN, PieceColour::WHITE, board.get_pawns() & board.pieces_to_move(1), ret);
    add_piece_mask_to_hash(PieceType::PAWN, PieceColour::BLACK, board.get_pawns() & board.pieces_to_move(0), ret);

    if (board.get_colour_to_move() == PieceColour::BLACK)
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

uint64_t ZobristHash::get_hash(const BitBoard& board, const Move& move) const
{
    BitBoard new_board(board);

    new_board.make_move(move);

    return get_hash(new_board);
}
