# cmake_minimum_required(VERSION <specify CMake version here>)
project(GameOfLive C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")


add_executable(GameOfLive main.c)