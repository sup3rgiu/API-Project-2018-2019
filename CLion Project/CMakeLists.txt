cmake_minimum_required(VERSION 3.14)
project(ProgettoAPI C)

set(CMAKE_C_STANDARD 11)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wmaybe-uninitialized -Wuninitialized -Wall -pedantic -Werror -g3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

add_executable(ProgettoAPI main.c)