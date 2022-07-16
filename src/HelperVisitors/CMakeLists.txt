add_library(HelperVisitors STATIC 
            Printer.cpp
            )

target_include_directories(HelperVisitors PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(HelperVisitors AST)