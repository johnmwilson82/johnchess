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
        std::vector<Move> avail_moves;
        std::vector<Move> king_moves;
        std::vector<Move> dsb_moves;
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

            king_moves = king->get_all_valid_moves(board);
            ofs << "Available_moves for " << king->get_loc().to_string() << "\n";
            for(const auto move : king_moves)
            {
                ofs << move.to_string() << ", ";
            }

            dsb_moves = dsb->get_all_valid_moves(board);
            ofs << "DSB Available_moves for " << dsb->get_loc().to_string() << "\n";
            for(const auto move : dsb_moves)
            {
                ofs << move.to_string() << ", ";
            }

            ofs << std::endl;
            ofs.flush();
            
        } while(avail_moves.size() == 0);

        //now we have a piece that belongs to us, we'll randomly make a move
        int mv_num = rand() % avail_moves.size();

        return avail_moves[mv_num];
    }
};