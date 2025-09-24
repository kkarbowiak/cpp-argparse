cmake_minimum_required(VERSION 3.15)

set(targets
    advanced1
    advanced2
    basic
    complex1
    complex2
    complex3
    complex4
    conflicting1
    conflicting2
    count1
    count2
    count3
    count4
    custom1
    custom2
    custom3
    nohandling1
    nohandling2
    nohandling3
    nohelp
    optional1
    optional2
    optional3
    positional1
    positional2
    positional3
    positional4
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
