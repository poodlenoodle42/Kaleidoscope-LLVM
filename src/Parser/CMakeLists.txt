add_library(Parser STATIC parser.cpp)
target_include_directories(Lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Parser Lexer)