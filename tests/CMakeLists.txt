find_package(GTest CONFIG REQUIRED)
enable_testing()

add_executable(ClientTest ClientTests.cpp)
target_include_directories(ClientTest PRIVATE ${PROJECT_SOURCE_DIR}/include synchronization.h)
target_link_libraries(ClientTest PRIVATE GTest::gtest GTest::gtest_main Bedrock)
#target_compile_definitions(ClientTest PRIVATE SYNC_DEBUG)

add_executable(ServerTest ServerTests.cpp)
target_include_directories(ServerTest PRIVATE ${PROJECT_SOURCE_DIR}/include synchronization.h)
target_link_libraries(ServerTest PRIVATE GTest::gtest GTest::gtest_main Bedrock)
#target_compile_definitions(ServerTest PRIVATE SYNC_DEBUG)