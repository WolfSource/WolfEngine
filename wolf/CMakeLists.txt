#cmake . -B build -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=armeabi-v7a -DANDROID_NDK=$NDK -DANDROID_PLATFORM=android-21 -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a -DCMAKE_ANDROID_NDK=$NDK -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_SYSTEM_NAME=Android -DCMAKE_SYSTEM_VERSION=21 -DCMAKE_BUILD_TYPE=Debug -GNinja

cmake_minimum_required(VERSION 3.22)

# set(VCPKG_ROOT_DIR $ENV{VCPKG_ROOT})
# set(VCPKG ${VCPKG_ROOT_DIR} CACHE STRING "path to the root folder of vcpkg")
# set(CMAKE_TOOLCHAIN_FILE "${VCPKG}/scripts/buildsystems/vcpkg.cmake")

set(WEBRTC_ROOT_DIR $ENV{WEBRTC_ROOT})
set(WEBRTC_SRC ${WEBRTC_ROOT_DIR} CACHE STRING "path to the root folder of webrtc src folder")

# set it to enviroment variable if it is not set
if ("${WEBRTC_SRC}" STREQUAL "")
  set(WEBRTC_SRC $ENV{WEBRTC_ROOT})
endif()

project(wolf)
set(TEST_PROJECT_NAME "${PROJECT_NAME}_tests")
message("CXX Compiler ID is ${CMAKE_CXX_COMPILER_ID}")

# required packages
find_package(Git REQUIRED)
if (LINUX)
    find_package(Wget REQUIRED)
    message("make sure libtool was installed.(apt-get install libtool-bin)")
endif()

# check for operating system
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    if (WIN32)
        set(WIN64 TRUE)
    endif()
endif()

if(UNIX AND NOT APPLE)
   set(LINUX TRUE)
endif()

# Wolf does not support Windows 32 bit anymore
if (MSVC AND NOT WIN64)
    message( FATAL_ERROR "Only Window 64 bit is supported" )
endif()

# set target os
if (WIN64)
    set(TARGET_OS "win")
    set(LIB_EXT "lib")
    set(SHARED_EXT "dll")
elseif(APPLE)
    set(TARGET_OS "mac")
    set(LIB_EXT "a")
    set(SHARED_EXT "dylib")
elseif(LINUX)
    set(TARGET_OS "linux")
    set(LIB_EXT "a")
    set(SHARED_EXT "so")
else()
    message( FATAL_ERROR "Unsuported OS, please open an issue at https://github.com/WolfEngine/WolfEngine" )
endif()

# use folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# FetchContent for cloning repositories, avaiable since CMAKE 3.11
include(FetchContent)

# CMAKE GUI Options
option(WOLF_TESTS "Enable tests" ON)

# media modules
# option(WOLF_MEDIA_AV1 "Enable AV1 video coding format" OFF)
# option(WOLF_MEDIA_IMAGE "Enable WebP/BMP/JPG/PNG formats" OFF)
# option(WOLF_MEDIA_SCREEN_CAPTURE "Enable screen capture" ON)
# option(WOLF_MEDIA_VPX "Enable VPX video coding format" OFF)

# option(WOLF_RENDER "Enable cross platform render based on BGFX" OFF)

# stream modules
# option(WOLF_STREAM_GRPC "Enable gRPC connection" OFF)
# option(WOLF_STREAM_KAFKA_CLIENT "Enable Kafka API connector" OFF)
# option(WOLF_STREAM_NNG "Enable nng socket for tcp/udp/ipc client/server" OFF)
option(WOLF_STREAM_QUIC "Enable QUIC protocol" ON)
option(WOLF_STREAM_RIST "Enable RIST streaming protocol" ON)
# option(WOLF_STREAM_RTSP_CLIENT "Enable RTSP for client" OFF)
# option(WOLF_STREAM_RTSP_SERVER "Enable RTSP for server" OFF)
option(WOLF_STREAM_HTTP_WS "Enable http 1.1/3 and websocket protocols" OFF)
option(WOLF_STREAM_WEBRTC "Enable native webRTC" OFF)

