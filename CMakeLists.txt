cmake_minimum_required(VERSION 3.27)
project(Bedrock)

set(CMAKE_CXX_STANDARD 17)

# Improt ENet (I dunno why the community branch is "unofficial" in vcpkg
find_package(unofficial-enet CONFIG REQUIRED)

add_library(Bedrock STATIC library.cpp)

add_executable(
        BedrockDebug
        main.cpp
)

target_link_libraries(BedrockDebug PRIVATE unofficial::enet::enet)
