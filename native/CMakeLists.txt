# StructSight Native Addon - CMake Configuration
cmake_minimum_required(VERSION 3.15)
project(structsight_native)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find LLVM
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Find Clang
find_package(Clang REQUIRED CONFIG)

# Include directories
include_directories(${CMAKE_JS_INC})
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/node_modules/node-addon-api)

# Add LLVM definitions
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Source files
set(SOURCE_FILES
    src/addon.cpp
    src/analyzer.cpp
    src/layout_calculator.cpp
    src/vtable_analyzer.cpp
)

# Create the addon
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# Link directories
link_directories(${LLVM_LIBRARY_DIRS})

# Link against LLVM and Clang libraries
# Use llvm-config to get the required libraries
execute_process(
    COMMAND llvm-config --libs support core
    OUTPUT_VARIABLE LLVM_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND llvm-config --system-libs
    OUTPUT_VARIABLE LLVM_SYSTEM_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

target_link_libraries(${PROJECT_NAME} 
    ${CMAKE_JS_LIB}
    clangTooling
    clangFrontend
    clangDriver
    clangSerialization
    clangParse
    clangSema
    clangAnalysis
    clangAST
    clangBasic
    clangEdit
    clangLex
    clangRewrite
    ${LLVM_LIBS}
    ${LLVM_SYSTEM_LIBS}
)

# Platform-specific settings
if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _HAS_EXCEPTIONS=1)
    target_compile_options(${PROJECT_NAME} PRIVATE /EHsc)
endif()

# NAPI version
target_compile_definitions(${PROJECT_NAME} PRIVATE NAPI_VERSION=8)
