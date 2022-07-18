
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
llvm_map_components_to_libnames(llvm_libs support core irreader target instcombine)


add_library(CodeGen STATIC 
            CodeGenerator.cpp
            CodeGenerationDriver.cpp
            )
message(STATUS "Linking against following llvm libs ${llvm_libs}")
target_include_directories(CodeGen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(CodeGen AST LLVM-14)