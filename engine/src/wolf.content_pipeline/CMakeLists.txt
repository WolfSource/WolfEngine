cmake_minimum_required(VERSION 3.0.0)
project(wolf.content_pipeline.linux VERSION 1.68.0 DESCRIPTION "wolf.content_pipeline.linux.x64")

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
${CMAKE_CURRENT_SOURCE_DIR}/../../src/wolf.media_core/
${CMAKE_CURRENT_SOURCE_DIR}/../../deps/assimp/include/
)

add_library(wolf.content_pipeline.linux SHARED ./w_camera.cpp
./w_cpipeline_model.cpp
./w_cpipeline_pch.cpp
./w_cpipeline_scene.cpp
)

# pre processors
target_compile_definitions(wolf.content_pipeline.linux PUBLIC 
GLM_FORCE_DEPTH_ZERO_TO_ONE 
_GNU_SOURCE 
_POSIX_PTHREAD_SEMANTICS 
_REENTRANT 
_THREAD_SAFE
__WOLF_CONTENT_PIPELINE__
__linux
)

if (CMAKE_BUILD_TYPE MATCHES Debug)
target_compile_definitions(wolf.content_pipeline.linux PUBLIC _DEBUG DEBUG) 
endif()

# compiler options
target_compile_options(wolf.content_pipeline.linux PRIVATE -fPIC -m64)

# libs
link_directories(/usr/local/lib)
if (CMAKE_BUILD_TYPE MATCHES Debug)

target_link_libraries(wolf.content_pipeline.linux 
${CMAKE_CURRENT_SOURCE_DIR}/../../../bin/linux/x64/debug/libwolf.system.linux.so
${CMAKE_CURRENT_SOURCE_DIR}/../../deps/assimp/lib/linux/x64/debug/libassimp.so)
else()
target_link_libraries(wolf.content_pipeline.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../../bin/linux/x64/release/libwolf.system.linux.so
${CMAKE_CURRENT_SOURCE_DIR}/../../deps/assimp/lib/linux/x64/release/libassimp.so)
endif()

target_link_libraries(wolf.content_pipeline.linux anl rt nsl pthread avcodec avformat avutil swresample swscale)
