cmake_minimum_required(VERSION 3.15)

set(targets
    advanced
    advanced1
    basic
    complex
    complex1
    complex2
    complex3
    conflicting
    conflicting1
    count
    count2
    count4
    custom
    custom1
    custom2
    nohandling
    nohandling1
    nohandling2
    nohelp
    optional
    optional1
    optional2
    positional
    positional1
    positional3
    string
    types
    undesired
    version)

foreach(target ${targets})
        add_executable(${target})
        target_sources(${target} PRIVATE ${target}.cpp)
        target_compile_features(${target} PRIVATE cxx_std_20)
        target_compile_options(${target} PRIVATE
                               $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
                                   -Wall -Wextra -Wmisleading-indentation -Wunused
                                   -Wuninitialized -Wshadow -Wconversion -Wsign-conversion
                                   -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Woverloaded-virtual
                                   -Werror -pedantic-errors>
                               $<$<CXX_COMPILER_ID:MSVC>:
                                   /W4 /WX /permissive->)
        target_link_libraries(${target} PRIVATE cpp-argparse)
endforeach()

add_custom_target(tutorial)
add_dependencies(tutorial ${targets})
