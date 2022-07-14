
add_library(Lexer STATIC lexer.cpp)
target_include_directories(Lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})