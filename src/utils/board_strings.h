#pragma once

#include <string>

#include "../board.h"

namespace utils
{
    Board board_from_string_repr(const std::string& board_str);
    std::string board_to_string_repr(const Board& board);
};