
set(JOHNCHESS_SOURCES
    bitboards/bitboard.cpp
    bitboards/bitboard_ray_attacks.cpp
    board_location.cpp
    move.cpp
    heuristic.cpp
    search_tree.cpp
    search_tree_node.cpp
    johnchess_app.cpp
    xboard_interface.cpp
    zobrist_hash.cpp
)

add_library(johnchess_lib STATIC ${JOHNCHESS_SOURCES})
target_include_directories(johnchess_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(johnchess_lib PUBLIC cxx_std_20)

add_executable(johnchess ../app/main.cpp)
target_link_libraries(johnchess PRIVATE johnchess_lib)
