#pragma once

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <chrono>

#include "search_tree.h"
#include "bitboards/bitboard.h"

class AI
{
public:
    AI(PieceColour colour) : m_colour(colour) {}
    virtual ~AI() {}

    virtual Move make_move(BitBoard& board, std::chrono::steady_clock::time_point deadline) = 0;

    void set_colour(PieceColour colour) { m_colour = colour; }

protected:
    PieceColour m_colour;
};

class BasicAI : public AI
{
private:
    std::unique_ptr<SearchTree> m_board_tree;

public:
    BasicAI(PieceColour colour) : AI(colour) {}

    Move make_move(BitBoard& board, std::chrono::steady_clock::time_point deadline) override
    {
        if (!m_board_tree)
            m_board_tree = std::make_unique<SearchTree>(board);

        return m_board_tree->search(deadline, m_colour);
    }
};