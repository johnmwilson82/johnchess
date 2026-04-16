include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(johnchess_tests
    test_bitboard.cpp
    test_heuristic.cpp
    test_ai.cpp
    test_zobrist_hash.cpp
    test_read_write_board.cpp
    perft.cpp
)

target_link_libraries(johnchess_tests PRIVATE johnchess_lib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(johnchess_tests)
