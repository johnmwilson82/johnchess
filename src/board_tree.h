#pragma once
#include <unordered_map>
#include <unordered_set>

#include "move.h"
#include "board.h"
#include "zobrist_hash.h"
#include "heuristic.h"

#include "utils/board_strings.h"

class BoardTreeNode
{
public:
    using hash_map_t = std::unordered_map<uint64_t, std::weak_ptr<BoardTreeNode>>;
    using edge_t = std::pair<std::shared_ptr<BoardTreeNode>, Move>;

private:    
    const Board m_board;    
    const uint64_t m_hash;

    const ShannonHeuristic m_heuristic;

    std::list<edge_t> m_child_nodes; 
    bool m_check_for_mate = false;

    std::unordered_set<uint64_t> m_ancestors;

    const uint32_t m_ply;

public:
    std::shared_ptr<BoardTreeNode> child_node(const Move& move, hash_map_t& hash_map, const ZobristHash& hasher);

    std::list<edge_t>& get_children() { return m_child_nodes; };

    const Board& get_board() const;

    uint64_t get_hash() const;

    uint32_t get_ply() const { return m_ply; }

    const double get_score(Piece::Colour ai_col, Piece::Colour colour_to_move) const;

    BoardTreeNode(const Board& board, const ZobristHash& hasher);
    BoardTreeNode(const BoardTreeNode& node, const Move& move, const ZobristHash& hasher);

    void set_check_for_mate() { m_check_for_mate = true; }
};

class BoardTree
{
private:
    std::unique_ptr<ZobristHash> hasher;
    BoardTreeNode::hash_map_t nodes; 

    std::shared_ptr<BoardTreeNode> root_node;

    bool prune_except_hash(uint64_t base_hash, uint64_t except_hash)
    {
        if (base_hash == except_hash)
        {
            return false;
        }

        if (nodes.contains(base_hash))
        {
            {
                auto& children = nodes.at(base_hash).lock()->get_children();

                while (!children.empty())
                {
                    auto child = children.front();

                    children.pop_front();

                    prune_except_hash(child.first->get_hash(), except_hash);
                }
            }

            if (nodes.at(base_hash).use_count() <= 1)
            {
                nodes.erase(base_hash);
            }
        }

        return true;
    }

    void rebase(uint64_t new_hash)
    {        
        // Store a reference to the root node here so it isn't removed
        auto new_root_node = nodes.at(new_hash).lock();

        prune_except_hash(root_node->get_hash(), new_hash);

        root_node = new_root_node;
    }

public:
    void set_new_root_from_move(const Move& move)
    {
        Board new_root_board(root_node->get_board(), move);

        set_new_root_from_board(new_root_board);
    }

    void set_new_root_from_board(const Board& board)
    {
        rebase(hasher->get_hash(board));
    }

    void populate_tree(uint8_t search_depth, std::list<std::shared_ptr<BoardTreeNode>> btns, Piece::Colour colour_to_move)
    {
        if (search_depth == 0)
        {
            return;
        }

        std::list<std::shared_ptr<BoardTreeNode>> new_btns;

        for (auto btn : btns)
        {
            auto available_moves = btn->get_board().get_all_legal_moves(colour_to_move);

            for (const auto& move : available_moves)
            {
                auto child = btn->child_node(move, nodes, *hasher);

                //for (int i = 0; i < 4 - search_depth; ++i)
                //{
                //    std::cout << "  ";
                //}

                //std::cout << move.to_string() << " - " << child->get_score(Piece::BLACK, colour_to_move) << std::endl;

                new_btns.push_back(child);
            }

            if (available_moves.size() == 0)
            {
                btn->set_check_for_mate();
            }
        }

        populate_tree(search_depth - 1, new_btns, Piece::opposite_colour(colour_to_move));
    }

    Move search(uint8_t search_depth, Piece::Colour ai_colour)
    {
        auto available_moves = root_node->get_board().get_all_legal_moves(ai_colour);

        //std::list<Move> available_moves = { Move(root_node->get_board(), "d4e2") };

        std::unique_ptr<Move> best_move;
        double best_score = -9999;

        for(const auto& move : available_moves)
        {
            auto child = root_node->child_node(move, nodes, *hasher);

            //std::cout << "==============================================" << std::endl;
            //std::cout << move.to_string() << " - " << child->get_score(Piece::BLACK, Piece::opposite_colour(ai_colour)) << std::endl;

            populate_tree(search_depth, { child }, Piece::opposite_colour(ai_colour));

            auto child_score = child->get_score(ai_colour, Piece::opposite_colour(ai_colour));

            //std::cout << "move = " << move.to_string() << ", heuristic = " << child_score << std::endl;

            if (child_score > best_score)
            {
                best_move = std::make_unique<Move>(move);

                best_score = child_score;

                //std::cout << "best move = " << best_move->to_string() << ", heuristic = " << best_score << std::endl;
            }
        }

        return *best_move;
    }

    BoardTree(const Board& board) :
        hasher(std::make_unique<ZobristHash>()),
        root_node(std::make_shared<BoardTreeNode>(board, *hasher))
    {
        nodes.emplace(root_node->get_hash(), root_node);
    }
};