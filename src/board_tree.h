#pragma once
#include <unordered_map>

#include "move.h"
#include "board.h"

class BoardTreeNode
{
private:
    Board m_board;
    std::unordered_map<Move, std::unique_ptr<BoardTreeNode>, MoveHashFunction> m_board_map;

public:
    bool has_move(const Move& move) const
    {
        return m_board_map.contains(move);
    }

    const Board& get_board() const
    {
        return m_board;
    }

    void add_move(const Move& move)
    {
        m_board_map.emplace(move, std::make_unique<BoardTreeNode>(*this, move));
    }

    BoardTreeNode(const Board& board) :
        m_board(board)
    {}

    BoardTreeNode(const BoardTreeNode& node, const Move& move) :
        m_board(node.m_board, move)
    {}
};


class BoardTree
{
private:
    

public:

};