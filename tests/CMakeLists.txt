find_package(GTest CONFIG REQUIRED)
enable_testing()

add_executable(TEST1 test1.cpp)
target_link_libraries(TEST1 PRIVATE GTest::gtest GTest::gtest_main)

add_executable(TEST2 test2.cpp)
target_link_libraries(TEST2 PRIVATE GTest::gtest GTest::gtest_main)
