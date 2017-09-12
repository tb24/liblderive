# add the executable
add_executable (TestAll main.test.c)
target_link_libraries (TestAll lderive)
target_link_libraries(TestAll m)

enable_testing()

add_test(simple TestAll WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

