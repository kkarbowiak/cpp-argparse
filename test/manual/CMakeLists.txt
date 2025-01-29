cmake_minimum_required(VERSION 3.15)

add_executable(app)
target_sources(app
    PRIVATE
    main.cpp)

target_compile_features(app PRIVATE cxx_std_20)
target_compile_options(app PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Werror -pedantic-errors>
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4 /WX /permissive->)
target_link_libraries(app PRIVATE cpp-argparse)
