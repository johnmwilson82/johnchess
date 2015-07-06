#include "xboard_interface.h"

XBoardInterface::XBoardInterface(std::istream& instr, std::ostream& outstr, std::string app_name) :
    m_instr(instr),
    m_outstr(outstr),
    m_app_name(app_name)
{
    //instr.setf(std::ios::unitbuf);
    //outstr.setf(std::ios::unitbuf);
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
    write_command("Illegal move: ", rcvd.raw_str());
}

void XBoardInterface::reply_newline()
{
    m_outstr << std::endl;
}

void XBoardInterface::add_variant(std::string variant_name)
{
    m_variants.push_back(variant_name);
}

void XBoardInterface::add_feature(std::string feature_str)
{
    m_features.push_back(feature_str);
}

void XBoardInterface::add_option(std::string option_str)
{
    m_options.push_back(option_str);
}

void XBoardInterface::reply_ping(CommandReceived rcvd)
{
    write_command("pong", rcvd.get_params()[0]);
}

void XBoardInterface::reply_features()
{
    //send name
    m_outstr << "feature myname=\"" << m_app_name << "\"" << std::endl;

    //send done = 0
    m_outstr << "feature done=0" << std::endl;

    for(std::vector<std::string>::iterator it = m_features.begin(); it != m_features.end(); it++)
    {
        m_outstr << "feature " << *it << std::endl;
    }

    //send variants
    std::string variant_str;
    for(std::vector<std::string>::iterator it = m_variants.begin(); it != m_variants.end(); it++)
    {
        variant_str += *it;
        if(it != m_variants.end()-1) variant_str += ",";
    }
    m_outstr << "feature variants=\"" << variant_str << "\"" << std::endl;

    //send options
    for(std::vector<std::string>::iterator it = m_options.begin(); it != m_options.end(); it++)
    {
        m_outstr << "feature option=\"" << *it << "\"" << std::endl;
    }

    //done
    m_outstr << "feature done=1" << std::endl;
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
        std::size_t found = rcvd.find_first_of(" ", i);
        params.push_back(rcvd.substr(i, found));
        while(rcvd[found] == ' ') found++;
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
        else if(!command.compare("accepted") || !command.compare("rejected"))
        {
            m_type = NONE;
            break;
        }
        else if(!command.compare("xboard"))
        {
            m_type = INIT;
            break;
        }
        else if(!command.compare("protover"))
        {
            m_type = INFO_REQ;
            m_params = params;
            break;
        }
        else if(!command.compare("memory"))
        {
            m_type = MEMORY;
            m_params = params;
            break;
        }
        else if(!command.compare("ping"))
        {
            m_type = PING;
            m_params = params;
            break;
        }
        else if(!command.compare("level"))
        {
            m_type = LEVEL;
            m_params = params;
            break;
        }
        else if(!command.compare("time"))
        {
            m_type = TIME;
            m_params = params;
            break;
        }
        else if(!command.compare("otim"))
        {
            m_type = OTIM;
            m_params = params;
            break;
        }
        else if(!command.compare("random"))
        {
            m_type = RANDOM;
            break;
        }
        else if(!command.compare("quit"))
        {
            m_type = QUIT;
            break;
        }
        else if(!command.compare("new"))
        {
            m_type = NEW;
            break;
        }
        else if(!command.compare("post"))
        {
            m_type = POST;
            break;
        }
        else if(!command.compare("hard"))
        {
            m_type = HARD;
            break;
        }
        else if(check_move_string(command))
        {
            // in a move command, so process it
            m_type = MOVE;
            m_move_string = command;
            break;
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
