#include "bitboards/bitboard.h"
//#include "pieces.h"

class ZobristHash
{
private:

    enum class PieceIndex : size_t
    {
        WHITE_KING,
        WHITE_QUEEN,
        WHITE_ROOK,
        WHITE_BISHOP,
        WHITE_KNIGHT,
        WHITE_PAWN,

        BLACK_KING,
        BLACK_QUEEN,
        BLACK_ROOK,
        BLACK_BISHOP,
        BLACK_KNIGHT,
        BLACK_PAWN,

        SIZE
    };

    uint64_t piece_table[64][static_cast<size_t>(PieceIndex::SIZE)];

    enum class BoardPropsIndex : size_t
    {
        BLACK_TO_MOVE,

        WHITE_CAN_CASTLE_KINGSIDE,
        WHITE_CAN_CASTLE_QUEENSIDE,
        BLACK_CAN_CASTLE_KINGSIDE,
        BLACK_CAN_CASTLE_QUEENSIDE,

        EN_PASSANT_POSSIBLE_FILE_A,
        EN_PASSANT_POSSIBLE_FILE_B,
        EN_PASSANT_POSSIBLE_FILE_C,
        EN_PASSANT_POSSIBLE_FILE_D,
        EN_PASSANT_POSSIBLE_FILE_E,
        EN_PASSANT_POSSIBLE_FILE_F,
        EN_PASSANT_POSSIBLE_FILE_G,
        EN_PASSANT_POSSIBLE_FILE_H,

        SIZE
    };

    uint64_t props_table[static_cast<size_t>(BoardPropsIndex::SIZE)];

    size_t piece_to_index(PieceType type, PieceColour colour) const;
    void add_piece_mask_to_hash(PieceType type, PieceColour colour, uint64_t mask, uint64_t& hash) const;

public:
    ZobristHash();

    uint64_t get_hash(const BitBoard& board) const;
    uint64_t get_hash(const BitBoard& board, const Move& move) const;
};