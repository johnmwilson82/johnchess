#include "xboard_interface.h"
#include <csignal>

XBoardInterface::XBoardInterface(std::istream& instr, std::ostream& outstr, const std::string& app_name) :
    m_instr(instr),
    m_outstr(outstr),
    m_app_name(app_name)
{
    instr.setf(std::ios::unitbuf);
    outstr.setf(std::ios::unitbuf);
    signal(SIGINT, SIG_IGN);
}


void XBoardInterface::tell_info(const std::string& infostring)
{
    write_command("tellics say", infostring);
}

void XBoardInterface::write_command(const std::string& command, const std::string& content)
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

void XBoardInterface::add_variant(const std::string& variant_name)
{
    m_variants.push_back(variant_name);
}

void XBoardInterface::add_feature(const std::string& feature_str)
{
    m_features.push_back(feature_str);
}

void XBoardInterface::add_option(const std::string& option_str)
{
    m_options.push_back(option_str);
}

void XBoardInterface::reply_ping(CommandReceived rcvd)
{
    write_command("pong", rcvd.get_params()[0]);
}

void XBoardInterface::send_move(const std::string& move)
{
    write_command("move", move);
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

std::vector<std::string> XBoardInterface::read_edit_mode()
{
    std::vector<std::string> out;
    std::string rcvd;

    do{
        std::getline(m_instr, rcvd);
        out.push_back(rcvd);
    }while(rcvd.compare("."));

    return out;
}

bool XBoardInterface::CommandReceived::check_move_string(const std::string& comm)
{
    if (comm.size() != 4)
        return false;

    //move string is always in the form 'a2a4'
    return (comm[0] >= 'a' && comm[0] <= 'h') &&
           (comm[1] >= '1' && comm[1] <= '8') &&
           (comm[2] >= 'a' && comm[2] <= 'h') &&
           (comm[3] >= '1' && comm[3] <= '8');
}

void XBoardInterface::CommandReceived::parse(const std::string& rcvd)
{
    std::deque<std::string> params;
    std::size_t i = 0;
    std::string rawcmd = rcvd; // make a copy

    while ((i = rawcmd.find(' ')) != std::string::npos)
    {
        params.push_back(rawcmd.substr(0, i));

        // delete up to the delimeter AND the delimeter
        rawcmd.erase(0, i + 1); 
    }
    
    if (!rawcmd.empty()) params.push_back(rawcmd);

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
        else if(!command.compare("force"))
        {
            m_type = FORCE;
            break;
        }
        else if(!command.compare("go"))
        {
            m_type = GO;
            break;
        }
        else if(!command.compare("playother"))
        {
            m_type = GO;
            m_params.push_back("playother");
            break;
        }
        else if(!command.compare("edit"))
        {
            m_type = EDIT;
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
