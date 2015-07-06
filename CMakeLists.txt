#cmake_minimum_required (VERSION 3.1.0 FATAL_ERROR)
project(JOHNCHESS)

add_executable(johnchess johnchess.cpp johnchess_app.cpp xboard_interface.cpp board.cpp pieces.cpp)

 set_property(TARGET johnchess PROPERTY CXX_STANDARD 11)
