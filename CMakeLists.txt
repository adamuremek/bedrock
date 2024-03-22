cmake_minimum_required(VERSION 3.27)
project(Bedrock)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Import ENet (I dunno why the community branch is "unofficial" in vcpkg
find_package(unofficial-enet CONFIG REQUIRED)

file(GLOB BEDROCK_SRC "src/*.cpp")

add_library(Bedrock STATIC ${BEDROCK_SRC}
        tests/test1.cpp)
target_include_directories(Bedrock PRIVATE ${PROJECT_SOURCE_DIR}/include)


add_executable(BedrockDebug main.cpp)

target_link_libraries(BedrockDebug PRIVATE unofficial::enet::enet Bedrock)
target_include_directories(BedrockDebug PRIVATE ${PROJECT_SOURCE_DIR}/include)


add_subdirectory(tests)

