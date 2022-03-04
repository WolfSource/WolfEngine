cmake_minimum_required(VERSION 3.0.0)
project(18_networking_bus_node0 VERSION 1.68.0 DESCRIPTION "18_networking_bus_node0 sample for Wolf")

if (NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif()

# set the default path lib
if(UNIX)
    if(APPLE)
        # APPLE OSX
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bin/osx/)
    else()
        # LINUX
        if (CMAKE_BUILD_TYPE MATCHES Debug)
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bin/linux/x64/debug/)
        else()
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bin/linux/x64/release/)
        endif()
    endif()
endif()

set(CMAKE_C_COMPILER "clang")#gcc
set(CMAKE_CXX_COMPILER "clang++")#g++
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXE_LINKER_FLAGS    "-Wl,--as-needed ${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--as-needed ${CMAKE_SHARED_LINKER_FLAGS}")

add_executable(18_networking_bus_node0 
main.cpp
pch.cpp)

# includes
include(CPack)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/../../../../engine/src/wolf.system/
${CMAKE_CURRENT_SOURCE_DIR}/../../../../engine/deps/nanomsg/include/)

# pre processors
target_compile_definitions(18_networking_bus_node0 PUBLIC 
NN_HAVE_ACCEPT4=1 
NN_HAVE_BACKTRACE=1 
NN_HAVE_CLOCK_GETTIME=1 
NN_HAVE_CLOCK_MONOTONIC=1 
NN_HAVE_EPOLL=1 
NN_HAVE_EVENTFD=1 
NN_HAVE_GCC_ATOMIC_BUILTINS 
NN_HAVE_GETADDRINFO_A=1 
NN_HAVE_LIBNSL=1 
NN_HAVE_LINUX 
NN_HAVE_MSG_CONTROL=1 
NN_HAVE_PIPE2=1 
NN_HAVE_PIPE=1 
NN_HAVE_POLL=1 
NN_HAVE_SEMAPHORE 
NN_HAVE_SEMAPHORE_PTHREAD=1 
NN_HAVE_SOCKETPAIR=1 
NN_HAVE_UNIX_SOCKETS=1 
NN_MAX_SOCKETS=512 
NN_STATIC_LIB=1
_GNU_SOURCE 
_POSIX_PTHREAD_SEMANTICS 
_REENTRANT 
_THREAD_SAFE 
__linux
)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(18_networking_bus_node0 PUBLIC _DEBUG DEBUG) 
endif()

# compiler options
target_compile_options(18_networking_bus_node0 PRIVATE -fPIC -m64)

# libs
link_directories(/usr/local/lib)
if (CMAKE_BUILD_TYPE MATCHES Debug)
target_link_libraries(18_networking_bus_node0 ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bin/linux/x64/debug/libwolf.system.linux.so)
target_link_libraries(18_networking_bus_node0 ${CMAKE_CURRENT_SOURCE_DIR}/../../../../engine/deps/nanomsg/lib/linux/x64/debug/libnanomsg.a)
else()
target_link_libraries(18_networking_bus_node0 ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bin/linux/x64/release/libwolf.system.linux.so)
target_link_libraries(18_networking_bus_node0 ${CMAKE_CURRENT_SOURCE_DIR}/../../../../engine/nanomsg/lib/linux/x64/release/libnanomsg.a)
endif()

target_link_libraries(18_networking_bus_node0 anl rt nsl pthread dl)
