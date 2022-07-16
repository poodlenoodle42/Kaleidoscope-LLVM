

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

BISON_TARGET(parser
             parser.yy
             ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp)

FLEX_TARGET(lexer
            lexer.l
            ${CMAKE_CURRENT_BINARY_DIR}/lexer.yy.cpp)
            

ADD_FLEX_BISON_DEPENDENCY(lexer parser)  

add_library(LexerParser STATIC 
            ${FLEX_lexer_OUTPUTS}
            ${BISON_parser_OUTPUTS}
            ParserDriver.cpp
            )

target_link_libraries(LexerParser AST)
target_include_directories(LexerParser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(LexerParser PRIVATE 
                            ${FLEX_INCLUDE_DIRS}
                            ${CMAKE_CURRENT_BINARY_DIR}
                            )  