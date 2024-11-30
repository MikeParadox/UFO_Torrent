find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(main_tests test.cpp)
target_link_libraries(main_tests Testing GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(main_tests)
add_test(NAME ufo_tests_lib COMMAND tests)
