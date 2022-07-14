cmake_minimum_required(VERSION 3.12)
project(KaleidoscopeCompiler)

add_subdirectory(src)

add_executable(KaleidoscopeCompiler src/main.cpp)
target_link_libraries(KaleidoscopeCompiler Lexer Parser)