cmake_minimum_required(VERSION 3.26)
project(Dynamic_array C)

set(CMAKE_C_STANDARD 99)

add_executable(Dynamic_array main.c
        bytearray.c
        bytearray.h)
