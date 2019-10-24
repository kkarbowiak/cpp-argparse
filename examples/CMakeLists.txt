cmake_minimum_required(VERSION 3.15)

add_executable(example_01)
target_sources(example_01
    PRIVATE
    example_01.cpp)

target_include_directories(example_01
    PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

target_compile_features(example_01 PRIVATE cxx_std_17)
target_compile_options(example_01 PRIVATE
                           $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
                               -Wall -Wextra -pedantic>
                           $<$<CXX_COMPILER_ID:MSVC>:
                               /W4>)
