cmake_minimum_required(VERSION 3.15)

project(STLRom VERSION 0.4.0 LANGUAGES CXX)

# assumes bison and flex installed with brew
if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH
        "/opt/homebrew/opt/flex"   # Apple Silicon
        "/opt/homebrew/opt/bison"  # Apple Silicon
        "/usr/local/opt/flex"      # Intel
        "/usr/local/opt/bison"     # Intel
    )
endif()

# Required stuff
find_package(FLEX)
find_package(BISON)

# Print the found versions/paths so you can verify Homebrew is being used
message(STATUS "Found Bison: ${BISON_EXECUTABLE} (Version: ${BISON_VERSION})")
message(STATUS "Found Flex: ${FLEX_EXECUTABLE} (Version: ${FLEX_VERSION})")

set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fsanitize=address")

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/include)

# --- MAC OS FIX: Force Homebrew Flex headers to the front ---
if(APPLE)
    include_directories(BEFORE 
        "/opt/homebrew/opt/flex/include"   # Apple Silicon
        "/usr/local/opt/flex/include"      # Intel
    )
endif()

if(FLEX_INCLUDE_DIRS)
    include_directories(BEFORE ${FLEX_INCLUDE_DIRS})
endif()

# Source files 
file (GLOB SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file (GLOB HEADER_FILES "${CMAKE_SOURCE_DIR}/include/*.h" "${CMAKE_SOURCE_DIR}/include/*.hh")

enable_testing()

add_subdirectory(${CMAKE_SOURCE_DIR}/test)

BISON_TARGET(MyParser ${CMAKE_SOURCE_DIR}/src/parser.y ${CMAKE_SOURCE_DIR}/src/parser.cpp DEFINES_FILE ${CMAKE_SOURCE_DIR}/include/parser.hpp)
FLEX_TARGET(MyScanner ${CMAKE_SOURCE_DIR}/src/scanner.l  ${CMAKE_SOURCE_DIR}/src/scanner.cpp)
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

add_executable(STLRomParser ${SOURCE_FILES} ${HEADER_FILES} ${BISON_MyParser_OUTPUTS} ${FLEX_MyScanner_OUTPUTS})

# --- MAC OS FIX: Link Flex Library (libfl) to your executable ---
if(FLEX_LIBRARIES)
    target_link_libraries(STLRomParser PRIVATE ${FLEX_LIBRARIES})
endif()
# ----------------------------------------------------------------


if (BUILD_PYTHON_EXTENSION)
  message(STATUS "Building Python Extension module.")
  find_package(Python COMPONENTS Interpreter Development REQUIRED)
  find_package(pybind11 CONFIG REQUIRED)

  pybind11_add_module(_stlrom
     src/pybind11/main.cpp 
     ${SOURCE_FILES}
     ${BISON_MyParser_OUTPUTS}
     ${FLEX_MyScanner_OUTPUTS})

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
