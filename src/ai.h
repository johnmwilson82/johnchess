#pragma once

#include <cstdlib>
#include <iostream>
#include <fstream>

#include "search_tree.h"
#include "bitboards/bitboard.h"

/*! /brief Base AI class
 *
 * This class defines the functionality for an AI
 */
class AI
{
public:
    AI(PieceColour colour) :
        m_colour(colour)
    {}
    virtual ~AI() {}

    //! Called by the application class to request a move
    /*!
     * \param board pointer to the board that the AI will make a move on
     * \return movement
     */
    virtual Move make_move(BitBoard& board) = 0;

    //! Set the colour of the AI side
    void set_colour(PieceColour colour) { m_colour = colour; }

protected:
    PieceColour m_colour;
};

class BasicAI : public AI
{
private:
    std::ofstream m_ofs;
    std::unique_ptr<SearchTree> m_board_tree;
    uint8_t m_search_depth;

public:
    BasicAI(PieceColour colour, uint8_t search_depth) :
        AI(colour),
        m_search_depth(search_depth),
        m_ofs("ai.txt")
    {}

    ~BasicAI() {};
    Move make_move(BitBoard& board) override
    {
        if(!m_board_tree)
        {
            m_board_tree = std::make_unique<SearchTree>(board);
        }
        else
        {
            //m_board_tree->set_new_root_from_board(board);
        }

        return m_board_tree->search(m_search_depth, m_colour);
    }
};