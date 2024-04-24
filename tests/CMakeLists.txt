enable_testing()

add_executable(TestMain test_main.cpp)
target_include_directories(TestMain PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/extern/googletest/googletest/include
        ${CMAKE_CURRENT_SOURCE_DIR}/utilities
        ${CMAKE_CURRENT_SOURCE_DIR}/tests)
target_link_libraries(TestMain PRIVATE
        Bedrock
        gtest
)