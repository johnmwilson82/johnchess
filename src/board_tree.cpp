#include "board_tree.h"
#include <vector>



bool BoardTree::prune_except_hash(uint64_t base_hash, uint64_t except_hash)
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


void BoardTree::rebase(uint64_t new_hash)
{
    // Store a reference to the root node here so it isn't removed
    auto new_root_node = nodes.at(new_hash).lock();

    prune_except_hash(root_node->get_hash(), new_hash);

    root_node = new_root_node;
}


void BoardTree::set_new_root_from_move(const Move& move)
{
    set_new_root_from_board(*root_node->get_board().clone_moved(move));
}


void BoardTree::set_new_root_from_board(const IBoard& board)
{
    rebase(hasher->get_hash(board));
}


void BoardTree::populate_tree(uint8_t search_depth, std::list<std::shared_ptr<BoardTreeNode>> btns, Piece::Colour colour_to_move)
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
            new_btns.emplace_back(
                btn->child_node(move, nodes, *hasher)
            );
        }

        if (available_moves.size() == 0)
        {
            btn->set_check_for_mate();
        }
    }

    populate_tree(search_depth - 1, new_btns, Piece::opposite_colour(colour_to_move));
}


Move BoardTree::search(uint8_t search_depth, Piece::Colour ai_colour)
{
    auto available_moves = root_node->get_board().get_all_legal_moves(ai_colour);

    std::unique_ptr<Move> best_move;
    double best_score = -9999;

    for (const auto& move : available_moves)
    {
        auto child = root_node->child_node(move, nodes, *hasher);

        populate_tree(search_depth, { child }, Piece::opposite_colour(ai_colour));

        auto child_score = child->get_score(ai_colour, Piece::opposite_colour(ai_colour));

        if (child_score > best_score)
        {
            best_move = std::make_unique<Move>(move);

            best_score = child_score;
        }
    }

    return *best_move;
}


BoardTree::BoardTree(const IBoard& board) :
    hasher(std::make_unique<ZobristHash>()),
    root_node(std::make_shared<BoardTreeNode>(board, *hasher))
{
    nodes.emplace(root_node->get_hash(), root_node);
}