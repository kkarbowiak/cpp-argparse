cmake_minimum_required(VERSION 3.15)

project("cpp-argparse" VERSION 1.1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE include/)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

enable_testing()
add_subdirectory(test/unittest)
add_subdirectory(thirdparty/doctest)

add_subdirectory(tutorial)
