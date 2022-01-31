#include "../src/johnchess_app.h"

#if 0

int main(int argc, const char* argv[])
{
    // Create the app
    JohnchessApp johnchess_app = JohnchessApp(argc, argv);
    johnchess_app.main_loop();
}

#else

#include "../src/utils/perft.h"
#include "bitboards/bitboard.h"
#include "utils/board_strings.h"

using namespace utils;

int main(int argc, const char* argv[])
{
    std::string board_str(
        " r n b q _ k _ r\n"
        " p p _ P b p p p\n"
        " _ _ p _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " _ _ B _ _ _ _ _\n"
        " _ _ _ _ _ _ _ _\n"
        " P P P _ N n P P\n"
        " R N B Q K _ _ R\n"
        "w KQ - 1 8\n"
    );


    BitBoard board = board_from_string_repr<BitBoard>(board_str);
    //board.set_to_start_position();

    board.make_move({ "d7c8q" });
    board.make_move({ "f2h1" });
    //board.make_move({ "d2d3" });
    //board.make_move({ "a1b1" });
    //board.make_move({ "f1e1" });
    //board.make_move({ "f8b4" });
    //e1g1

    auto moves = board.get_all_legal_moves(board.get_colour_to_move());

    auto move = std::ranges::find_if(moves, [&](const Move& move) { return move == Move("b2a1n"); });

    //board.make_move(*move);
    //board.unmake_move(*move);
    //board.make_move(*move);

    uint64_t total = 0;
    for (const auto& move : moves)
    {        
        //if (!(move == Move("g7h6") || move == Move("b2a1n"))) continue;

        std::cout << move.to_string();// << "\n";

        board.make_move(move);
        //std::cout << "=================================\n";
        uint64_t res = perft(board, 0);
        total += res;
        //std::cout << "perft result depth 1 = " << perft(board, 1) << "\n";
        //std::cout << "perft result depth 2 = " << perft(board, 2) << "\n";
        //std::cout << "perft result depth 3: \n" << perft(board, 3) << "\n";
        std::cout << ": " << res << "\n";
        //std::cout << "perft result depth 4 = " << perft(board, 5) << "\n\n";
        board.unmake_move(move);
    }
    std::cout << "Total = " << total << "\n";
}

#endif