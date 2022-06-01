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

add_test(NAME pyrobonTLtest
          COMMAND python test_prog.py
          WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")