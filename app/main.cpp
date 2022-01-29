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
        " r _ _ _ k _ _ r\n"
        " P p p p _ p p p\n"
        " _ b _ _ _ n b N\n"
        " n P _ _ _ _ _ _\n"
        " B B P _ P _ _ _\n"
        " q _ _ _ _ N _ _\n"
        " P p _ P _ _ P P\n"
        " R _ _ Q _ R K _\n"
        "w kq - 0 0\n"
    );


    BitBoard board = board_from_string_repr<BitBoard>(board_str);
    //board.set_to_start_position();

    board.make_move({ "f3d4" });
    //board.make_move({ "b2a1n" });
    //board.unmake_move({ "b2a1n" });
    //board.make_move({ "h6f7" });
    //board.make_move({ "f8b4" });
    //e8f7

    auto moves = board.get_all_legal_moves(board.get_colour_to_move());

    uint64_t total = 0;
    for (const auto& move : moves)
    {
        std::cout << move.to_string();// << "\n";
        board.make_move(move);
        //std::cout << "=================================\n";
        uint64_t res = perft(board, 3);
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