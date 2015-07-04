#ifndef XBOARD_INTERFACE_H
#define XBOARD_INTERFACE_H

#include <iostream>
#include <string>

class XBoardInterface
{
public:
    class CommandReceived
    {
    public:
        CommandReceived(std::string rcvd) :
            m_raw(rcvd),
            m_invalid(false),
            m_finish(false)
        { 
            try
            {
                parse(rcvd);
            }
            catch (...)
            {
                m_invalid = true;
            }
        }

        std::string raw_str() { return m_raw; };
        bool is_invalid() { return m_invalid; };
        bool is_finish() { return m_finish; };


    private:
        bool m_invalid, m_finish;
        std::string m_raw;

    private:
        void parse(std::string rcvd);
    };


public:
    XBoardInterface(std::istream& instr, std::ostream& outstr);

public:
    void tell_info(std::string infostring);
    CommandReceived wait_for_command();
    void reply_invalid(CommandReceived rcvd);

private:
    void write_command(std::string command, std::string content);

private:
    std::istream& m_instr;
    std::ostream& m_outstr;
};

#endif
