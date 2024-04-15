find_package(GTest CONFIG REQUIRED)
enable_testing()

#add_executable(ClientTest client_tests.cpp)
#target_include_directories(ClientTest PRIVATE ${PROJECT_SOURCE_DIR}/include synchronization.h)
#target_link_libraries(ClientTest PRIVATE GTest::gtest GTest::gtest_main Bedrock)
##target_compile_definitions(ClientTest PRIVATE POSIX)
#
#add_executable(ServerTest server_tests.cpp)
#target_include_directories(ServerTest PRIVATE ${PROJECT_SOURCE_DIR}/include synchronization.h)
#target_link_libraries(ServerTest PRIVATE GTest::gtest GTest::gtest_main Bedrock)
##target_compile_definitions(ServerTest PRIVATE POSIX)
#
#add_executable(MessagingTests messaging_tests.cpp)
#target_include_directories(MessagingTests PRIVATE ${PROJECT_SOURCE_DIR}/include)
#target_link_libraries(MessagingTests PRIVATE GTest::gtest GTest::gtest_main Bedrock)

add_executable(TestMain test_main.cpp)
target_include_directories(TestMain PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/utilities
        ${CMAKE_CURRENT_SOURCE_DIR}/tests)
target_link_libraries(TestMain PRIVATE
        Bedrock
        GTest::gtest
)