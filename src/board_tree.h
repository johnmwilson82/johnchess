#pragma once
#include <unordered_map>

#include "move.h"
#include "board.h"
#include "zobrist_hash.h"
#include "heuristic.h"

#include "utils/board_strings.h"

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
    std::optional<Move> m_move;
    bool m_check_for_mate = false;

public:
    std::shared_ptr<BoardTreeNode> child_node(const Move& move, hash_map_t& hash_map, const ZobristHash& hasher);

    std::list<std::shared_ptr<BoardTreeNode>> get_children() { return m_child_nodes; };

    const Board& get_board() const;

    uint64_t get_hash() const;

    const double get_score(Piece::Colour ai_col, Piece::Colour colour_to_move) const;

    BoardTreeNode(const Board& board, const ZobristHash& hasher);
    BoardTreeNode(const BoardTreeNode& node, const Move& move, const ZobristHash& hasher);
    BoardTreeNode(const BoardTreeNode& node, const Move& move);

    std::optional<Move> get_move() const { return m_move; }

    void set_check_for_mate() { m_check_for_mate = true; }

    void set_root_node() { m_prev_hash = std::nullopt; }
};

class BoardTree
{
private:
    std::unique_ptr<ZobristHash> hasher;
    BoardTreeNode::hash_map_t nodes; 

    std::shared_ptr<BoardTreeNode> root_node;

    double best_heuristic;

    void prune_except_hash(uint64_t base_hash, uint64_t except_hash)
    {
        if (base_hash == except_hash)
        {
            return;
        }

        if (nodes.contains(base_hash))
        {
            auto base_node = nodes.at(base_hash);
            for (auto& child : base_node->get_children())
            {
                prune_except_hash(child->get_hash(), except_hash);
            }

            nodes.erase(base_hash);
        }
    }

    void rebase(uint64_t new_hash)
    {
        prune_except_hash(root_node->get_hash(), new_hash);

        root_node = nodes.at(new_hash);

        root_node->set_root_node();
    }

public:
    void set_new_root_from_move(const Move& move)
    {
        Board new_root_board(root_node->get_board(), move);

        set_new_root_from_board(new_root_board);
    }

    void set_new_root_from_board(const Board& board)
    {
        auto new_node = nodes.at(hasher->get_hash(board));

        rebase(hasher->get_hash(board));
    }

    void populate_tree(uint8_t search_depth, std::shared_ptr<BoardTreeNode> btn, Piece::Colour colour_to_move)
    {
        if(search_depth > 0)
        {
            auto available_moves = btn->get_board().get_all_legal_moves(colour_to_move);

            for(const auto& move : available_moves)
            {
                auto child = btn->child_node(move, nodes, *hasher);

                //for (int i = 0; i < 4 - search_depth; ++i)
                //{
                //    std::cout << "  ";
                //}

                //std::cout << child->get_move()->to_string() << " - " << child->get_score(Piece::BLACK, colour_to_move) << std::endl;
                
                populate_tree(search_depth-1, child, Piece::opposite_colour(colour_to_move));
            }

            if (available_moves.size() == 0)
            {
                btn->set_check_for_mate();
            }
        }
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
            //std::cout << child->get_move()->to_string() << " - " << child->get_score(Piece::BLACK, Piece::opposite_colour(ai_colour)) << std::endl;

            populate_tree(search_depth, child, Piece::opposite_colour(ai_colour));

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