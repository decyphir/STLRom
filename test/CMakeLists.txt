project(pyrobonTLtest)

add_executable(test_driver
  "${CMAKE_SOURCE_DIR}/test/test_driver.cpp"
  ${SOURCE_FILES}
	${HEADER_FILES}
)

add_test(NAME test_driver
          COMMAND test_driver
          WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test"
)

add_test(NAME pyrobonTLtest1
          COMMAND python test_prog1.py
          WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")
add_test(NAME pyrobonTLtest2
          COMMAND python test_prog2.py
          WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")
add_test(NAME pyrobonTLtest3
          COMMAND python test_prog3.py
          WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")
add_test(NAME pyrobonTLtest4
          COMMAND python test_prog4.py
          WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")
add_test(NAME pyrobonTLtest5
          COMMAND python test_prog5.py
          WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")
add_test(NAME pyrobonTLtest6
          COMMAND python test_prog6.py
          WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")
add_test(NAME pyrobonTLtest7
          COMMAND python test_prog7.py
          WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")