# system modules
# option(WOLF_SYSTEM_AVX2 "Enable avx2" OFF)
# option(WOLF_SYSTEM_AVX512 "Enable avx512" OFF)
# option(WOLF_SYSTEM_BROTLI "Enable brotli compression" OFF)
# option(WOLF_SYSTEM_LUA "Enable lua scripting" OFF)
# option(WOLF_SYSTEM_LZ4 "Enable LZ4 Compression" OFF)
# option(WOLF_SYSTEM_LZMA "Enable LZMA Compression" OFF)
# option(WOLF_SYSTEM_MIMALLOC "Enable Microsoft mimalloc" OFF)
# option(WOLF_SYSTEM_RAFT "Enable raft algorithm" OFF)

option(WOLF_SYSTEM_BORINGSSL "Enable BoringSSL" ON)
option(WOLF_SYSTEM_EXECUTION "Enable the prototype implementation of the C++26 sender/receiver async programming model based on libunifex" ON)
option(WOLF_SYSTEM_JSON "Enable Json based on RapidJson" ON)
option(WOLF_SYSTEM_LIBEVENT "Enable libevent2" ON)

# set type of library
set(LIBRARY_TYPE "SHARED" CACHE STRING "Library type")

# set C/CXX ios standards
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
  set(CMAKE_BUILD_TYPE "Debug")
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")

find_package (Threads)

if (MSVC)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "/EHsc /W3")

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
  SOURCE_SUBDIR  include
)
FetchContent_Populate(gsl)

# fetch expected
# message("fetching https://github.com/TartanLlama/expected.git")
# FetchContent_Declare(
#   expected
#   GIT_REPOSITORY https://github.com/TartanLlama/expected.git
#   GIT_TAG        master
#   SOURCE_SUBDIR  include
# )
# FetchContent_Populate(expected)

set(INCLUDES     
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${gsl_SOURCE_DIR}/include
  #${expected_SOURCE_DIR}/include
)
set(LIBS)

if (NOT EMSCRIPTEN)
    list(APPEND LIBS Threads::Threads)
endif()

if (WOLF_SYSTEM_EXECUTION)
    # fetch libunifex
    message("fetching https://github.com/facebookexperimental/libunifex.git")
    FetchContent_Declare(
        unifex
        GIT_REPOSITORY https://github.com/facebookexperimental/libunifex.git
        GIT_TAG        main
    )
    FetchContent_MakeAvailable(unifex)
    list(APPEND INCLUDES 
        ${unifex_SOURCE_DIR}/include
        ${unifex_BINARY_DIR}/include
    )
    list(APPEND LIBS unifex)
endif()

if (WOLF_SYSTEM_LIBEVENT)
   	# fetch libevent
    message("fetching https://github.com/libevent/libevent.git")
    FetchContent_Declare(
        libevent
        GIT_REPOSITORY https://github.com/libevent/libevent.git
        GIT_TAG        master
    )
    
    set(EVENT__DISABLE_MBEDTLS ON CACHE BOOL "EVENT_DISABLE_MBEDTLS")
    set(EVENT__DISABLE_OPENSSL ON CACHE BOOL "EVENT_DISABLE_OPENSSL")
    set(EVENT__DISABLE_SAMPLES ON CACHE BOOL "EVENT_DISABLE_SAMPLES")
    
    FetchContent_MakeAvailable(libevent)
    set (EVENT_INCLUDE_DIR ${libevent_SOURCE_DIR}/include)
    set (EVENT_LIB libevent)

    list(APPEND INCLUDES 
        ${EVENT_INCLUDE_DIR}
    )
    list(APPEND LIBS ${EVENT_LIB})
endif()

