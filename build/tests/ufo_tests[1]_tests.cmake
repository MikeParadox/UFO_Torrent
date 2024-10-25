add_test([=[TestCaseName.TestName]=]  [==[/home/mikhail/programming/c++/projects/UFO-Torrent/build/ufo_tests]==] [==[--gtest_filter=TestCaseName.TestName]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[TestCaseName.TestName]=]  PROPERTIES WORKING_DIRECTORY [==[/home/mikhail/programming/c++/projects/UFO-Torrent/build/tests]==] SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  ufo_tests_TESTS TestCaseName.TestName)
