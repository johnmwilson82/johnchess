#include "johnchess_app.h"

JohnchessApp::JohnchessApp(int argc, const char* argv[]) :
    m_app_opts(NULL)
{
    m_app_opts = parse_args(argc, argv);
    m_xboard_interface = new XBoardInterface(get_input_stream(), get_output_stream()); //opts.out_stream);
    show_welcome();
}

JohnchessApp::~JohnchessApp()
{
    if (m_app_opts)
        delete m_app_opts;

    delete m_xboard_interface;
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
        if (rcvd.is_finish()){
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
