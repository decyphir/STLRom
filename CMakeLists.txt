cmake_minimum_required(VERSION 3.1)

project(STLROM2 VERSION 0.1 LANGUAGES CXX)

# Required stuff
find_package(FLEX)
find_package(BISON)

set(CMAKE_CXX_STANDARD 11)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Source files 
file (GLOB SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file (GLOB HEADER_FILES "${CMAKE_SOURCE_DIR}/src/*.h" "${CMAKE_SOURCE_DIR}/src/*.hh")

enable_testing()

BISON_TARGET(MyParser ${CMAKE_SOURCE_DIR}/src/parser.y ${CMAKE_SOURCE_DIR}/src/parser.cpp)
FLEX_TARGET(MyScanner ${CMAKE_SOURCE_DIR}/src/scanner.l  ${CMAKE_SOURCE_DIR}/src/scanner.cpp)
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

add_executable(STLROM2 ${SOURCE_FILES} ${HEADER_FILES} ${BISON_MyParser_OUTPUTS} ${FLEX_MyScanner_OUTPUTS})