if (WOLF_SYSTEM_BORINGSSL)
   	# get boringSSL
    message("fetching https://github.com/google/boringssl")
    FetchContent_Declare(
        boringssl
        GIT_REPOSITORY https://github.com/google/boringssl
        GIT_TAG        master
    )
    FetchContent_MakeAvailable(boringssl)
	set_target_properties(
		all_tests boringssl_gtest boringssl_gtest_main bssl bssl_shim crypto crypto_test crypto_test_data
		decrepit decrepit_test fips_specific_tests_if_any fipsmodule global_target handshaker run_tests
		ssl ssl_test test_support_lib urandom_test
		PROPERTIES FOLDER "BoringSSL")
	
    set (GETOPT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/system)
    set (BORINGSSL_INCLUDE ${boringssl_SOURCE_DIR}/include)
    set (BORINGSSL_LIB_ssl ${boringssl_BINARY_DIR}/ssl/${CMAKE_BUILD_TYPE}/ssl.${LIB_EXT})
    set (BORINGSSL_LIB_crypto ${boringssl_BINARY_DIR}/crypto/${CMAKE_BUILD_TYPE}/crypto.${LIB_EXT})

    list(APPEND INCLUDES 
        ${BORINGSSL_INCLUDE}
    )
    list(APPEND LIBS ${BORINGSSL_LIB_ssl} ${BORINGSSL_LIB_crypto})
endif()

if (WOLF_STREAM_QUIC)
    if (NOT WOLF_SYSTEM_BORINGSSL)
        message( FATAL_ERROR "'WOLF_STREAM_QUIC' needs 'WOLF_SYSTEM_BORINGSSL' = ON" )
    endif()

    # get quic
    message("fetching https://github.com/litespeedtech/lsquic.git")
    FetchContent_Declare(
        quic
        GIT_REPOSITORY https://github.com/litespeedtech/lsquic.git
        GIT_TAG        master
    )
    
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set(LSQUIC_DEVEL ON CACHE BOOL "LSQUIC_DEVEL")
    else()
        set(LSQUIC_DEVEL OFF CACHE BOOL "LSQUIC_DEVEL")
    endif()

    set(LSQUIC_SHARED_LIB OFF CACHE BOOL "LSQUIC_SHARED_LIB")
    set(LSQUIC_TESTS OFF CACHE BOOL "LSQUIC_TESTS")
    
    FetchContent_MakeAvailable(quic)
	
    list(APPEND INCLUDES 
        ${quic_SOURCE_DIR}/include
        ${quic_BINARY_DIR}/include
    )
    list(APPEND LIBS quic)
endif()

if (WOLF_SYSTEM_MIMALLOC)
    message("fetching https://github.com/microsoft/mimalloc.git")
    FetchContent_Declare(
        mimalloc-static
        GIT_REPOSITORY https://github.com/microsoft/mimalloc.git
        GIT_TAG        master
    )
    FetchContent_MakeAvailable(mimalloc-static)
    list(APPEND INCLUDES
        ${mimalloc_static_SOURCE_DIR}/include
    )
    list(APPEND LIBS mimalloc-static)
endif()

