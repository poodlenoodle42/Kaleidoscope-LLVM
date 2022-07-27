

add_library(Compilation STATIC 
            AOT.cpp
            JIT.cpp
            )

target_include_directories(Compilation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Compilation CodeGen LLVM) #Link to LLVM dynamically 