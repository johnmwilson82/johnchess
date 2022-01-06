#include "../src/johnchess_app.h"

#if 1

int main(int argc, const char* argv[])
{
    // Create the app
    JohnchessApp johnchess_app = JohnchessApp(argc, argv);
    johnchess_app.main_loop();
}

#else

#include "../src/utils/perft.h"
int main(int argc, const char* argv[])
{
    Board board;
    board.set_to_start_position();

    std::cout << "perft result depth 4 = " << perft(board, 4) << " - expected 197281\n";
}

#endif