cmake_minimum_required(VERSION 3.15)

project(STLRom2 VERSION 0.1 LANGUAGES CXX)

# Required stuff
find_package(FLEX)
find_package(BISON)

set(CMAKE_CXX_STANDARD 11)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files 
file (GLOB SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file (GLOB HEADER_FILES "${CMAKE_SOURCE_DIR}/include/*.h" "${CMAKE_SOURCE_DIR}/include/*.hh")

enable_testing()

add_subdirectory(${CMAKE_SOURCE_DIR}/test)


BISON_TARGET(MyParser ${CMAKE_SOURCE_DIR}/src/parser.y ${CMAKE_SOURCE_DIR}/src/parser.cpp)
FLEX_TARGET(MyScanner ${CMAKE_SOURCE_DIR}/src/scanner.l  ${CMAKE_SOURCE_DIR}/src/scanner.cpp)
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

add_executable(STLRom2 ${SOURCE_FILES} ${HEADER_FILES} ${BISON_MyParser_OUTPUTS} ${FLEX_MyScanner_OUTPUTS})

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
    ${SOURCE_FILES}
    ${BISON_MyParser_OUTPUTS}
    ${FLEX_MyScanner_OUTPUTS})     


