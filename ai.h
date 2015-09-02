#ifndef AI_H
#define AI_H
#include "board.h"

#include <cstdlib>
#include <iostream>

/*! /brief Base AI class
 *
 * This class defines the functionality for an AI
 */
class AI
{
public:
    AI(Piece::Colour colour) :
        m_colour(colour)
    {}
    virtual ~AI() {}

    //! Called by the application class to request a move
    /*!
     * \param board pointer to the board that the AI will make a move on
     * \return movement
     */
    virtual Move make_move(Board* board) = 0;

    //! Set the colour of the AI side
    void set_colour(Piece::Colour colour) { m_colour = colour; }

protected:
    Piece::Colour m_colour;
};

/*! /brief Random moves AI
 *
 * This AI just picks the first move that it finds which is legal and
 * plays it!
 */
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
