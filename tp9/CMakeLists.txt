cmake_minimum_required(VERSION 3.18)
project(tp9)


set(CMAKE_CXX_STANDARD 11)
add_executable(tp9 main.cpp Mon_ptr_u.hpp Mon_ptr_u.tcc optional.hpp)

target_compile_options(tp9 PRIVATE -Werror -Wall -Wextra -pedantic -Og -fsanitize=leak)
