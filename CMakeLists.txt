cmake_minimum_required(VERSION 3.23)
project(Programmierpraktikum)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(Programmierpraktikum examples/main.cpp include/SAT.h src/SAT.cpp)
