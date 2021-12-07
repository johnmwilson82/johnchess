#pragma once

#include <cstdlib>
#include <iostream>
#include <fstream>

#include "board_tree.h"
#include "board.h"

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
    Move make_move(const Board& board) override
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

class BasicAI : public AI
{
private:
    std::ofstream m_ofs;
    std::unique_ptr<BoardTree> m_board_tree;
    uint8_t m_search_depth;

public:
    BasicAI(Piece::Colour colour, uint8_t search_depth) :
        AI(colour),
        m_search_depth(search_depth),
        m_ofs("ai.txt")
    {}

    ~BasicAI() {};
    Move make_move(const Board& board) override
    {
        if(!m_board_tree)
        {
            m_board_tree = std::make_unique<BoardTree>(board);
        }
        else
        {
            m_board_tree->set_new_root_from_board(board);
        }

        return m_board_tree->search(m_search_depth, m_colour);
    }
};