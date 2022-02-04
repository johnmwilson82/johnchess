#pragma once

#include <iostream>
#include "xboard_interface.h"

#include "bitboards/bitboard.h"
#include "ai.h"

class JohnchessApp {
private:
    typedef struct app_opts {
        std::istream *in_stream;
        std::ostream *out_stream;
        app_opts() : in_stream(NULL), out_stream(NULL) {}
    } app_opts_t;

public: //ctor + dtor
    JohnchessApp(int argc, const char* argv[]);
    ~JohnchessApp();

public: //public methods
    void main_loop();

private:
    std::unique_ptr<XBoardInterface> m_xboard_interface;
    std::unique_ptr<BitBoard> m_board;
    std::unique_ptr<AI> m_ai;

    app_opts_t* parse_args(int argc, const char* argv[]);
    void show_welcome();
    void make_ai_move();
    bool check_game_end();

    app_opts_t* m_app_opts;
    std::istream& get_input_stream();
    std::ostream& get_output_stream();
    bool m_force_mode;

};