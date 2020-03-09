cmake_minimum_required(VERSION 3.15)

project("cpp-argparse")

add_library(${PROJECT_NAME} INTERFACE)

add_library(argparse-compilation OBJECT include/argparse.h)
target_compile_features(argparse-compilation PRIVATE cxx_std_17)
target_compile_options(argparse-compilation PRIVATE
                           $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
                               -Wall -Wextra -Wshadow -pedantic>
                           $<$<CXX_COMPILER_ID:MSVC>:
                               /W4>)

target_include_directories(${PROJECT_NAME} INTERFACE include/)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

enable_testing()
add_subdirectory(test/unittest)
add_subdirectory(thirdparty/doctest)

add_subdirectory(examples)
