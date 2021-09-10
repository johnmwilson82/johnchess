#include "johnchess_app.h"

int main(int argc, const char* argv[])
{
    // Create the app
    JohnchessApp johnchess_app = JohnchessApp(argc, argv);
    johnchess_app.main_loop();
}
