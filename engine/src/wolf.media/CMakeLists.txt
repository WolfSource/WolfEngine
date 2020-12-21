cmake_minimum_required(VERSION 3.0.0)
project(wolf.media_core.linux VERSION 1.68.0 DESCRIPTION "wolf.media_core.linux.x64")

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
/usr/include/
${CMAKE_CURRENT_SOURCE_DIR}/../../src/wolf.system/
)

add_library(wolf.media_core.linux SHARED ./w_media_core_pch.cpp
./w_media_core.cpp
)

# pre processors
target_compile_definitions(wolf.media_core.linux PUBLIC 
GLM_FORCE_DEPTH_ZERO_TO_ONE 
_GNU_SOURCE 
_POSIX_PTHREAD_SEMANTICS 
_REENTRANT 
_THREAD_SAFE
__WOLF_MEDIA_CORE__
__720__ 
__linux
)

if (CMAKE_BUILD_TYPE MATCHES Debug)
target_compile_definitions(wolf.media_core.linux PUBLIC _DEBUG DEBUG) 
endif()

# compiler options
target_compile_options(wolf.media_core.linux PRIVATE -fPIC -m64)

# libs
link_directories(/usr/local/lib)
if (CMAKE_BUILD_TYPE MATCHES Debug)

target_link_libraries(wolf.media_core.linux 
${CMAKE_CURRENT_SOURCE_DIR}/../../../bin/linux/x64/debug/libwolf.system.linux.so)

else()
target_link_libraries(wolf.media_core.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../../bin/linux/x64/release/libwolf.system.linux.so)
endif()

target_link_libraries(wolf.media_core.linux anl rt nsl pthread avcodec avformat avutil swresample swscale)
