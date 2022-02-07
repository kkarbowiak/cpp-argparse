cmake_minimum_required(VERSION 3.15)

include(CTest)

add_executable(unittest)
target_sources(unittest
    PRIVATE
    main.cpp
    test_argument_parser.cpp
    test_parsing.cpp
    test_parsing_mutually_exclusive_group.cpp
    test_usage_message.cpp
    test_help_message.cpp
    test_error_message.cpp
    cstring_array.h
    custom_a.h
    custom_b.h)

target_include_directories(unittest
    PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

target_link_libraries(unittest PRIVATE doctest)

target_compile_features(unittest PRIVATE cxx_std_17)
target_compile_options(unittest PRIVATE
                           $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
                               -Wall -Wextra -Werror -pedantic-errors $ENV{COMPILER_OPTIONS}>
                           $<$<CXX_COMPILER_ID:MSVC>:
                               /W4 /WX>)
target_link_options(unittest PRIVATE $ENV{LINKER_OPTIONS})

add_test(NAME unittest COMMAND unittest)
add_custom_target(run_unit_test ALL
    COMMAND ${CMAKE_CTEST_COMMAND} -C debug --output-on-failure
    DEPENDS unittest)
