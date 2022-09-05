cmake_minimum_required(VERSION 3.18)

project(wolf_sys)
set(TEST_PROJECT_NAME "${PROJECT_NAME}_tests")
message("CXX Compiler ID is ${CMAKE_CXX_COMPILER_ID}")

# required packages
find_package(Git REQUIRED)
if (LINUX)
    find_package(Wget REQUIRED)
    message("make sure libtool was installed.(apt-get install libtool-bin)")
endif()

# check for win64
if (WIN32)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(WIN64 TRUE)
  endif()
  set(LIB_EXT "lib")
else()
  set(LIB_EXT "a")
endif()

# FetchContent for cloning repositories, avaiable since CMAKE 3.11
include(FetchContent)

if (WIN32)
set(TARGET_OS "windows")
elseif(LINUX)
set(TARGET_OS "linux")
endif()

# CMAKE GUI Options
option(WOLF_MEDIA_FFMPEG "Enable FFMPEG for multimedia files" OFF)
option(WOLF_STREAM_RIST "Enable RIST streaming protocol" OFF)
option(WOLF_STREAM_RTSP_CLIENT "Enable real time streaming for client" OFF)

option(WOLF_SYSTEM_LZ4 "Enable LZ4 Compression" OFF)
option(WOLF_SYSTEM_LZMA "Enable LZMA Compression" OFF)
option(WOLF_SYSTEM_OPENAL "Enable openAL soft" OFF)

option(WOLF_ENABLE_TESTS "Enable test for wolf_sys" ON)

set(LIBRARY_TYPE "SHARED" CACHE STRING "Library type")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
if (MSVC)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_FLAGS "/EHsc /W4")
else()
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_FLAGS "-fexceptions")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

find_package(Threads REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# include sources
file(GLOB_RECURSE WOLF_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/dllmain.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/wolf/wolf.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/wolf/wolf.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/wolf/wolf.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/wolf/version.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/wolf/version.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/wolf/DISABLE_ANALYSIS_BEGIN"
    "${CMAKE_CURRENT_SOURCE_DIR}/wolf/DISABLE_ANALYSIS_END"
)
set(SRCS ${WOLF_SRCS})
set(INCLUDES 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/wolf/
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)
set(LIBS)

# include cmakes
include(cmake/system.cmake)
include(cmake/stream.cmake)
include(cmake/media.cmake)

# fetch catch2 for tests
if (WOLF_ENABLE_TESTS)
    message("fetching https://github.com/catchorg/Catch2.git")
    FetchContent_Declare(
      catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG        v2.x
    )
    FetchContent_Populate(catch2)
    list(APPEND INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/_deps/catch2-src/single_include/)
endif()

# set clang tidy
set(CMAKE_CXX_CLANG_TIDY
    clang-tidy;
    -format-style=file;)  

# set win32 & win64 defenitions
if (MSVC)
  add_definitions(-D_WIN32 -DWIN32)
  if (WIN64)
    add_definitions(-D_WIN64 -DWIN64)
  endif()
endif()

# set DEBUG/NDEBUG definition
if(CMAKE_BUILD_TYPE MATCHES Debug)
  add_definitions(-DDEBUG)
else()
  add_definitions(-DNDEBUG)
endif()

# set Wolf definitions
get_cmake_property(_vars VARIABLES)
foreach (_var ${_vars})
    string(FIND ${_var} "WOLF_" out)
    if(("${out}" EQUAL 0) AND ("(${${_var}}" MATCHES ON))
        add_definitions("-D${_var}")
    endif()
endforeach()

# add includes
include_directories(${INCLUDES})

# make library from sources
add_library(${PROJECT_NAME} ${LIBRARY_TYPE}
    ${SRCS}
)

# link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})

# create source groups
source_group("wolf" FILES 
    ${WOLF_SRCS}
)
source_group("media" FILES 
    ${FFMPEG_SRCS}
)
source_group("stream" FILES 
    ${RIST_SRCS}
)
source_group("system" FILES 
    ${LZ4_SRCS}
)

if (NOT MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE -std=c++20 -fPIC -Wall)
endif()

# build tests
if (WOLF_ENABLE_TESTS)
  add_executable (${TEST_PROJECT_NAME} test.cpp)
  target_link_libraries(${TEST_PROJECT_NAME} PRIVATE ${PROJECT_NAME})
  if (MSVC)
    target_compile_options(${TEST_PROJECT_NAME} PRIVATE -std=c++20)
  else()
    target_compile_options(${TEST_PROJECT_NAME} PRIVATE -Wall)
  endif()
endif()