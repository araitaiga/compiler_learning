cmake_minimum_required(VERSION 3.10)
add_compile_options(-O0 -g -Wall)
# C++14を使用する
set(CMAKE_CXX_STANDARD 14)

# プロジェクト名と使用する言語を指定
project(CalculatorCompiler LANGUAGES CXX)

# ヘッダファイルのインクルードパスを指定
include_directories(include)

# 実行ファイルを生成
add_executable(9cc src/simple_compiler.cpp)
