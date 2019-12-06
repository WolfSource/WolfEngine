cmake_minimum_required(VERSION 3.0.0)
project(wolf.system.linux VERSION 1.68.0 DESCRIPTION "wolf.system.linux.x64")

if (NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif()

# set the default path lib
if(UNIX)
    if(APPLE)
        # APPLE OSX
        set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../bin/osx/)
    else()
        # LINUX
        if (CMAKE_BUILD_TYPE MATCHES Debug)
            set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../bin/linux/x64/debug/)
        else()
            set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../bin/linux/x64/release/)
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

# includes
include(CPack)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/../../deps/luaJIT/include/
)

add_library(wolf.system.linux SHARED ./lz4/lz4.c
./lz4/lz4frame.c
./lz4/lz4hc.c
./lz4/xxhash.c
./spdlog/fmt/bundled/format.cc
./spdlog/fmt/bundled/ostream.cc
./spdlog/fmt/bundled/posix.cc
./spdlog/fmt/bundled/printf.cc
./w_aligned_malloc.cpp
./w_bounding.cpp
./w_compress.c
./w_inputs_manager.cpp
./w_logger.cpp
./w_lua.cpp
./w_memory.cpp
./w_network.cpp
./w_system_pch.cpp
./w_task.cpp
./w_thread_pool.cpp
./w_thread.cpp
./w_time_span.cpp
./w_window.cpp
./w_xml.cpp
./wolf.cpp
)

# pre processors
target_compile_definitions(wolf.system.linux PUBLIC 
GLM_FORCE_DEPTH_ZERO_TO_ONE 
_GNU_SOURCE 
_POSIX_PTHREAD_SEMANTICS 
_REENTRANT 
_THREAD_SAFE 
__LUA__ 
__WOLF_SYSTEM__
__linux
)

if (CMAKE_BUILD_TYPE MATCHES Debug)
target_compile_definitions(wolf.system.linux PUBLIC _DEBUG DEBUG) 
endif()

# compiler options
target_compile_options(wolf.system.linux PRIVATE -fPIC -m64)

# libs
link_directories(/usr/local/lib)
if (CMAKE_BUILD_TYPE MATCHES Debug)
target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/luaJIT/lib/linux/x64/debug/libluajit-static.a)
else()
target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/luaJIT/lib/linux/x64/release/libluajit-static.a)
endif()

target_link_libraries(wolf.system.linux xcb anl rt nsl pthread)
