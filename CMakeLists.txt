cmake_minimum_required(VERSION 3.22)
project(Bedrock VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

#====================== CONFIGURATION ========================#

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    set(BUILD_SHARED_LIBS ON)
    set(BUILD_AS_STANDALONE TRUE)
else()
    set(BUILD_AS_STANDALONE FALSE)
endif()

include(CheckFunctionExists)
include(CheckStructHasMember)
include(CheckTypeSize)
check_function_exists("fcntl" HAS_FCNTL)
check_function_exists("poll" HAS_POLL)
check_function_exists("getaddrinfo" HAS_GETADDRINFO)
check_function_exists("getnameinfo" HAS_GETNAMEINFO)
check_function_exists("gethostbyname_r" HAS_GETHOSTBYNAME_R)
check_function_exists("gethostbyaddr_r" HAS_GETHOSTBYADDR_R)
check_function_exists("inet_pton" HAS_INET_PTON)
check_function_exists("inet_ntop" HAS_INET_NTOP)
check_struct_has_member("struct msghdr" "msg_flags" "sys/types.h;sys/socket.h" HAS_MSGHDR_FLAGS)
set(CMAKE_EXTRA_INCLUDE_FILES "sys/types.h" "sys/socket.h")
check_type_size("socklen_t" HAS_SOCKLEN_T BUILTIN_TYPES_ONLY)
unset(CMAKE_EXTRA_INCLUDE_FILES)
if(MSVC)
    add_definitions(-W3)
else()
    add_definitions(-Wno-error)
endif()

if(HAS_FCNTL)
    add_definitions(-DHAS_FCNTL=1)
endif()
if(HAS_POLL)
    add_definitions(-DHAS_POLL=1)
endif()
if(HAS_GETNAMEINFO)
    add_definitions(-DHAS_GETNAMEINFO=1)
endif()
if(HAS_GETADDRINFO)
    add_definitions(-DHAS_GETADDRINFO=1)
endif()
if(HAS_GETHOSTBYNAME_R)
    add_definitions(-DHAS_GETHOSTBYNAME_R=1)
endif()
if(HAS_GETHOSTBYADDR_R)
    add_definitions(-DHAS_GETHOSTBYADDR_R=1)
endif()
if(HAS_INET_PTON)
    add_definitions(-DHAS_INET_PTON=1)
endif()
if(HAS_INET_NTOP)
    add_definitions(-DHAS_INET_NTOP=1)
endif()
if(HAS_MSGHDR_FLAGS)
    add_definitions(-DHAS_MSGHDR_FLAGS=1)
endif()
if(HAS_SOCKLEN_T)
    add_definitions(-DHAS_SOCKLEN_T=1)
endif()


#====================== BUILD ======================#
set(SRC_ENET_PREFIX  ${PROJECT_SOURCE_DIR}/src/enet)
set(SRC_BEDROCK_PREFIX  ${PROJECT_SOURCE_DIR}/src)

set(SOURCE_FILES
        ${SRC_ENET_PREFIX}/callbacks.c
        ${SRC_ENET_PREFIX}/compress.c
        ${SRC_ENET_PREFIX}/host.c
        ${SRC_ENET_PREFIX}/list.c
        ${SRC_ENET_PREFIX}/packet.c
        ${SRC_ENET_PREFIX}/peer.c
        ${SRC_ENET_PREFIX}/protocol.c
        ${SRC_ENET_PREFIX}/unix.c
        ${SRC_ENET_PREFIX}/win32.c
        ${SRC_BEDROCK_PREFIX}/bedrock.cpp
        ${SRC_BEDROCK_PREFIX}/layers.cpp
)


set(BEDROCK_FILES ${SOURCE_FILES})
set(BEDROCK_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(BEDROCK_LIBS)
set(BEDROCK_DEFINITIONS)


if(WIN32 AND BUILD_SHARED_LIBS AND (MSVC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    set(BEDROCK_DEFINITIONS
            ${BEDROCK_DEFINITIONS}
            -DENET_DLL=1
            -DENET_BUILDING_LIB
            -DBEDROCK_DLL=1)
endif()

if(WIN32)
    set(BEDROCK_LIBS winmm ws2_32)
endif()

if(NOT BUILD_AS_STANDALONE)
    set(BEDROCK_FILES ${BEDROCK_FILES} PARENT_SCOPE)
    set(BEDROCK_INCLUDE_DIR ${BEDROCK_INCLUDE_DIR} PARENT_SCOPE)
    set(BEDROCK_LIBS ${BEDROCK_LIBS} PARENT_SCOPE)
    set(BEDROCK_DEFINITIONS ${BEDROCK_DEFINITIONS} PARENT_SCOPE)
endif()


if(BUILD_AS_STANDALONE)
    # Only allow tests to compile for unix-like systems except for MacOS
    # Tests should only be run on POSIX/UNIX systems
    if(UNIX AND NOT APPLE)
        # Import googletest
        add_subdirectory(${PROJECT_SOURCE_DIR}/extern/googletest)
        add_subdirectory(tests)
    endif()

    add_definitions(${BEDROCK_DEFINITIONS})

    #======TESTING EXE=====#
    add_executable(BedrockDebug main.cpp)
    target_link_libraries(BedrockDebug ${BEDROCK_LIBS})
    target_include_directories(BedrockDebug PRIVATE ${BEDROCK_INCLUDE_DIR})
    #======================#

    include_directories(${BEDROCK_INCLUDE_DIR})

    add_library(Bedrock ${BEDROCK_FILES})
    target_link_libraries(Bedrock ${BEDROCK_LIBS})

    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${PROJECT_VERSION}")

    install(TARGETS Bedrock
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib/static)
    install(DIRECTORY include/ DESTINATION include)
endif()
