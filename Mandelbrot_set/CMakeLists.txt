cmake_minimum_required(VERSION 3.26)
project(Mandelbrot_set C)

set(CMAKE_C_STANDARD 99)

add_executable(Mandelbrot_set main.c)

target_link_libraries(Mandelbrot_set SDL2)