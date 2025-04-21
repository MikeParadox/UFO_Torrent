# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   # Specify the commit you depend on and update it regularly.
#   URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
# )

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