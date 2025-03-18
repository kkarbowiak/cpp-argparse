cmake_minimum_required(VERSION 3.15)

project("cpp-argparse" VERSION 2.5.0 LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} INTERFACE include/)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    enable_testing()
    add_subdirectory(test/unittest)
    add_subdirectory(test/exit)
    add_subdirectory(thirdparty/doctest)

    add_subdirectory(tutorial)
endif()
