#pragma once

#include "board.h"

#include <cstdlib>
#include <iostream>
#include <fstream>

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
    virtual Move make_move(const Board& board) = 0;

    //! Set the colour of the AI side
    void set_colour(Piece::Colour colour) { m_colour = colour; }

protected:
    Piece::Colour m_colour;
};

/*! /brief Random moves AI
 *
 * This AI just picks a random move that it finds which is legal and
 * plays it!
 */
class RandomAI : public AI
{
private:
    std::ofstream ofs;
public:
    RandomAI(Piece::Colour colour) :
        AI(colour),
        ofs("ai.txt")
    {}

    ~RandomAI() {};
    Move make_move(const Board& board)
    {
        std::list<Move> avail_moves;

        static auto dsb = board.square(2, 0).get_piece();
        do{
            //choose random square
            int x = rand() % 8;
            int y = rand() % 8;

            auto king = board.square(4, 7).get_piece();

            auto pc = board.square(x, y).get_piece();
            avail_moves.clear();
            if(!pc)
                continue;
            if(pc->get_colour() != m_colour)
                continue;

            avail_moves = pc->get_all_valid_moves(board);

            bool in_check;
            // Erase moves that put ai into check
            avail_moves.remove_if([&](const Move& move) {
                Board test_board(board, move);
                in_check = test_board.get_in_check(m_colour);
                ofs << "Move " << move.to_string() << (in_check ? " in check" : " not in check") << std::endl;
                return in_check;
            });

            ofs << std::endl;
            ofs.flush();
            
        } while(avail_moves.size() == 0);

        //now we have a piece that belongs to us, we'll randomly make a move
        int mv_num = rand() % avail_moves.size();

        auto it = avail_moves.begin();
        std::advance(it, mv_num);
        return *it;
    }
};
