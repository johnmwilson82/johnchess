#include "johnchess_app.h"
#include <stdexcept>
#include <fstream>

#include "bitboards/bitboard.h"
#include "utils/board_strings.h"

JohnchessApp::JohnchessApp(int argc, const char* argv[]) :
    m_app_opts(NULL),
    m_force_mode(false)
{
    m_app_opts = parse_args(argc, argv);
    m_xboard_interface = std::make_unique<XBoardInterface>(get_input_stream(), get_output_stream(), "Johnchess v0.1");
    m_xboard_interface->add_feature("memory=1");
    m_xboard_interface->add_feature("setboard=0");
    m_xboard_interface->add_feature("ping=1");
    m_xboard_interface->add_variant("normal");

    show_welcome();
    m_board = std::make_unique<BitBoard>();
    m_board->set_to_start_position();

    m_ai = std::make_unique<BasicAI>(PieceColour::BLACK, 5);
}

JohnchessApp::~JohnchessApp()
{
    if (m_app_opts)
        delete m_app_opts;
}

void JohnchessApp::make_ai_move()
{
    auto moving_colour = m_board->get_colour_to_move();

    std::string move_string = m_ai->make_move(*m_board).to_string();

    m_board->make_move(move_string);

    m_board->get_all_legal_moves(m_board->get_colour_to_move()); // TODO: this is just to load the state for get_in_check
    if(m_board->get_in_check(moving_colour))
    {
        throw std::runtime_error("AI seems to have generated a nonsense move :" + move_string);
    }

    m_xboard_interface->send_move(move_string);

    //std::ofstream ofs("current_board.txt");

    //ofs << "Move = " << move_string << std::endl;

    //ofs << utils::board_to_string_repr(*m_board);

    //ofs << std::endl << (m_board->get_colour_to_move() == PieceColour::WHITE ? "White" : "Black") << " to move" << std::endl;
    //ofs.flush();
}

bool JohnchessApp::check_game_end()
{
    PieceColour colour_to_move = m_board->get_colour_to_move();

    // Check whether AI player is in mate
    BitBoard::Mate mate = m_board->get_mate(colour_to_move);
    if(mate != BitBoard::NO_MATE)
    {
        if(mate == BitBoard::STALEMATE)
        {
            m_xboard_interface->reply_result(XBoardInterface::Result::DRAW);
        }
        else //if mate == Board::CHECKMATE
        {
            // Do something for checkmate
            m_xboard_interface->reply_result(
                colour_to_move == PieceColour::BLACK ? 
                XBoardInterface::Result::WHITE_WIN :
                XBoardInterface::Result::BLACK_WIN);
        }
        return true;
    }

    return false;
}

void JohnchessApp::main_loop()
{
    bool finished = false;
    while(!finished)
    {
        XBoardInterface::CommandReceived rcvd = m_xboard_interface->wait_for_command();
        if (rcvd.is_invalid()){
            m_xboard_interface->reply_invalid(rcvd);
            continue;
        }

        switch(rcvd.get_type())
        {
            case XBoardInterface::CommandReceived::MOVE:
            {
                PieceColour colour_to_move = m_board->get_colour_to_move();

                const auto rcvd_move = rcvd.get_move_string();
                m_board->make_move(rcvd_move);

                m_board->get_all_legal_moves(m_board->get_colour_to_move()); // TODO: this is just to load the state for get_in_check
                if(m_board->get_in_check(colour_to_move))
                {
                    m_board->unmake_move(rcvd_move);
                    m_xboard_interface->reply_illegal_move(rcvd);
                    break;
                }

                // Check whether received move has caused game end
                if(check_game_end()) break;

                if(!m_force_mode)
                {
                    make_ai_move();
                }

                // Check whether AI move has caused game end
                if(check_game_end()) break;

                break;
            }
            case XBoardInterface::CommandReceived::INFO_REQ:
                m_xboard_interface->reply_features();
                break;

            case XBoardInterface::CommandReceived::INIT:
                m_xboard_interface->reply_newline();
                break;

            case XBoardInterface::CommandReceived::PING:
                m_xboard_interface->reply_ping(rcvd);
                break;

            case XBoardInterface::CommandReceived::NEW:
                m_board->set_to_start_position();
                break;

            case XBoardInterface::CommandReceived::MEMORY:
                // set max memory
            case XBoardInterface::CommandReceived::LEVEL:
                // set level
            case XBoardInterface::CommandReceived::POST:
                // set output pondering mode
            case XBoardInterface::CommandReceived::HARD:
                // set hard mode
            case XBoardInterface::CommandReceived::RANDOM:
                // set random mode
            case XBoardInterface::CommandReceived::TIME:
            case XBoardInterface::CommandReceived::OTIM:
            case XBoardInterface::CommandReceived::NONE:
                break;

            case XBoardInterface::CommandReceived::FORCE:
                m_force_mode = true;
                break;

            case XBoardInterface::CommandReceived::QUIT:
                finished = true;
                break;

            case XBoardInterface::CommandReceived::EDIT:
                m_board->set_from_edit_mode(m_xboard_interface->read_edit_mode());
                break;

            case XBoardInterface::CommandReceived::GO:
                m_force_mode = false;
                // if there are 0 params, this is an immediate go (alternatively it's playother)
                if (rcvd.get_params().size() == 0)
                {
                    make_ai_move();
                }
                break;

        }

    }
}

void JohnchessApp::show_welcome()
{
    m_xboard_interface->tell_info("   Johnchess 0.1");
    m_xboard_interface->tell_info("   by John Wilson");
}

std::istream& JohnchessApp::get_input_stream()
{
    if (m_app_opts->in_stream)
        return *m_app_opts->in_stream;

    return std::cin;
}

std::ostream& JohnchessApp::get_output_stream()
{
    if (m_app_opts->out_stream)
        return *m_app_opts->out_stream;

    return std::cout;
}

JohnchessApp::app_opts_t* JohnchessApp::parse_args(int argc, const char* argv[])
{
    JohnchessApp::app_opts_t *opts = new JohnchessApp::app_opts_t;

    return opts;
}
