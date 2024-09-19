include(Testing)

add_executable(unit_tests queue_test.cpp)
target_link_libraries(unit_tests PRIVATE queue_lib)
AddTests(unit_tests)
