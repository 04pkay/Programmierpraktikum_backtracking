cmake_minimum_required(VERSION 3.23)
project(Programmierpraktikum)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

add_executable(Programmierpraktikum src/main.cpp include/SAT.h src/SAT.cpp src/backtracking.cpp include/backtracking.h include/ASAT.h src/ASAT.cpp include/CSAT.h src/CSAT.cpp)
