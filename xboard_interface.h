#ifndef XBOARD_INTERFACE_H
#define XBOARD_INTERFACE_H

#include <iostream>
#include <string>
#include <stdexcept>

class XBoardInterface
{
public:
    class CommandReceived
    {
    public:
        typedef enum {
            MOVE,
            NONE
        } type_t;

    public:
        CommandReceived(std::string rcvd) :
            m_raw(rcvd),
            m_invalid(false),
            m_finish(false),
            m_type(NONE)
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
        type_t get_type() { return m_type; }
        std::string get_move_string()
        {
            if (m_type != MOVE)
                throw std::runtime_error("get_move_string called on invalid CommandReceived");
            return m_move_string;
        }

    private:
        bool check_move_string(std::string comm);
        type_t m_type;
        bool m_invalid, m_finish;
        std::string m_raw;
        std::string m_move_string;

    private:
        void parse(std::string rcvd);
    };


public:
    XBoardInterface(std::istream& instr, std::ostream& outstr);

public:
    void tell_info(std::string infostring);
    CommandReceived wait_for_command();
    void reply_invalid(CommandReceived rcvd);
    void reply_illegal_move(CommandReceived rcvd);

private:
    void write_command(std::string command, std::string content);

private:
    std::istream& m_instr;
    std::ostream& m_outstr;
};

#endif
