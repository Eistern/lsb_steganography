cmake_minimum_required(VERSION 3.17)
project(lsb_steganography)

set(CMAKE_CXX_STANDARD 14)

add_executable(coder coder.cpp lodepng/lodepng.cpp lodepng/lodepng.h)