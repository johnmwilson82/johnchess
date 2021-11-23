#include "heuristic.h"

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


ShannonHeuristic::ShannonHeuristic(const Board& board)
{
    auto piece_fn = [&](const Piece& piece) {
        double mult = piece.get_colour() == Piece::WHITE ? 1 : -1;

        switch(piece.get_type())
        {
            case Piece::PAWN:
                accum += 1 * mult;
                if(piece.get_all_valid_moves(board).size() == 0)
                {
                    accum -= 0.5 * mult;
                }
                // TODO: check isolated pawns and doubled pawns
                break;
            
            case Piece::BISHOP:
            case Piece::KNIGHT:
                accum += 3 * mult;
                break;

            case Piece::ROOK:
                accum += 5 * mult;
                break;

            case Piece::QUEEN:
                accum += 9 * mult;
                break;

            case Piece::KING:
                accum += 200 * mult;
                break;
            
        }
    };

    board.for_all_pieces(piece_fn);    
    
    //accum += board.get_all_legal_moves(Piece::WHITE).size() * 0.1;
    //accum -= board.get_all_legal_moves(Piece::BLACK).size() * 0.1;
}