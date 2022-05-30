cmake_minimum_required(VERSION 3.1)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(pyrobonTL)

enable_testing()

include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_SOURCE_DIR}/pybind11")


file (GLOB SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file (GLOB HEADER_FILES "${CMAKE_SOURCE_DIR}/include/*.h" "${CMAKE_SOURCE_DIR}/include/*.hh")

# Set up such that XCode organizes the files
#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES} ${HEADER_FILES} )

add_subdirectory(${CMAKE_SOURCE_DIR}/pybind11)
pybind11_add_module(pyrobonTL 
  ${SOURCE_FILES}
	${HEADER_FILES}
)

add_test(NAME pyrobonTLtest
          COMMAND python ../test_prog.py
          WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")