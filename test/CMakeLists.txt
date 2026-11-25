project(STLRomtest)

# C++ Test Executables
set(CPP_TESTS test_driver test_until test_monitor test_stress test_stl_data test_past_operators)

foreach(t ${CPP_TESTS})
    add_executable(${t} "${CMAKE_CURRENT_SOURCE_DIR}/${t}.cpp")
    target_link_libraries(${t} PRIVATE stlromlib)
    
    add_test(NAME cpp_${t} COMMAND ${t} WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    set_tests_properties(cpp_${t} PROPERTIES LABELS "cpp")
endforeach()


# Python test scripts
# Helper function
function(add_python_test TEST_NAME SCRIPT_NAME)
    add_test(NAME ${TEST_NAME} 
             COMMAND python ${SCRIPT_NAME} 
             WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    set_tests_properties(${TEST_NAME} PROPERTIES LABELS "python")
endfunction()



add_python_test(test_t0                 test_online_rob_t0.py)
add_python_test(test_>                  test_gt.py)
add_python_test(test_<                  test_lt.py)
add_python_test(test_or                 test_or.py)
add_python_test(test_always             test_alw.py)
add_python_test(test_eventually         test_ev.py)
add_python_test(test_implies            test_implies.py)
add_python_test(test_U                  test_U.py)
add_python_test(test_SetBigM            test_SetBigM.py)
add_python_test(test_past_operators     test_past_operators.py)



# ---------------------------------------------------------------------------
# New test suite (test/suite/, run with pytest). Kept apart from the tests
# above: it is not part of `ctest`. Run it with
#     cmake --build build --target test-suite
# It tests the Python module installed in .venv, not this CMake build, so after
# a C++ change reinstall it first: uv pip install --python .venv/bin/python -e .
# See test/suite/README.md.
# ---------------------------------------------------------------------------
if(EXISTS "${CMAKE_SOURCE_DIR}/.venv/bin/python")
    set(_suite_python "${CMAKE_SOURCE_DIR}/.venv/bin/python")
else()
    find_program(_suite_python NAMES python3 python)
endif()
set(STLROM_SUITE_PYTHON "${_suite_python}" CACHE FILEPATH "Python that runs test/suite")

add_custom_target(test-suite
    COMMAND ${STLROM_SUITE_PYTHON} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/suite
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    USES_TERMINAL
    COMMENT "Running the test suite in test/suite ..."
)
