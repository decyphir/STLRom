project(STLRomtest)

# C++ Test Executables
set(CPP_TESTS test_driver test_until test_monitor test_stress test_stl_data)

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



add_python_test(test_t0         test_online_rob_t0.py)
add_python_test(test_>          test_gt.py)
add_python_test(test_<          test_lt.py)
add_python_test(test_or         test_or.py)
add_python_test(test_always     test_alw.py)
add_python_test(test_eventually test_ev.py)
add_python_test(test_implies    test_implies.py)
add_python_test(test_U          test_U.py)
add_python_test(test_SetBigM    test_SetBigM.py)
