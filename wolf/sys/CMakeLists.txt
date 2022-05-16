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
option(WOLF_ENABLE_LZ4 "Enable LZ4 Compression" OFF)
option(WOLF_ENABLE_LZMA "Enable LZMA Compression" OFF)
option(WOLF_ENABLE_NNG "Enable nng socket for tcp/udp/ipc client/server" OFF)
option(WOLF_ENABLE_RIST "Enable RIST streaming protocol" OFF)
option(WOLF_ENABLE_RTSP "Enable real time streaming for client" OFF)
option(WOLF_ENABLE_SRT "Enable SRT streaming protocol" OFF)
option(WOLF_LINK_MIMALLOC "Enable SRT streaming protocol" OFF)

set(LIBRARY_TYPE "STATIC" CACHE STRING "Library type")

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
if (MSVC)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "/EHsc")
else()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-fexceptions")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# fetch gsl
message("fetching https://github.com/microsoft/GSL.git")
FetchContent_Declare(
  gsl
  GIT_REPOSITORY https://github.com/microsoft/GSL.git
  GIT_TAG        main
)
FetchContent_Populate(gsl)

set(INCLUDES ${CMAKE_CURRENT_SOURCE_DIR})

# fetch mimalloc
message("fetching https://github.com/microsoft/mimalloc.git")
FetchContent_Declare(
  mimalloc-static
  GIT_REPOSITORY https://github.com/microsoft/mimalloc.git
  GIT_TAG        master
)
list(APPEND INCLUDES
  ${CMAKE_CURRENT_BINARY_DIR}/_deps/gsl-src/include
  ${CMAKE_CURRENT_BINARY_DIR}/_deps/mimalloc-static-src/include
)
set(DEPS mimalloc-static)
set(SOURCES wolf.cpp)

# fetch lz4
if (WOLF_ENABLE_LZ4)
  message("fetching https://github.com/lz4/lz4.git")
  FetchContent_Declare(
    lz4
    GIT_REPOSITORY https://github.com/lz4/lz4.git
    GIT_TAG        dev
    SOURCE_SUBDIR  build/cmake
  )
  list(APPEND SOURCES
    compression/lz4.cpp
  )
  list(APPEND INCLUDES
    ${CMAKE_CURRENT_BINARY_DIR}/_deps/lz4-src/lib
  )
  list(APPEND DEPS lz4)
endif()

if (WOLF_ENABLE_RIST)
  execute_process(COMMAND chmod +x ${CMAKE_CURRENT_SOURCE_DIR}/shell/librist.sh)

  if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    add_custom_target(librist COMMAND /bin/sh -c "${CMAKE_CURRENT_SOURCE_DIR}/shell/librist.sh --debug")
  else()
    add_custom_target(librist COMMAND /bin/sh -c "${CMAKE_CURRENT_SOURCE_DIR}/shell/librist.sh")
  endif()

  list(APPEND SOURCES
    stream/rist.cpp
  )
  list(APPEND INCLUDES
    ${CMAKE_CURRENT_BINARY_DIR}/librist/contrib
    ${CMAKE_CURRENT_BINARY_DIR}/librist/contrib/mbedtls/include
    ${CMAKE_CURRENT_BINARY_DIR}/librist/build
    ${CMAKE_CURRENT_BINARY_DIR}/librist/build/include
    ${CMAKE_CURRENT_BINARY_DIR}/librist/build/include/librist
    ${CMAKE_CURRENT_BINARY_DIR}/librist/include
    ${CMAKE_CURRENT_BINARY_DIR}/librist/include/librist
    ${CMAKE_CURRENT_BINARY_DIR}/librist/src
  )
endif()

# apply some changes to CMake cache file
set(MI_BUILD_SHARED OFF CACHE BOOL "MI_BUILD_SHARED")
set(MI_BUILD_TESTS OFF CACHE BOOL "MI_BUILD_TESTS")

FetchContent_MakeAvailable(${DEPS})

# for rustc, we don't need any link
set(CMAKE_CXX_CLANG_TIDY
    clang-tidy;
    -format-style=file;)  

# set defenitions
if (MSVC)
  add_definitions(-D_WIN32)
elseif (APPLE)
  add_definitions(
    -DTARGET_IPHONE_SIMULATOR=0
    -DTARGET_OS_SIMULATOR=0
    -DTARGET_OS_IPHONE=0)
endif()
if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_definitions(-DDEBUG)
else()
  add_definitions(-DNDEBUG)
endif()

# add includes
include_directories(${INCLUDES})

# add libraries
add_library(${PROJECT_NAME} ${LIBRARY_TYPE} ${SOURCES})

# link mimalloc
if (WOLF_LINK_MIMALLOC)
  if (MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${mimalloc-static_BINARY_DIR}/libmimalloc-debug.lib)
  else()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${mimalloc-static_BINARY_DIR}/libmimalloc-debug.a)
  endif()
endif()

# link lz4
if (WOLF_ENABLE_LZ4)
  if (MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${lz4_BINARY_DIR}/${CMAKE_BUILD_TYPE}/liblz4.lib)
  else()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${lz4_BINARY_DIR}/liblz4.a)
  endif()
endif()

# link rist
if (WOLF_ENABLE_RIST)
  add_dependencies(${PROJECT_NAME} librist)
  if (MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/librist/build/librist.lib)
  else()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/librist/build/librist.a)
  endif()
endif()

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -fPIC -Wall)
# target_compile_definitions(${PROJECT_NAME} PRIVATE)

# build tests
if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Android")
  # add_executable (${TEST_PROJECT_NAME} test.cpp)
  # target_link_libraries(${TEST_PROJECT_NAME} PRIVATE ${PROJECT_NAME})
  # target_compile_options(${TEST_PROJECT_NAME} PRIVATE -Wall)
  # target_compile_definitions(${TEST_PROJECT_NAME} PRIVATE )
endif()