cmake_minimum_required(VERSION 3.15)

set(targets
    basic
    positional
    positional1
    positional3
    optional
    optional1
    optional2
    complex
    complex1
    complex2
    complex3
    advanced
    advanced1
    conflicting
    conflicting1
    nohelp
    undesired
    nohandling
    nohandling1
    custom
    custom1
    custom2
    types
    string)

foreach(target ${targets})
        add_executable(${target})
        target_sources(${target} PRIVATE ${target}.cpp)
        target_compile_features(${target} PRIVATE cxx_std_17)
        target_compile_options(${target} PRIVATE
                               $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
                                   -Wall -Wextra -Werror -pedantic-errors>
                               $<$<CXX_COMPILER_ID:MSVC>:
                                   /W4 /WX /permissive->)
        target_link_libraries(${target} PRIVATE cpp-argparse)
endforeach()

add_custom_target(tutorial)
add_dependencies(tutorial ${targets})
