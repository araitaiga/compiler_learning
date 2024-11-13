cmake_minimum_required(VERSION 3.10)
add_compile_options(-O0 -g -Wall)
set(CMAKE_CXX_STANDARD 17)

project(CalculatorCompiler LANGUAGES CXX)
include_directories(include)

add_executable(9cc src/simple_compiler.cpp)
