cmake_minimum_required(VERSION 3.15)

include(CTest)

add_executable(unittest)
target_sources(unittest
    PRIVATE
    main.cpp
    test.cpp)

target_include_directories(unittest
    PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

target_compile_features(unittest PRIVATE cxx_std_17)
target_compile_options(unittest PRIVATE -Wall -Wextra -pedantic)

add_test(NAME unittest COMMAND unittest)
