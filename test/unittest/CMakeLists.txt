cmake_minimum_required(VERSION 3.15)

include(CTest)

add_executable(unittest)
target_sources(unittest
    PRIVATE
    main.cpp
    test_ArgumentParser.cpp
    test_ArgumentParser_Parsing.cpp
    test_ArgumentParser_UsageMessage.cpp
    test_ArgumentParser_HelpMessage.cpp
    cstring_array.h
    custom.h)

target_include_directories(unittest
    PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

target_link_libraries(unittest PRIVATE doctest)

target_compile_features(unittest PRIVATE cxx_std_17)
target_compile_options(unittest PRIVATE
                           $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
                               -Wall -Wextra -pedantic>
                           $<$<CXX_COMPILER_ID:MSVC>:
                               /W4>)

add_test(NAME unittest COMMAND unittest)
