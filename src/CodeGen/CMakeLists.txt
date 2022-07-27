

add_library(CodeGen STATIC 
            CodeGenerator.cpp
            )

target_include_directories(CodeGen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(CodeGen AST LLVM) #Link to LLVM dynamically 