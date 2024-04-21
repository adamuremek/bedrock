cmake_minimum_required(VERSION 3.22)
project(Bedrock)

set(CMAKE_CXX_STANDARD 17)

# Import ENet (I dunno why the community branch is "unofficial" in vcpkg
find_package(unofficial-enet CONFIG REQUIRED)


add_library(Bedrock STATIC
        src/layers.cpp
        src/bedrock.cpp
)
target_include_directories(Bedrock PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(Bedrock PRIVATE unofficial::enet::enet)


add_executable(BedrockDebug main.cpp)
target_link_libraries(BedrockDebug PRIVATE unofficial::enet::enet Bedrock)
target_include_directories(BedrockDebug PRIVATE ${PROJECT_SOURCE_DIR}/include)


# Only allow tests to compile for unix-like systems such as Linux or MacOS
# Tests should only be run on POSIX/UNIX systems
if(UNIX)
    add_subdirectory(tests)
endif()
