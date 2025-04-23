
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})


# Now simply link against gtest or gtest_main as needed. Eg
add_executable(ufo_tests ../src/bencode/decode.cpp testPiece.cpp testPrinter.cpp)

target_link_libraries(
        ufo_tests
        Source
        GTest::gtest_main
        #        ${CURSES_LIBRARIES}
        #        ${Boost_LIBRARIES}
)

include(GoogleTest)
gtest_discover_tests(ufo_tests)