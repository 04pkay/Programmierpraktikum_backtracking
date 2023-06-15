cmake_minimum_required(VERSION 3.23)
project(Programmierpraktikum)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

include_directories(include)

add_executable(sat src/main.cpp include/SAT.h src/SAT.cpp src/backtracking.cpp include/backtracking.h include/ASAT.h src/ASAT.cpp include/CSAT.h src/CSAT.cpp)
add_executable(slimsat src/main.cpp include/slimSAT.h src/slimSAT.cpp src/backtracking.cpp include/backtracking.h include/ASAT.h src/ASAT.cpp include/CSAT.h src/CSAT.cpp)
