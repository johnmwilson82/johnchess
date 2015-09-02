#ifndef XBOARD_INTERFACE_H
#define XBOARD_INTERFACE_H

#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <cstdlib>
#include <deque>

class XBoardInterface
{
public:
    class CommandReceived
    {
    public:
        typedef enum {
            MOVE,
            INIT,
            INFO_REQ,
            PING,
            LEVEL,
            RANDOM,
            MEMORY,
            NEW,
            POST,
            HARD,
            QUIT,
            TIME,
            OTIM,
            EDIT,
            FORCE,
            GO,
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

        std::string raw_str() { return m_raw; }
        bool is_invalid() { return m_invalid; }
        bool is_finish() { return m_finish; }
        type_t get_type() { return m_type; }

        std::string get_move_string()
        {
            if (m_type != MOVE)
                throw std::runtime_error("get_move_string called on invalid CommandReceived");
            return m_move_string;
        }
        std::deque<std::string> get_params() { return m_params; }
        std::deque<int> get_intparams()
        {
            std::deque<int> ret;
            for(std::deque<std::string>::iterator it = m_params.begin(); it != m_params.end(); it++)
            {
                ret.push_back(atoi((*it).c_str()));
            }
            return ret;
        }

    private:
        bool check_move_string(std::string comm);
        type_t m_type;
        bool m_invalid, m_finish;
        std::string m_raw;
        std::string m_move_string;
        std::deque<std::string> m_params;

    private:
        void parse(std::string rcvd);
    };


public:
    XBoardInterface(std::istream& instr, std::ostream& outstr, std::string app_name);

public:
    void tell_info(std::string infostring);
    CommandReceived wait_for_command();
    void reply_invalid(CommandReceived rcvd);
    void reply_illegal_move(CommandReceived rcvd);
    void reply_ping(CommandReceived rcvd);
    void send_move(std::string move);
    void reply_newline();
    void add_variant(std::string variant_name);
    void add_feature(std::string feature_str);
    void add_option(std::string option_str);
    void reply_features();
    std::vector<std::string> read_edit_mode();

private:
    void write_command(std::string command, std::string content);

private:
    std::istream& m_instr;
    std::ostream& m_outstr;

    std::string m_app_name;
    std::vector<std::string> m_variants;
    std::vector<std::string> m_features;
    std::vector<std::string> m_options;
};

#endif
