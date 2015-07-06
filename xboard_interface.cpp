#include "xboard_interface.h"
#include <deque>

XBoardInterface::XBoardInterface(std::istream& instr, std::ostream& outstr) :
    m_instr(instr),
    m_outstr(outstr)
{

}


void XBoardInterface::tell_info(std::string infostring)
{
    write_command("tellics say", infostring);
}

void XBoardInterface::write_command(std::string command, std::string content)
{
    m_outstr << command << " " << content << std::endl;
}

void XBoardInterface::reply_invalid(XBoardInterface::CommandReceived rcvd)
{
    write_command("Error (unknown command): ", rcvd.raw_str());
}

void XBoardInterface::reply_illegal_move(CommandReceived rcvd)
{
    write_command("Illegal move:%s", rcvd.raw_str());
}

bool XBoardInterface::CommandReceived::check_move_string(std::string comm)
{
    if (comm.size() != 4)
        return false;

    //move string is always in the form 'a2a4'
    return (comm[0] >= 'a' && comm[0] <= 'h') &&
           (comm[1] >= '1' && comm[1] <= '8') &&
           (comm[2] >= 'a' && comm[2] <= 'h') &&
           (comm[3] >= '1' && comm[3] <= '8');
}

void XBoardInterface::CommandReceived::parse(std::string rcvd)
{
    std::deque<std::string> params;
    std::size_t i = 0;
    while (i != std::string::npos)
    {
        std::size_t found = rcvd.find_first_of(" ");
        params.push_back(rcvd.substr(i, found));
        i = found;
    }
    while (!params.empty())
    {
        std::string command = params[0];
        params.pop_front();
        if(!command.compare("tellics"))
        {
            // do something
        }
        else if(check_move_string(command))
        {
            // in a move command, so process it
            m_type = MOVE;
        }
        else
        {
            m_invalid = true;
        }
    }
}

XBoardInterface::CommandReceived XBoardInterface::wait_for_command()
{
    std::string rcvd;
    std::getline(m_instr, rcvd);
    return XBoardInterface::CommandReceived(rcvd);

}
