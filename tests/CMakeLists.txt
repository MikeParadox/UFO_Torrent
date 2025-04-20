# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   # Specify the commit you depend on and update it regularly.
#   URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
# )

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

find_package(Curses REQUIRED)
include_directories(${CURSES_INDLUDE_DIRS})

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS}) 

#For Windows: Prevent overriding the parent project's compiler/linker settings
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(ufo_tests ../src/bencode/decode.cpp testDecoder.cpp)
target_link_libraries(ufo_tests Testing GTest::gtest_main
${CURSES_LIBRARIES} 
${Boost_LIBRARIES} 
)
include(GoogleTest)
gtest_discover_tests(ufo_tests)
add_test(NAME ufo_tests_lib COMMAND tests)