if (WOLF_STREAM_WEBRTC)
    # we need http & json for webrtc
    
    if (NOT WOLF_SYSTEM_JSON)
        message( FATAL_ERROR "'WOLF_STREAM_WEBRTC' needs 'WOLF_SYSTEM_JSON' = ON" )
    endif()

    if (NOT WOLF_STREAM_HTTP)
        message( FATAL_ERROR "'WOLF_STREAM_WEBRTC' needs 'WOLF_STREAM_HTTP' = ON" )
    endif()
   
    list(APPEND INCLUDES
        ${WEBRTC_SRC}
        ${WEBRTC_SRC}/third_party/abseil-cpp
        ${WEBRTC_SRC}/third_party/libyuv/include
    )
    if (WIN32)
        # enable/disable debug option
        if(CMAKE_BUILD_TYPE MATCHES Debug)
            add_definitions(
                -D_HAS_ITERATOR_DEBUGGING=1
                -D_ITERATOR_DEBUG_LEVEL=2
            )
        else()
            add_definitions(
                -D_HAS_ITERATOR_DEBUGGING=0
                -D_ITERATOR_DEBUG_LEVEL=0
            )
        endif()

        add_definitions(
            -DWEBRTC_WIN 
            -D__PRETTY_FUNCTION__=__FUNCTION__
            #-DUSE_X11 
            #-D_WINSOCKAPI_
            -DHAVE_SOUND) 

        list(APPEND LIBS
            secur32 
            dmoguids 
            wmcodecdspuuid 
            strmiids 
            msdmo 
            winmm 
            dxgi 
            d3d11 
            iphlpapi 
            dwmapi
        )
    elseif (APPLE)

        add_definitions(
            -DHAVE_SOUND 
            -DWEBRTC_MAC 
            -DWEBRTC_POSIX 
            -fno-rtti)
        
        find_library(FOUNDATION Foundation)
        find_library(CORE_FOUNDATION CoreFoundation)
        find_library(APPLICATION_SERVICES ApplicationServices)
        find_library(CORE_SERVICES CoreServices)
        find_library(CORE_AUDIO CoreAudio)
        find_library(AUDIO_TOOLBOX AudioToolBox)

        list(APPEND LIBS
            ${FOUNDATION} 
            ${CORE_FOUNDATION} 
            ${APPLICATION_SERVICES} 
            ${CORE_SERVICES}
            ${CORE_AUDIO} 
            ${AUDIO_TOOLBOX} 
        )
    endif()
    add_definitions(-DHAVE_JPEG)
    link_directories(${WEBRTC_SRC}/out/${TARGET_OS}/${CMAKE_BUILD_TYPE}/obj/)
    list(APPEND LIBS webrtc)
endif()

if (WOLF_STREAM_HTTP)
    message("fetching https://github.com/civetweb/civetweb.git")
    FetchContent_Declare(
        civetweb
        GIT_REPOSITORY https://github.com/civetweb/civetweb.git
        GIT_TAG        master
    )
    set(BUILD_TESTING OFF CACHE BOOL "BUILD_TESTING")
    set(CIVETWEB_BUILD_TESTING OFF CACHE BOOL "CIVETWEB_BUILD_TESTING")
    set(CIVETWEB_ENABLE_CXX ON CACHE BOOL "CIVETWEB_ENABLE_CXX")
    set(CIVETWEB_ENABLE_ASAN OFF CACHE BOOL "CIVETWEB_ENABLE_ASAN")

    FetchContent_MakeAvailable(civetweb)

    list(APPEND INCLUDES
        ${civetweb_SOURCE_DIR}/include
    )
    link_directories(${civetweb_BINARY_DIR}/src/)
    list(APPEND LIBS civetweb-cpp)
    #add_definitions(-DOPENSSL_API_1_1 -DUSE_WEBSOCKET)
endif()

if (WOLF_SYSTEM_JSON)
    message("fetching https://github.com/Tencent/rapidjson.git")
    FetchContent_Declare(
        rapidjson
        GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
        GIT_TAG        master
    )
    FetchContent_Populate(rapidjson)

    list(APPEND INCLUDES
        ${rapidjson_SOURCE_DIR}/include
    )
endif()

# fetch catch2 for tests
if (WOLF_TESTS)
    message("fetching https://github.com/catchorg/Catch2.git")
    FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.x
    )
    FetchContent_Populate(catch2)
    list(APPEND INCLUDES
        ${catch2_SOURCE_DIR}/single_include
    )
endif()

# enabling clang-tidy
# can be enabled with .CLANG-TIDY from Visual Studio Code
# https://devblogs.microsoft.com/cppblog/visual-studio-code-c-december-2021-update-clang-tidy/
# can be enabled with .CLANG-TIDY from Visual Studio
# https://devblogs.microsoft.com/cppblog/code-analysis-with-clang-tidy-in-visual-studio/
# set(CMAKE_CXX_CLANG_TIDY
#     clang-tidy;
#     -format-style=file;)  

# add definitions
if (MSVC)
    add_definitions(
        -EHsc 
        -DNOMINMAX
        -DWIN64
    )
