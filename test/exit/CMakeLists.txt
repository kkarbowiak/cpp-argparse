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

add_test(NAME normal-test COMMAND app arg --optional opt)
set_property(TEST normal-test PROPERTY PASS_REGULAR_EXPRESSION "positional: arg\noptional: opt")

add_test(NAME help-test COMMAND app --help)
set_property(TEST help-test PROPERTY PASS_REGULAR_EXPRESSION "usage: app \\[-h\\] \\[--optional OPTIONAL\\] \\[-v\\] positional\n\npositional arguments:\n  positional\n\noptional arguments:\n  -h, --help            show this help message and exit\n  --optional OPTIONAL\n  -v, --version         show program's version number and exit\n")

add_test(NAME version-test COMMAND app --version)
set_property(TEST version-test PROPERTY PASS_REGULAR_EXPRESSION "1\.0\.0")

add_test(NAME error-test-1 COMMAND app)
set_property(TEST error-test-1 PROPERTY WILL_FAIL true)

add_test(NAME error-test-2 COMMAND app)
set_property(TEST error-test-2 PROPERTY PASS_REGULAR_EXPRESSION "the following arguments are required: positional\nusage: app \\[-h\\] \\[--optional OPTIONAL\\] \\[-v\\] positional\n\npositional arguments:\n  positional\n\noptional arguments:\n  -h, --help            show this help message and exit\n  --optional OPTIONAL\n  -v, --version         show program's version number and exit\n")

add_custom_target(run-exit-test ALL
    COMMAND ${CMAKE_CTEST_COMMAND} -C debug --output-on-failure
    DEPENDS app)
