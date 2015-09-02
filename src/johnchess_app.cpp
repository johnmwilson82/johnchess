#include "johnchess_app.h"
#include <stdexcept>

JohnchessApp::JohnchessApp(int argc, const char* argv[]) :
    m_app_opts(NULL)
{
    m_app_opts = parse_args(argc, argv);
    m_xboard_interface = new XBoardInterface(get_input_stream(), get_output_stream(), "Johnchess v0.1");
    m_xboard_interface->add_feature("memory=1");
    m_xboard_interface->add_feature("setboard=0");
    m_xboard_interface->add_feature("ping=1");
    m_xboard_interface->add_variant("normal");

    show_welcome();
    m_board = new Board(8, 8);

    m_ai = new RandomAI(Piece::BLACK);
}

JohnchessApp::~JohnchessApp()
{
    if (m_app_opts)
        delete m_app_opts;

    delete m_xboard_interface;
    delete m_board;
    delete m_ai;
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
                if(!m_board->move_piece(rcvd.get_move_string()))
                    m_xboard_interface->reply_illegal_move(rcvd);
                else
                {
                    std::string move_string = m_ai->make_move(m_board).to_string();
                    if(!m_board->move_piece(move_string))
                        throw std::runtime_error("AI seems to have generated a nonsense move");
                    m_xboard_interface->send_move(move_string);
                }
                break;

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

            case XBoardInterface::CommandReceived::QUIT:
                finished = true;
                break;

            case XBoardInterface::CommandReceived::EDIT:
                m_board->set_from_edit_mode(m_xboard_interface->read_edit_mode());
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
