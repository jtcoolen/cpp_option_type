cmake_minimum_required(VERSION 3.18)
project(tpNote3)

set(CMAKE_CXX_STANDARD 11)

add_executable(tpNote3 src/main.cpp include/optional_stack.hpp include/optional_pt.hpp include/optional.hpp include/opt.hpp)


target_compile_options(tpNote3 PRIVATE -Wall -Wextra -pedantic -Og -fsanitize=leak)
