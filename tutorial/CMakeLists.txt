cmake_minimum_required(VERSION 3.15)

add_executable(basic)
target_sources(basic
    PRIVATE
    basic.cpp)

target_include_directories(basic
    PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

target_compile_features(basic PRIVATE cxx_std_17)
target_compile_options(basic PRIVATE
                           $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
                               -Wall -Wextra -pedantic>
                           $<$<CXX_COMPILER_ID:MSVC>:
                               /W4>)

add_executable(positional)
target_sources(positional
    PRIVATE
    positional.cpp)

target_include_directories(positional
    PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

target_compile_features(positional PRIVATE cxx_std_17)
target_compile_options(positional PRIVATE
                           $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
                               -Wall -Wextra -pedantic>
                           $<$<CXX_COMPILER_ID:MSVC>:
                               /W4>)
