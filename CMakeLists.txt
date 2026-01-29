cmake_minimum_required(VERSION 3.15)

project(STLRom VERSION 0.1 LANGUAGES CXX)

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

add_executable(STLRomParser ${SOURCE_FILES} ${HEADER_FILES} ${BISON_MyParser_OUTPUTS} ${FLEX_MyScanner_OUTPUTS})

if (BUILD_PYTHON_EXTENSION)
  message(STATUS "Building Python Extension module.")
  find_package(Python COMPONENTS Interpreter Development REQUIRED)
  find_package(pybind11 CONFIG REQUIRED)

  pybind11_add_module(_stlrom
     src/pybind11/main.cpp 
     ${SOURCE_FILES})

  install (
    TARGETS _stlrom
   DESTINATION stlrom
  )
else()
  message(STATUS "Building C++ library only.")
  
endif()

add_library(stlromlib SHARED
    ${SOURCE_FILES}
    ${BISON_MyParser_OUTPUTS}
    ${FLEX_MyScanner_OUTPUTS})     


# Enables make test-cpp and make test-python to run python and cpp files separately
# labels are added to tests in test/CMakeLists.txt
add_custom_target(test-cpp
    COMMAND ${CMAKE_CTEST_COMMAND} -L cpp --output-on-failure
    COMMENT "Running C++ unit tests..."
)

add_custom_target(test-python
    COMMAND ${CMAKE_CTEST_COMMAND} -L python --output-on-failure
    COMMENT "Running Python extension tests..."
)
