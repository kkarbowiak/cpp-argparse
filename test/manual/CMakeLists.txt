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

add_test(NAME version-test COMMAND app --version)
set_property(TEST version-test PROPERTY PASS_REGULAR_EXPRESSION "1\.0\.0")

add_test(NAME error-test COMMAND app)
set_property(TEST error-test PROPERTY WILL_FAIL true)

add_custom_target(run_exit_test ALL
    COMMAND ${CMAKE_CTEST_COMMAND} -C debug --output-on-failure
    DEPENDS app)
