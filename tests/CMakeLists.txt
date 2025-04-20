find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})


add_executable(ufo_tests
        test.cpp
        #        testDecoder.cpp
        #        testEncoder.cpp
        #        testPiece.cpp
        #        testPrinter.cpp
)

target_link_libraries(
        ufo_tests
        Source
        GTest::gtest_main
        #        ${CURSES_LIBRARIES}
        #        ${Boost_LIBRARIES}
)
include(GoogleTest)
gtest_discover_tests(ufo_tests)


