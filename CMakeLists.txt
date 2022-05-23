cmake_minimum_required(VERSION 3.4...3.18)
project(pyrobonTL)

add_subdirectory(pybind11)
pybind11_add_module(pyrobonTL src/main.cpp)

add_subdirectory(src)

include_directories(include)

target_compile_definitions(pyrobonTL PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})