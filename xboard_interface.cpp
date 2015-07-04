#include "xboard_interface.h"

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
    write_command("Error (unknown command):", rcvd.raw_str());
}

void XBoardInterface::CommandReceived::parse(std::string rcvd)
{
    m_invalid = true;
}


XBoardInterface::CommandReceived XBoardInterface::wait_for_command()
{
    std::string rcvd;
    std::getline(m_instr, rcvd);
    return XBoardInterface::CommandReceived(rcvd);

}
