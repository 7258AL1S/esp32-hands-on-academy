cmake_minimum_required(VERSION 3.16)
project(hardware_reference_contracts LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
add_executable(host_contracts host_contracts.cpp)
set(H03_POLICY_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/../lessons/H03/solution/button_timer.hpp"
    CACHE FILEPATH "Your H03 policy header; defaults to maintainer reference")
set(H05_PARSER_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/../lessons/H05/solution/line_parser.hpp"
    CACHE FILEPATH "Your H05 parser header; defaults to maintainer reference")
target_compile_definitions(host_contracts PRIVATE
    H03_POLICY_HEADER="${H03_POLICY_HEADER}"
    H05_PARSER_HEADER="${H05_PARSER_HEADER}")
if(MSVC)
    target_compile_options(host_contracts PRIVATE /W4)
else()
    target_compile_options(host_contracts PRIVATE -Wall -Wextra -Wpedantic)
endif()
enable_testing()
add_test(NAME reference_contracts COMMAND host_contracts)
