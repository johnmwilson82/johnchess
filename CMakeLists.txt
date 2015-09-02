#cmake_minimum_required (VERSION 3.1.0 FATAL_ERROR)
project(JOHNCHESS)

SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(src)

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)

set_property(TARGET johnchess PROPERTY CXX_STANDARD 11)
