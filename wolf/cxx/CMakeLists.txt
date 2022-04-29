# cmake . -B build -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=armeabi-v7a -DANDROID_NDK=$NDK -DANDROID_PLATFORM=android-21 -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a -DCMAKE_ANDROID_NDK=$NDK -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_SYSTEM_NAME=Android -DCMAKE_SYSTEM_VERSION=21 -DCMAKE_BUILD_TYPE=Debug -GNinja 

cmake_minimum_required(VERSION 3.22)

project(wolf_cxx)
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
option(WOLF_ENABLE_FFMPEG_RTSP_CLIENT "Enable real time streaming for client" OFF)
option(WOLF_ENABLE_LIVE555_RTSP_SERVER "Enable real time streaming for server" OFF)
option(WOLF_ENABLE_WEBRTC "Enable webRTC" OFF)
option(WOLF_ENABLE_WEBSOCKET_SERVER "Enable websocket" OFF)

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

# fetch gsl
message("fetching https://github.com/microsoft/GSL.git")
FetchContent_Declare(
  gsl
  GIT_REPOSITORY https://github.com/microsoft/GSL.git
  GIT_TAG        main
)
FetchContent_Populate(gsl)


set(INCLUDES ${CMAKE_CURRENT_SOURCE_DIR})
set(LIBS "")

# fetch mimalloc
message("fetching https://github.com/microsoft/mimalloc.git")
FetchContent_Declare(
  mimalloc
  GIT_REPOSITORY https://github.com/microsoft/mimalloc.git
  GIT_TAG        master
)
list(APPEND INCLUDES
  ${mimalloc_SOURCE_DIR}/include
)
list(APPEND LIBS mimalloc)

if (WOLF_ENABLE_LZ4)
  # fetch lz4
  message("fetching https://github.com/lz4/lz4.git")
    FetchContent_Declare(
    lz4
    GIT_REPOSITORY https://github.com/lz4/lz4.git
    GIT_TAG        dev
    SOURCE_SUBDIR  build/cmake
  )

  list(APPEND INCLUDES
    ${lz4_SOURCE_DIR}/lib
  )
  list(APPEND LIBS lz4)
endif()

FetchContent_MakeAvailable(${LIBS})

# set(CMAKE_CXX_CLANG_TIDY
#     clang-tidy;
#     -format-style=file;)  

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

add_library(${PROJECT_NAME} ${LIBRARY_TYPE}
    Wolf.cpp
)

# link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -fPIC -Wall)
#target_compile_definitions(${PROJECT_NAME} PRIVATE)