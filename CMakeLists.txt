cmake_minimum_required(VERSION 3.12)
project(KaleidoscopeCompiler C CXX)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

add_subdirectory(src)

add_executable(KaleidoscopeCompiler src/main.cpp)
target_link_libraries(KaleidoscopeCompiler LexerParser HelperVisitors CodeGen Compilation)
set_property(TARGET KaleidoscopeCompiler PROPERTY CXX_STANDARD 17)
set_property(TARGET KaleidoscopeCompiler PROPERTY ENABLE_EXPORTS 1)