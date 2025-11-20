cmake_minimum_required(VERSION 3.15)

# Option to control building the Python extension
option(BUILD_PYTHON_EXTENSION "Build the pybind11 Python extension module" OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(stlrom)
enable_testing()

include_directories("${CMAKE_SOURCE_DIR}/include")

file (GLOB SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_subdirectory(${CMAKE_SOURCE_DIR}/test)

if (BUILD_PYTHON_EXTENSION)
  message(STATUS "Building Python Extension module.")
  find_package(Python COMPONENTS Interpreter Development REQUIRED)
  find_package(pybind11 CONFIG REQUIRED)

  pybind11_add_module(stlrom
     src/pybind11/main.cpp 
     ${SOURCE_FILES})

  install (
    TARGETS stlrom
   DESTINATION .
  )
else()
  message(STATUS "Building C++ library only.")
  
endif()

add_library(stlromlib SHARED
    ${SOURCE_FILES})     

