#pragma once

enum class PieceType {
    KING,
    QUEEN,
    ROOK,
    BISHOP,
    KNIGHT,
    PAWN
};

enum class PieceColour {
    WHITE,
    BLACK
};

static constexpr PieceColour opposite_colour(PieceColour col) { return col == PieceColour::WHITE ? PieceColour::BLACK : PieceColour::WHITE; }