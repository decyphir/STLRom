project(STLRomtest)

add_executable(test_driver
  "${CMAKE_SOURCE_DIR}/test/test_driver.cpp"  
)
target_link_libraries(test_driver PRIVATE stlromlib)

add_executable(test_until
  "${CMAKE_SOURCE_DIR}/test/test_until.cpp"
)
target_link_libraries(test_until PRIVATE stlromlib)

add_executable(test_monitor
"${CMAKE_SOURCE_DIR}/test/test_monitor.cpp"
)
target_link_libraries(test_monitor PRIVATE stlromlib)

add_executable(test_stress
"${CMAKE_SOURCE_DIR}/test/test_stress.cpp"
)
target_link_libraries(test_stress PRIVATE stlromlib)

add_test(NAME test_cpp_until
COMMAND test_until
WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test"
)

add_test(NAME test_cpp_driver
          COMMAND test_driver
          WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test"
)

add_test(NAME test_t0
          COMMAND python test_online_rob_t0.py
          WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")

add_test(NAME test_>
          COMMAND python test_prog1.py
          WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")
add_test(NAME test_<
          COMMAND python test_prog2.py
          WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")
add_test(NAME test_or
          COMMAND python test_prog3.py
          WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")
add_test(NAME test_always
          COMMAND python test_prog4.py
          WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")
add_test(NAME test_eventually
          COMMAND python test_prog5.py
          WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")
add_test(NAME test_implies
          COMMAND python test_prog6.py
          WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")
add_test(NAME test_until
          COMMAND python test_prog7.py
          WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")
add_test(NAME test_SetBigM
          COMMAND python test_prog8.py
          WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")
