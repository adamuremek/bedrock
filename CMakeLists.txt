cmake_minimum_required(VERSION 3.22)
project(Bedrock VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

# Import ENet
add_subdirectory(${PROJECT_SOURCE_DIR}/extern/enet)

add_library(Bedrock STATIC
        src/layers.cpp
        src/bedrock.cpp
)
target_include_directories(Bedrock PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/extern/enet/include)
target_link_libraries(Bedrock PRIVATE enet)


#add_executable(BedrockDebug main.cpp)
#target_link_libraries(BedrockDebug PRIVATE enet Bedrock)
#target_include_directories(BedrockDebug PRIVATE ${PROJECT_SOURCE_DIR}/include)


# Only allow tests to compile for unix-like systems such as Linux or MacOS
# Tests should only be run on POSIX/UNIX systems
if(UNIX)
    add_subdirectory(tests)
endif()

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${PROJECT_VERSION}")

install(TARGETS Bedrock
        EXPORT BedrockTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)
install(DIRECTORY include/ DESTINATION include)