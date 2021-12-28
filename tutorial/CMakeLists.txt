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

add_executable(positional1)
target_sources(positional1
    PRIVATE
    positional1.cpp)

target_include_directories(positional1
    PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

target_compile_features(positional1 PRIVATE cxx_std_17)
target_compile_options(positional1 PRIVATE
                           $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
                               -Wall -Wextra -pedantic>
                           $<$<CXX_COMPILER_ID:MSVC>:
                               /W4>)

add_executable(positional3)
target_sources(positional3
    PRIVATE
    positional3.cpp)

target_include_directories(positional3
    PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

target_compile_features(positional3 PRIVATE cxx_std_17)
target_compile_options(positional3 PRIVATE
                           $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
                               -Wall -Wextra -pedantic>
                           $<$<CXX_COMPILER_ID:MSVC>:
                               /W4>)

add_executable(optional)
target_sources(optional
    PRIVATE
    optional.cpp)

target_include_directories(optional
    PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

target_compile_features(optional PRIVATE cxx_std_17)
target_compile_options(optional PRIVATE
                           $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
                               -Wall -Wextra -pedantic>
                           $<$<CXX_COMPILER_ID:MSVC>:
                               /W4>)

add_executable(optional1)
target_sources(optional1
    PRIVATE
    optional1.cpp)

target_include_directories(optional1
    PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

target_compile_features(optional1 PRIVATE cxx_std_17)
target_compile_options(optional1 PRIVATE
                           $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
                               -Wall -Wextra -pedantic>
                           $<$<CXX_COMPILER_ID:MSVC>:
                               /W4>)

add_executable(optional2)
target_sources(optional2
    PRIVATE
    optional2.cpp)

target_include_directories(optional2
    PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

target_compile_features(optional2 PRIVATE cxx_std_17)
target_compile_options(optional2 PRIVATE
                           $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
                               -Wall -Wextra -pedantic>
                           $<$<CXX_COMPILER_ID:MSVC>:
                               /W4>)
