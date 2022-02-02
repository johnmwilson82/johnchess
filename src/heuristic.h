#pragma once

#include "bitboards/bitboard.h"

class ShannonHeuristic
{
// f(p) = 200(K-K')
//        + 9(Q-Q')
//        + 5(R-R')
//        + 3(B-B' + N-N')
//        + 1(P-P')
//        - 0.5(D-D' + S-S' + I-I')
//        + 0.1(M-M') + ...

// KQRBNP = number of kings, queens, rooks, bishops, knights and pawns
// D,S,I = doubled, blocked and isolated pawns
// M = Mobility (the number of legal moves)
private:
    float accum = 0.0;

public:
    ShannonHeuristic(const BitBoard& board);
    
    float get() const { return accum; };
};

