# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   # Specify the commit you depend on and update it regularly.
#   URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
# )

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

#For Windows: Prevent overriding the parent project's compiler/linker settings
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(main_tests test.cpp)
target_link_libraries(main_tests Testing GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(main_tests)
add_test(NAME ufo_tests_lib COMMAND tests)