elseif(APPLE)
    add_definitions(-DNEED_XLOCALE_H=1)
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG -D_DEBUG)
else()
    add_definitions(-DNDEBUG)
endif()

# setup Wolf definitions
get_cmake_property(_vars VARIABLES)
foreach (_var ${_vars})
    string(FIND ${_var} "WOLF_" out)
    if(("${out}" EQUAL 0) AND ("(${${_var}}" MATCHES ON))
        add_definitions("-D${_var}")
    endif()
endforeach()

# include sources
file(GLOB_RECURSE WOLF_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/dllmain.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/getopt.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/wolf.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/wolf.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/DISABLE_ANALYSIS_BEGIN"
    "${CMAKE_CURRENT_SOURCE_DIR}/DISABLE_ANALYSIS_END"
)
file(GLOB_RECURSE WOLF_STREAM_WEBRTC_CAPTURER_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/stream/webrtc/capturer/*"
)
file(GLOB_RECURSE WOLF_STREAM_WEBRTC_DATA_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/stream/webrtc/data/*"
)
file(GLOB_RECURSE WOLF_STREAM_WEBRTC_INTERCEPTOR_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/stream/webrtc/interceptor/*"
)
file(GLOB_RECURSE WOLF_STREAM_WEBRTC_MEDIA_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/stream/webrtc/media/*"
)
file(GLOB_RECURSE WOLF_STREAM_WEBRTC_PEER_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/stream/webrtc/peer/*"
)
file(GLOB_RECURSE WOLF_STREAM_QUIC_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/stream/quic/*"
)
file(GLOB_RECURSE WOLF_SYSTEM_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/system/*"
)

# includes
include_directories(${INCLUDES})

# add source codes
add_library(${PROJECT_NAME} ${LIBRARY_TYPE}
    ${WOLF_SRCS}
	${WOLF_STREAM_QUIC_SRC}
    ${WOLF_STREAM_WEBRTC_CAPTURER_SRC}
    ${WOLF_STREAM_WEBRTC_DATA_SRC}
    ${WOLF_STREAM_WEBRTC_INTERCEPTOR_SRC}
    ${WOLF_STREAM_WEBRTC_MEDIA_SRC}
    ${WOLF_STREAM_WEBRTC_PEER_SRC}
    ${WOLF_SYSTEM_SRC}
)
		
if (MSVC)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
    else()
        set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
    endif()
endif()

# link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})

# create source group
source_group("wolf" FILES ${WOLF_SRCS})
source_group("stream/quic" FILES ${WOLF_STREAM_QUIC_SRC})
source_group("stream/webrtc/capturer" FILES ${WOLF_STREAM_WEBRTC_CAPTURER_SRC})
source_group("stream/webrtc/data" FILES ${WOLF_STREAM_WEBRTC_DATA_SRC})
source_group("stream/webrtc/interceptor" FILES ${WOLF_STREAM_WEBRTC_INTERCEPTOR_SRC})
source_group("stream/webrtc/media" FILES ${WOLF_STREAM_WEBRTC_MEDIA_SRC})
source_group("stream/webrtc/peer" FILES ${WOLF_STREAM_WEBRTC_PEER_SRC})
source_group("system" FILES ${WOLF_SYSTEM_SRC})

# add compile options
if (NOT WIN32)
    target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)# -Wall)
endif()

# build tests
if (WOLF_TESTS)
    add_executable (${TEST_PROJECT_NAME} tests.cpp)
    link_directories(${CMAKE_BINARY_DIR})
    target_link_libraries(${TEST_PROJECT_NAME} PRIVATE ${PROJECT_NAME})
    if (WOLF_SYSTEM_EXECUTION)
        target_link_libraries(${TEST_PROJECT_NAME} PRIVATE unifex)
    endif()
    if (NOT EMSCRIPTEN)
        target_link_libraries(${TEST_PROJECT_NAME} PRIVATE Threads::Threads)
    endif()
    target_compile_options(${TEST_PROJECT_NAME} PRIVATE -std=c++2b -Wall)
endif()