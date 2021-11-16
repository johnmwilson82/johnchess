#pragma once
#include <unordered_map>

#include "move.h"
#include "board.h"
#include "zobrist_hash.h"
#include "heuristic.h"


class BoardTreeNode
{
public:
    using hash_map_t = std::unordered_map<uint64_t, std::shared_ptr<BoardTreeNode>>;

private:    
    const Board m_board;    
    const uint64_t m_hash;

    const ShannonHeuristic m_heuristic;

    std::list<std::shared_ptr<BoardTreeNode>> m_child_nodes; 
    std::optional<uint64_t> m_prev_hash;

public:
    std::shared_ptr<BoardTreeNode> rebase(const Move& move, hash_map_t& hash_map, const ZobristHash& hasher);

    std::shared_ptr<BoardTreeNode> rebase(uint64_t new_root_hash, hash_map_t& hash_map);

    void delete_node(hash_map_t& hash_map);

    std::shared_ptr<BoardTreeNode> child_node(const Move& move, hash_map_t& hash_map, const ZobristHash& hasher);

    const Board& get_board() const;

    uint64_t get_hash() const;

    const ShannonHeuristic& get_heuristic() const;

    BoardTreeNode(const Board& board, const ZobristHash& hasher);
    BoardTreeNode(const BoardTreeNode& node, const Move& move, const ZobristHash& hasher);
};

class BoardTree
{
private:
    std::unique_ptr<ZobristHash> hasher;
    BoardTreeNode::hash_map_t nodes; 

    std::shared_ptr<BoardTreeNode> root_node;

    double best_heuristic;

public:
    void set_new_root_from_move(const Move& move)
    {
        auto old_root_node = root_node;
        root_node = root_node->rebase(move, nodes, *hasher);
        nodes.erase(old_root_node->get_hash());
        old_root_node.reset();
    }

    void set_new_root_from_board(const Board& board)
    {
        uint64_t hash = hasher->get_hash(board);
        auto old_root_node = root_node;
        root_node = root_node->rebase(hash, nodes);
        nodes.erase(old_root_node->get_hash());
        old_root_node.reset();
    }

    void search(uint8_t search_depth, double& best_heuristic, std::shared_ptr<BoardTreeNode> btn, Piece::Colour ai_colour)
    {
        if(search_depth > 0)
        {
            auto available_moves = btn->get_board().get_all_legal_moves();

            for(const auto& move : available_moves)
            {
                auto child = btn->child_node(move, nodes, *hasher);

                double prev_best_heuristic = best_heuristic;

                search(search_depth-1,
                       best_heuristic,
                       child, 
                       ai_colour);

                if (best_heuristic != prev_best_heuristic)
                {
                    //std::cout << "best child move = " << move.to_string() << ", heuristic = " << best_heuristic << std::endl;
                }
            }
        }
        else
        {
            // +ve heuristics benefit white
            // We need to find the heuristic that least benefits white
            double heuristic = Piece::WHITE == ai_colour ?
                btn->get_heuristic().get() : -btn->get_heuristic().get();

            if (btn->get_board().get_colour_to_move() == ai_colour)
            {
                if (heuristic > best_heuristic)
                {
                    best_heuristic = heuristic;
                }
            }
            else
            {
                if (heuristic < best_heuristic)
                {
                    best_heuristic = heuristic;
                }
            }
        }
    }

    Move search_root(uint8_t search_depth, Piece::Colour ai_colour)
    {
        auto available_moves = root_node->get_board().get_all_legal_moves();

        double best_heuristic = -9999;
        std::unique_ptr<Move> best_move;

        for(const auto& move : available_moves)
        {
            auto child = root_node->child_node(move, nodes, *hasher);
            double heuristic = 0;
            search(search_depth, heuristic, child, ai_colour);
            if (heuristic > best_heuristic)
            {
                best_move = std::make_unique<Move>(move);
                best_heuristic = heuristic;
                std::cout << "best move = " << best_move->to_string() << ", heuristic = " << best_heuristic << std::endl;
            }
        }

        return *best_move;
    }

    BoardTree(const Board& board) :
        hasher(std::make_unique<ZobristHash>()),
        root_node(std::make_shared<BoardTreeNode>(board, *hasher))
    {}
};