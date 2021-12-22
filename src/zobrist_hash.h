#include "board.h"
#include "pieces.h"

class ZobristHash
{
private:
    uint64_t table[64][18];

    enum class PieceIndex : size_t
    {
        WHITE_KING,
        WHITE_QUEEN,
        WHITE_ROOK,
        WHITE_BISHOP,
        WHITE_KNIGHT,
        WHITE_PAWN,
        WHITE_PAWN_EN_PASSANT_CAPTURABLE,
        WHITE_KING_CAN_CASTLE,
        WHITE_ROOK_CAN_CASTLE,

        BLACK_KING,
        BLACK_QUEEN,
        BLACK_ROOK,
        BLACK_BISHOP,
        BLACK_KNIGHT,
        BLACK_PAWN,
        BLACK_PAWN_EN_PASSANT_CAPTURABLE,
        BLACK_KING_CAN_CASTLE,
        BLACK_ROOK_CAN_CASTLE,
    };

    size_t piece_to_index(const Piece& piece) const;
    
public:
    ZobristHash();

    uint64_t get_hash(const IBoard& board) const;
    uint64_t get_hash(const IBoard& board, const Move& move) const;
};