#ifndef AI_H
#define AI_H
#include "board.h"

#include <cstdlib>
#include <iostream>

class AI
{
public:
    AI(Piece::Colour colour) :
        m_colour(colour)
    {}
    virtual ~AI() {}

    virtual Move make_move(Board* board) = 0;

    void set_colour(Piece::Colour colour) { m_colour = colour; }

protected:
    Piece::Colour m_colour;
};

class RandomAI : public AI
{
public:
    RandomAI(Piece::Colour colour) :
        AI(colour)
    {}

    ~RandomAI() {};
    Move make_move(Board* board)
    {
        Piece *pc;
        std::vector<Move> avail_moves;
        do{
            //choose random square
            int x = rand() % 8;
            int y = rand() % 8;
            pc = board->square(x, y).get_piece();
            avail_moves.clear();
            if(!pc)
                continue;
            if(pc->get_colour() != m_colour)
                continue;
            avail_moves = pc->get_all_valid_moves(*board);
        } while(avail_moves.size() == 0);

        //now we have a piece that belongs to us, we'll randomly make a move
        int mv_num = rand() % avail_moves.size();

        return avail_moves[mv_num];
    }
};

#endif
