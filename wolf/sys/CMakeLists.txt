cmake_minimum_required(VERSION 3.22)

project(wolf_sys)
set(TEST_PROJECT_NAME "${PROJECT_NAME}_tests")
message("CXX Compiler ID is ${CMAKE_CXX_COMPILER_ID}")

# required packages
find_package(Git REQUIRED)
if (LINUX)
    find_package(Wget REQUIRED)
    message("make sure libtool was installed.(apt-get install libtool-bin)")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    if (WIN32)
        set(WIN64 TRUE)
    endif()
endif()

# FetchContent for cloning repositories, avaiable since CMAKE 3.11
include(FetchContent)

# CMAKE GUI Options
option(WOLF_STREAM_NNG "Enable nng socket for tcp/udp/ipc client/server" OFF)
option(WOLF_STREAM_RIST "Enable RIST streaming protocol" OFF)
option(WOLF_STREAM_RTSP "Enable real time streaming for client" OFF)
option(WOLF_STREAM_SRT "Enable SRT streaming protocol" OFF)
option(WOLF_STREAM_WEBRTC_CLIENT "Enable webRTC client" OFF)

option(WOLF_SYSTEM_LZ4 "Enable LZ4 Compression" OFF)
option(WOLF_SYSTEM_LZMA "Enable LZMA Compression" OFF)

option(WOLF_ENABLE_TESTS "Enable test for wolf_sys" ON)

set(LIBRARY_TYPE "SHARED" CACHE STRING "Library type")

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
if (MSVC)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_FLAGS "/EHsc /W4")
else()
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_FLAGS "-fcoroutines-ts -fexceptions")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

find_package(Threads REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# include sources
file(GLOB_RECURSE WOLF_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/wolf/wolf.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/wolf/wolf.cppm"
)
set(SRCS ${WOLF_SRCS})
set(INCLUDES 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/wolf/
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)
set(LIBS)

# fetch gsl
message("fetching https://github.com/microsoft/GSL.git")
FetchContent_Declare(
  gsl
  GIT_REPOSITORY https://github.com/microsoft/GSL.git
  GIT_TAG        main
  SOURCE_SUBDIR  include
)
FetchContent_Populate(gsl)
list(APPEND INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/_deps/gsl-src/include/)

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

# fetch mimalloc
message("fetching https://github.com/microsoft/mimalloc.git")
FetchContent_Declare(
    mimalloc-static
    GIT_REPOSITORY https://github.com/microsoft/mimalloc.git
    GIT_TAG        master
)
set(MI_BUILD_SHARED OFF CACHE BOOL "MI_BUILD_SHARED")
set(MI_BUILD_TESTS OFF CACHE BOOL "MI_BUILD_TESTS")

FetchContent_MakeAvailable(mimalloc-static)
list(APPEND INCLUDES ${mimalloc-static_SRC_DIR}/include)
list(APPEND LIBS mimalloc-static)    

# include sources
file(GLOB_RECURSE STREAM_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/stream/stream.cppm"
)
file(GLOB_RECURSE SYSTEM_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/system/system.cppm"
)
list(APPEND SRCS 
    ${STREAM_SRCS}
    ${SYSTEM_SRCS}
)

# fetch lz4
if (WOLF_SYSTEM_LZ4)
  message("fetching https://github.com/lz4/lz4.git")
  FetchContent_Declare(
    lz4_static
    GIT_REPOSITORY https://github.com/lz4/lz4.git
    GIT_TAG        dev
    SOURCE_SUBDIR  build/cmake
  )
  FetchContent_MakeAvailable(lz4_static)

  file(GLOB_RECURSE LZ4_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/system/lz4.cppm"
  )
  list(APPEND SRCS ${LZ4_SRCS})
  list(APPEND INCLUDES ${lz4_SRC_DIR}/include)
  list(APPEND LIBS lz4_static)    
endif()

if (WOLF_STREAM_RIST)
  list(APPEND SOURCES
    stream/rist.cpp
  )
  file(GLOB_RECURSE RIST_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/stream/rist.cppm"
    "${CMAKE_CURRENT_SOURCE_DIR}/stream/rist_data_block.cppm"
  )
  list(APPEND SRCS ${RIST_SRCS})
  list(APPEND INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/librist/src
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/librist/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/librist/include/librist
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/librist/include/contrib
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/librist/include/contrib/mbedtls/include
  )
  list(APPEND LIBS 
    ws2_32
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/librist/lib/windows/x64/${CMAKE_BUILD_TYPE}/librist.lib)  
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
source_group("stream" FILES 
    ${STREAM_SRCS}
)
source_group("system" FILES 
    ${SYSTEM_SRCS}
)

if (WOLF_STREAM_RIST)
    source_group("stream" FILES 
        ${RIST_SRCS}
    )
endif()
if (WOLF_SYSTEM_LZ4)
    source_group("system" FILES 
        ${LZ4_SRCS}
    )
endif()

if (NOT MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE -std=c++2b -fPIC -Wall)
endif()

# link mimalloc
# if (WOLF_LINK_MIMALLOC)
#   if (MSVC)
#     target_link_libraries(${PROJECT_NAME} PRIVATE ${mimalloc-static_BINARY_DIR}/libmimalloc-debug.lib)
#   else()
#     target_link_libraries(${PROJECT_NAME} PRIVATE ${mimalloc-static_BINARY_DIR}/libmimalloc-debug.a)
#   endif()
# endif()

# link lz4
# if (WOLF_ENABLE_LZ4)
#   if (MSVC)
#     target_link_libraries(${PROJECT_NAME} PRIVATE ${lz4_BINARY_DIR}/${CMAKE_BUILD_TYPE}/liblz4.lib)
#   else()
#     target_link_libraries(${PROJECT_NAME} PRIVATE ${lz4_BINARY_DIR}/liblz4.a)
#   endif()
# endif()

# link rist
# if (WOLF_ENABLE_RIST)
#   add_dependencies(${PROJECT_NAME} librist)
#   target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/librist/build/librist.a)
# endif()

# build tests
if (WOLF_ENABLE_TESTS)
  add_executable (${TEST_PROJECT_NAME} test.cpp)
  target_link_libraries(${TEST_PROJECT_NAME} PRIVATE ${PROJECT_NAME})
  if (MSVC)
    target_compile_options(${TEST_PROJECT_NAME} PRIVATE -std=c++2b)
  else()
    target_compile_options(${TEST_PROJECT_NAME} PRIVATE -Wall)
  endif()
endif()