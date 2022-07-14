cmake_minimum_required(VERSION 3.12)
project(KaleidoscopeCompiler CXX)

add_subdirectory(src)

add_executable(KaleidoscopeCompiler src/main.cpp)
target_link_libraries(KaleidoscopeCompiler LexerParser)
set_property(TARGET KaleidoscopeCompiler PROPERTY CXX_STANDARD 17)