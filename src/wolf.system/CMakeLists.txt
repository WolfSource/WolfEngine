# CMAKE required version
cmake_minimum_required (VERSION 3.11)

if(APPLE)
    set(TARGET "OSX" CACHE STRING "target platform")
    set(ARCH "x86_64" CACHE STRING "target architecture")
endif()

set(LIBRARY_TYPE "SHARED" CACHE STRING "Library type")

if("${TARGET}" STREQUAL "IOS")
    set(IOS TRUE)
endif()													
if(UNIX AND NOT APPLE AND NOT IOS)
   set(LINUX TRUE)
endif()

# set the project name
if (IOS)
    project (wolf_system_ios)						 
elseif (APPLE)
	project (wolf_system_osx)
elseif (WIN32)
	project (wolf_system_win)
elseif (LINUX)
	project (wolf_system_linux)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    if (WIN32)
        set(WIN64 TRUE)
    endif()
endif()

if (IOS)
    SET(PREFIX_LIB_PATH "ios")
    if(NOT ${DEVELOPMENT_TEAM_ID})
        set (DEVELOPMENT_TEAM_ID "")# set your dev id for signing package
    endif()  
elseif(APPLE)
	SET(PREFIX_LIB_PATH "osx")
elseif(WIN64)
	SET(PREFIX_LIB_PATH "win/x64")
elseif(WIN32)
	SET(PREFIX_LIB_PATH "win/x86")
elseif(LINUX)
	SET(PREFIX_LIB_PATH "linux")
endif()

# required packages
find_package(Git REQUIRED)
if (LINUX)
    find_package(Wget REQUIRED)
    message("make sure libtool was installed.(apt-get install libtool-bin)")
endif()
# FetchContent for cloning repositories, avaiable since CMAKE 3.11
include(FetchContent)

# CMAKE GUI Options
option(WOLF_ENABLE_ACTIVEMQ "Enable ActiveMQ" OFF)
option(WOLF_ENABLE_AVX2 "Enable AVX2" OFF)
option(WOLF_ENABLE_AVX512 "Enable AVX512" OFF)
option(WOLF_ENABLE_CXX_EXTENSIONS "Enable CXX Extensions" OFF)
option(WOLF_ENABLE_CURL "Enable CURL" OFF)
option(WOLF_ENABLE_FIBER "Enable Fiber" OFF)
option(WOLF_ENABLE_HTTP1_1_WS "Enable HTTP 1.1 and WebSocket" OFF)
option(WOLF_ENABLE_LZ4 "Enable LZ4 Compression" OFF)
option(WOLF_ENABLE_LZMA "Enable LZMA Compression" OFF)
option(WOLF_ENABLE_LUA "Enable Lua Scripting" OFF)
option(WOLF_ENABLE_PYTHON3 "Enable Python3 Scripting" OFF)
option(WOLF_ENABLE_QUIC "Enable QUIC protocol" OFF)
#option(WOLF_ENABLE_SSL "Enable WOLF SSL" OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ENABLE_CXX_EXTENSIONS)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
#set(CMAKE_CXX17_STANDARD_COMPILE_OPTION "-std=c++17")
#set(CMAKE_CXX17_EXTENSION_COMPILE_OPTION "-std=gnu++17")

if (WOLF_ENABLE_AVX2)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx -march=native")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx -march=native")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set (LIBRARY_TYPE "SHARED")

# fetch libexpat
message("fetching https://github.com/libexpat/libexpat.git")
FetchContent_Declare(
  libexpat
  GIT_REPOSITORY https://github.com/libexpat/libexpat.git
  GIT_TAG        master
  SOURCE_SUBDIR  expat
)

if(WIN32 OR WIN64)
    # fetch apr
    message("fetching https://github.com/apache/apr.git")
    FetchContent_Declare(
      apr
      GIT_REPOSITORY https://github.com/apache/apr.git
      GIT_TAG        trunk
    )
endif()

if(WOLF_ENABLE_CURL)
    message("fetching https://github.com/curl/curl.git")
    FetchContent_Declare(
        libcurl
        GIT_REPOSITORY https://github.com/curl/curl.git
        GIT_TAG        master
    )
endif()

if(WOLF_ENABLE_LUA)
    message("fetching https://github.com/torch/luajit-rocks.git")
    FetchContent_Declare(
        luajit
        GIT_REPOSITORY https://github.com/torch/luajit-rocks.git
        GIT_TAG        master
    )
endif()

if(WOLF_ENABLE_HTTP1_1_WS)
    message("fetching https://github.com/madler/zlib.git")
    FetchContent_Declare(
        zlib
        GIT_REPOSITORY https://github.com/madler/zlib.git
        GIT_TAG        master
    )
    message("fetching https://github.com/libuv/libuv.git")
    FetchContent_Declare(
        libuv
        GIT_REPOSITORY https://github.com/libuv/libuv.git
        GIT_TAG        master
    )
endif()

# we need wolf ssl for uSocket and apr socket
#if(WOLF_ENABLE_SSL)
#    message("fetching https://github.com/wolfSSL/wolfssl.git")
#    FetchContent_Declare(
#        wolfssl
#        GIT_REPOSITORY https://github.com/wolfSSL/wolfssl.git
#        GIT_TAG        master
#    )
#endif()

# change some options of CMAKE dependencies
set(EXPAT_SHARED_LIBS OFF CACHE BOOL "EXPAT_SHARED_LIBS")
set(EXPAT_BUILD_TESTS OFF CACHE BOOL "EXPAT_BUILD_TESTS")
set(EXPAT_BUILD_TOOLS OFF CACHE BOOL "EXPAT_BUILD_TOOLS")
set(CMAKE_DEBUG_POSTFIX "" CACHE STRING "" FORCE)
set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "" FORCE)
set(CMAKE_MINSIZEREL_POSTFIX "" CACHE STRING "" FORCE)
set(CMAKE_RELWITHDEBINFO_POSTFIX "" CACHE STRING "" FORCE)
set(BUILD_TESTING  OFF CACHE BOOL "BUILD_TESTING")

# change some options of CURL
if(WOLF_ENABLE_CURL)
    set(BUILD_CURL_EXE OFF CACHE BOOL "BUILD_CURL_EXE")
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "BUILD_SHARED_LIBS")
endif()

# change some options of LUAJIT
if(WOLF_ENABLE_LUA)
    set(WITH_LUAJIT21 ON CACHE BOOL "WITH_LUAJIT21")
endif()

# change some options of WOLFSSL
if(WOLF_ENABLE_SSL)
    set(WOLFSSL_EXAMPLES "no" CACHE STRING "WOLFSSL_EXAMPLES")
    set(WOLFSSL_CRYPT_TESTS "no" CACHE STRING "WOLFSSL_CRYPT_TESTS")
endif()

if(LINUX)
    set(CMAKE_C_FLAGS "-fPIC" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS "-fPIC" CACHE STRING "" FORCE)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif()

# After the following call, the CMake targets will be defined and available to the rest of the build
#if(WOLF_ENABLE_SSL)
#    FetchContent_MakeAvailable(wolfssl)
#endif()

if (UNIX)
    FetchContent_MakeAvailable(libexpat)

    if (WOLF_ENABLE_ACTIVEMQ)
        # build activeMQ via shell script
        execute_process(COMMAND chmod +x ${CMAKE_CURRENT_SOURCE_DIR}/shell/amq.sh)
        add_custom_target(libamq_target COMMAND /bin/sh -c
        "${CMAKE_CURRENT_SOURCE_DIR}/shell/amq.sh --build_dir=${CMAKE_CURRENT_BINARY_DIR} --${CMAKE_BUILD_TYPE}")
    endif()
	
    if(IOS)
        # build apr via shell script
        execute_process(COMMAND chmod +x ${CMAKE_CURRENT_SOURCE_DIR}/shell/apr.sh)
        add_custom_target(libapr_target COMMAND /bin/sh -c
        "${CMAKE_CURRENT_SOURCE_DIR}/shell/apr.sh --bypass_linux=1 --bypass_macos=1 --bypass_ios_simulator=1 --build_dir=${CMAKE_CURRENT_BINARY_DIR} --arch=${ARCH}  --${CMAKE_BUILD_TYPE}")
        
        add_custom_command(TARGET libapr_target PRE_BUILD COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/_deps/libexpat-build/Debug-iphoneos ${CMAKE_CURRENT_BINARY_DIR}/_deps/libexpat-build/Debug)
    elseif(APPLE)
        # build apr via shell script
        execute_process(COMMAND chmod +x ${CMAKE_CURRENT_SOURCE_DIR}/shell/apr.sh)
        add_custom_target(libapr_target COMMAND /bin/sh -c
        "${CMAKE_CURRENT_SOURCE_DIR}/shell/apr.sh --bypass_linux=1 --bypass_ios=1 --bypass_ios_simulator=1 --arch=${CMAKE_SYSTEM_PROCESSOR} --build_dir=${CMAKE_CURRENT_BINARY_DIR} --${CMAKE_BUILD_TYPE}")
    elseif(LINUX)
        # build apr via shell script
        execute_process(COMMAND chmod +x ${CMAKE_CURRENT_SOURCE_DIR}/shell/apr.sh)
        add_custom_target(libapr_target COMMAND /bin/sh -c
        "${CMAKE_CURRENT_SOURCE_DIR}/shell/apr.sh --bypass_ios=1 --bypass_ios_simulator=1 --bypass_macos=1  --arch=${CMAKE_SYSTEM_PROCESSOR} --build_dir=${CMAKE_CURRENT_BINARY_DIR} --${CMAKE_BUILD_TYPE}")
    endif()
else()
    FetchContent_MakeAvailable(libexpat    apr)
endif()

# fetch curl
if(WOLF_ENABLE_CURL)
    FetchContent_MakeAvailable(libcurl)
endif()

#fetch lua
if(WOLF_ENABLE_LUA)
    FetchContent_MakeAvailable(luajit)
endif()
 
#fetch websocket
if(WOLF_ENABLE_HTTP1_1_WS)
    FetchContent_MakeAvailable(zlib libuv)
endif()

if(IOS)
    set(CMAKE_OSX_SYSROOT iphoneos)
    set(CMAKE_SYSTEM_NAME iOS)
    set(CMAKE_OSX_ARCHITECTURES ${ARCH})
endif()

# create filter for each folder
if(NOT IOS)
file(GLOB_RECURSE W_BASE64_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_base64/*"
)
endif()

file(GLOB_RECURSE W_CHRONO_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_chrono/*"
)
file(GLOB_RECURSE W_COMPRESSION_LZ4_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_compression/lz4/*"
)

if(MSVC)
file(GLOB_RECURSE W_COMPRESSION_LZMA_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_compression/lzma/*"
)
endif()

file(GLOB_RECURSE W_COMPRESSION_MSGPACK_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_compression/msgpack/msgpack.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_compression/msgpack/objectc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_compression/msgpack/unpack.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_compression/msgpack/version.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_compression/msgpack/vrefbuffer.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_compression/msgpack/zone.c"
)
file(GLOB_RECURSE W_COMPRESSION_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_compression/w_compress.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_compression/w_compress.h"
)
file(GLOB_RECURSE W_LIBEV_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_concurrency/libev/ev.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_concurrency/libev/ev.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_concurrency/libev/event.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_concurrency/libev/event.h"
)
file(GLOB_RECURSE W_CONCURRENCY_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_concurrency/w_atomic.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_concurrency/w_atomic.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_concurrency/w_concurrent_queue.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_concurrency/w_concurrent_queue.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_concurrency/w_condition_variable.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_concurrency/w_condition_variable.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_concurrency/w_mutex.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_concurrency/w_mutex.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_concurrency/w_signal.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_concurrency/w_signal.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_concurrency/w_sigslot.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_concurrency/w_sigslot.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_concurrency/w_thread.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_concurrency/w_thread.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_concurrency/w_thread_pool.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_concurrency/w_thread_pool.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_concurrency/w_timer.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_concurrency/w_timer.h"
)
file(GLOB_RECURSE W_FIBER_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_fiber/*"
)
file(GLOB_RECURSE W_IO_STB_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_io/stb/*"
)
file(GLOB_RECURSE W_IO_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_io/w_io.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_io/w_io.h"
)
file(GLOB_RECURSE W_JSON_RAPIDJSON_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_json/rapidjson/*"
)
file(GLOB_RECURSE W_LOG_SPDLOG_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_log/spdlog/src/async.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_log/spdlog/src/cfg.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_log/spdlog/src/color_sinks.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_log/spdlog/src/file_sinks.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_log/spdlog/src/fmt.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_log/spdlog/src/spdlog.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_log/spdlog/src/stdout_sinks.cpp"
)
file(GLOB_RECURSE W_LOG_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_log/logger.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_log/logger.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_log/w_log.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_log/w_log.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_log/w_log_config.h"
)
file(GLOB_RECURSE W_MEMORY_BUCKETS_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_memory/buckets/*"
)
file(GLOB_RECURSE W_MEMORY_HASH_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_memory/hash/*"
)
file(GLOB_RECURSE W_MEMORY_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_memory/w_array.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_memory/w_array.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_memory/w_hash.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_memory/w_hash.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_memory/w_mem_cache.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_memory/w_mem_cache.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_memory/w_mem_map.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_memory/w_mem_map.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_memory/w_mem_pool.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_memory/w_mem_pool.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_memory/w_shared_mem.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_memory/w_shared_mem.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_memory/w_string.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_memory/w_string.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_memory/w_table.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_memory/w_table.h"
)
file(GLOB_RECURSE W_NET_AMQ_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/amq/*"
)
file(GLOB_RECURSE W_NET_ASIO_IMP_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/asio/imp/round_robin.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/w_net/asio/imp/yield.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/w_net/asio/imp/yield_imp.hpp"
)
file(GLOB_RECURSE W_NET_ASIO_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/w_net/asio/fiber_client_server.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/w_net/asio/fiber_client_server.h"
)
file(GLOB_RECURSE W_NET_HTTP1_1_HELPERS_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/w_net/http/helpers/AsyncFileReader.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/w_net/http/helpers/AsyncFileStreamer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/http/helpers/Middleware.hpp"
)
file(GLOB_RECURSE W_NET_HTTP1_1_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/w_net/http/http.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/http/http.h"
)
file(GLOB_RECURSE W_NET_WS_USOCKETS_CRYPTO_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/uSockets/crypto/*"
)
file(GLOB_RECURSE W_NET_WS_USOCKETS_EVENTING_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/uSockets/eventing/*"
)
file(GLOB_RECURSE W_NET_WS_USOCKETS_INTERNAL_EVENTING_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/uSockets/internal/eventing/epoll_kqueue.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/uSockets/internal/eventing/gcd.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/uSockets/internal/eventing/libuv.h"
)
file(GLOB_RECURSE W_NET_WS_USOCKETS_INTERNAL_NETWORKING_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/uSockets/internal/networking/bsd.h"
)
file(GLOB_RECURSE W_NET_WS_USOCKETS_INTERNAL_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/uSockets/internal/internal.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/uSockets/internal/loop_data.h"
)
file(GLOB_RECURSE W_NET_WS_USOCKETS_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/uSockets/bsd.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/uSockets/context.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/uSockets/libusockets.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/uSockets/loop.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/uSockets/socket.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/uSockets/verson.txt"
)
file(GLOB_RECURSE W_NET_WS_UWEBSOCKETS_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/ws/uWebSockets/*"
)
file(GLOB_RECURSE W_NET_WS_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/ws/uws.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/ws/uws.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/ws/ws.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/ws/ws.h"
)
file(GLOB_RECURSE W_NET_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/test.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/w_net.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_net/w_net.h"
)
file(GLOB_RECURSE W_OS_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_os/*"
)
file(GLOB_RECURSE W_SCRIPT_LUA_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_script/lua/*"
)
file(GLOB_RECURSE W_SCRIPT_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_script/w_lua.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_script/w_lua.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_script/w_python.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_script/w_python.h"
)
file(GLOB_RECURSE W_TEST_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_test/base.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/w_test/catch.hpp"
)
file(GLOB_RECURSE WOLF_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/wolf.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/wolf.h"
)

# add includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/_deps/libexpat-src/expat/lib
    ${CMAKE_CURRENT_BINARY_DIR}/_deps/apr-build
    ${CMAKE_CURRENT_BINARY_DIR}/_deps/apr-src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/w_log
    ${CMAKE_CURRENT_SOURCE_DIR}/w_compression/msgpack
)

if (WOLF_ENABLE_ACTIVEMQ)
    if (MSVC)
        include_directories(${CMAKE_CURRENT_SOURCE_DIR}/_deps/activemq-cpp/include)
    else()
        include_directories(${CMAKE_CURRENT_BINARY_DIR}/_deps/activemq-cpp-build/${CMAKE_BUILD_TYPE}/include/activemq-cpp-3.10.0)
    endif()
endif()

if (WOLF_ENABLE_CURL)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/_deps/curl-build)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/_deps/curl-src/include)
endif()

if (WOLF_ENABLE_FIBER)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/include/)
endif()

if (WOLF_ENABLE_QUIC)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/_deps/quiche/include/)
endif()

if(WOLF_ENABLE_HTTP1_1_WS)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/w_net/uSockets)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/_deps/openSSL/include)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/_deps/zlib-src)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/_deps/zlib-build)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/_deps/libuv-src/include)
endif()

#additional library search path
#link_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/)

# add sources
add_library(${PROJECT_NAME} ${LIBRARY_TYPE}
    ${W_BASE64_SRC_DIR}
    ${W_CHRONO_SRC_DIR}
    ${W_COMPRESSION_LZ4_SRC_DIR}
    ${W_COMPRESSION_LZMA_SRC_DIR}
    ${W_COMPRESSION_MSGPACK_SRC_DIR}
    ${W_COMPRESSION_SRC_DIR}
    ${W_LIBEV_SRC_DIR}
    ${W_CONCURRENCY_SRC_DIR}
    ${W_FIBER_SRC_DIR}
    ${W_IO_STB_SRC_DIR}
    ${W_IO_SRC_DIR}
    ${W_JSON_RAPIDJSON_SRC_DIR}
    ${W_LOG_SPDLOG_SRC_DIR}
    ${W_LOG_SRC_DIR}
    ${W_MEMORY_BUCKETS_SRC_DIR}
    ${W_MEMORY_HASH_SRC_DIR}
    ${W_MEMORY_SRC_DIR}
    ${W_NET_AMQ_SRC_DIR}
    ${W_NET_ASIO_IMP_SRC_DIR}
    ${W_NET_ASIO_SRC_DIR}
    ${W_NET_HTTP1_1_HELPERS_SRC_DIR}
    ${W_NET_HTTP1_1_SRC_DIR}
    ${W_NET_WS_USOCKETS_CRYPTO_SRC_DIR}
    ${W_NET_WS_USOCKETS_EVENTING_SRC_DIR}
    ${W_NET_WS_USOCKETS_INTERNAL_EVENTING_SRC_DIR}
    ${W_NET_WS_USOCKETS_INTERNAL_NETWORKING_SRC_DIR}
    ${W_NET_WS_USOCKETS_INTERNAL_SRC_DIR}
    ${W_NET_WS_USOCKETS_SRC_DIR}
    ${W_NET_WS_UWEBSOCKETS_SRC_DIR}
    ${W_NET_WS_SRC_DIR}
    ${W_NET_SRC_DIR}
    ${W_OS_SRC_DIR}
    ${W_SCRIPT_LUA_SRC_DIR}
    ${W_SCRIPT_SRC_DIR}
    ${W_TEST_SRC_DIR}
    ${WOLF_SRC_DIR}
 )
#target_compile_features(${PROJECT_NAME} PUBLIC cxx_alias_templates)
if(UNIX)
    target_compile_options(${PROJECT_NAME} PUBLIC -std=c++20)
endif()
#target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

if (UNIX)
    if (WOLF_ENABLE_ACTIVEMQ)
        add_dependencies(${PROJECT_NAME} libamq_target)
    endif()
    add_dependencies(${PROJECT_NAME} libapr_target)
endif()

 source_group("w_base64" FILES ${W_BASE64_SRC_DIR})
 source_group("w_chrono" FILES ${W_CHRONO_SRC_DIR})
 source_group("w_compression/lz4" FILES ${W_COMPRESSION_LZ4_SRC_DIR})
 if (MSVC)
 source_group("w_compression/lzma" FILES ${W_COMPRESSION_LZMA_SRC_DIR})
 endif()
 source_group("w_compression/msgpack" FILES ${W_COMPRESSION_MSGPACK_SRC_DIR})
 source_group("w_compression" FILES ${W_COMPRESSION_SRC_DIR})
 source_group("w_concurrency/libev" FILES ${W_LIBEV_SRC_DIR})
 source_group("w_concurrency" FILES ${W_CONCURRENCY_SRC_DIR})
 source_group("w_fiber" FILES ${W_FIBER_SRC_DIR})
 source_group("w_io/stb" FILES ${W_IO_STB_SRC_DIR})
 source_group("w_io" FILES ${W_IO_SRC_DIR})
 source_group("w_json/rapidjson" FILES ${W_JSON_RAPIDJSON_SRC_DIR})
 source_group("w_log/spdlog/src" FILES ${W_LOG_SPDLOG_SRC_DIR})
 source_group("w_log" FILES ${W_LOG_SRC_DIR})
 source_group("w_memory/buckets" FILES ${W_MEMORY_BUCKETS_SRC_DIR})
 source_group("w_memory/hash" FILES ${W_MEMORY_HASH_SRC_DIR})
 source_group("w_memory" FILES ${W_MEMORY_SRC_DIR})
 source_group("w_net/amq" FILES ${W_NET_AMQ_SRC_DIR})
 source_group("w_net/asio/imp" FILES ${W_NET_ASIO_IMP_SRC_DIR})
 source_group("w_net/asio" FILES ${W_NET_ASIO_SRC_DIR})
 source_group("w_net/http/helpers" FILES ${W_NET_HTTP1_1_HELPERS_SRC_DIR})
 source_group("w_net/http" FILES ${W_NET_HTTP1_1_SRC_DIR})
 source_group("w_net/uSockets/crypto" FILES ${W_NET_WS_USOCKETS_CRYPTO_SRC_DIR})
 source_group("w_net/uSockets/eventing" FILES ${W_NET_WS_USOCKETS_EVENTING_SRC_DIR})
 source_group("w_net/uSockets/internal/eventing" FILES ${W_NET_WS_USOCKETS_INTERNAL_EVENTING_SRC_DIR})
 source_group("w_net/uSockets/internal/networking" FILES ${W_NET_WS_USOCKETS_INTERNAL_NETWORKING_SRC_DIR})
 source_group("w_net/uSockets/internal" FILES ${W_NET_WS_USOCKETS_INTERNAL_SRC_DIR})
 source_group("w_net/uSockets" FILES ${W_NET_WS_USOCKETS_SRC_DIR})
 source_group("w_net/ws/uWebSockets" FILES ${W_NET_WS_UWEBSOCKETS_SRC_DIR})
 source_group("w_net/ws" FILES ${W_NET_WS_SRC_DIR})
 source_group("w_net" FILES ${W_NET_SRC_DIR})
 source_group("w_os" FILES ${W_OS_SRC_DIR})
 source_group("w_script/lua" FILES ${W_SCRIPT_LUA_SRC_DIR})
 source_group("w_script" FILES ${W_SCRIPT_SRC_DIR})
 source_group("w_test" FILES ${W_TEST_SRC_DIR})
 source_group("wolf" FILES ${WOLF_SRC_DIR})
 
 #set c++17 
 #target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

 # set global compiler options
target_compile_definitions(${PROJECT_NAME} PUBLIC
        APR_DECLARE_EXPORT
        libapr_1_EXPORTS
        APU_DECLARE_STATIC
        APU_DSO_MODULE_BUILD
        XML_STATIC
        expat_EXPORTS
        _CRT_RAND_S
        NO_FSEEKO
        ASMV
        ASMINF
        SPDLOG_COMPILED_LIB
        SPDLOG_WCHAR_FILENAMES
        SPDLOG_USE_WCHAR
        EV_STANDALONE
        )

if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        DEBUG
    )
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        NDEBUG
    )
endif()

 # set activeMQ compile definitions
if (WOLF_ENABLE_ACTIVEMQ)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        WOLF_ENABLE_ACTIVEMQ
        _HAS_AUTO_PTR_ETC=1
    )
endif()

if (WOLF_ENABLE_AVX2)
    if (APPLE OR IOS)
         macro (set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
            set_property (TARGET ${TARGET} PROPERTY
                XCODE_ATTRIBUTE_${XCODE_PROPERTY} ${XCODE_VALUE})
         endmacro (set_xcode_property)
            set_xcode_property(${PROJECT_NAME} CLANG_X86_VECTOR_INSTRUCTIONS "avx2")
			set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS -liconv)
    elseif (LINUX)
        set (AVX2_FLAGS "-D__AVX -DUSE_AVX2")
        set (AVX2_FLAGS "${AVX2_FLAGS} -mavx2 -mfma")
        target_compile_definitions(${PROJECT_NAME} PUBLIC
                ${AVX2_FLAGS})
    endif()
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        WOLF_ENABLE_AVX2
    )
endif()

# set AVX512 compile definition
if (WOLF_ENABLE_AVX512)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        WOLF_ENABLE_AVX512
    )
    if (UNIX)
        set (AVX512_FLAGS "-D__AVX -DWOLF_ENABLE_AVX512 -mavx512f -mfma")
        target_compile_definitions(${PROJECT_NAME} PUBLIC ${AVX512_FLAGS})
    endif()
endif()

#set CURL compile definitions
if (WOLF_ENABLE_CURL)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        WOLF_ENABLE_CURL
        CURL_STATICLIB
    )
endif()

#set CXX extensions definition
if (WOLF_ENABLE_CXX_EXTENSIONS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        WOLF_ENABLE_CXX_EXTENSIONS
    )
endif()

#set FIBER compile definitions
if (WOLF_ENABLE_FIBER)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        WOLF_ENABLE_FIBER
        BOOST_BIND_GLOBAL_PLACEHOLDERS
    )
endif()

#set HTTP1.1 compile definitions
if (WOLF_ENABLE_HTTP1_1_SERVER)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        WOLF_ENABLE_HTTP1_1_SERVER
    )
endif()

 # set LUA compile definitions
if (WOLF_ENABLE_LUA)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        WOLF_ENABLE_LUA
    )
endif()
        
 # set LZ4 compile definitions
if (WOLF_ENABLE_LZ4)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        WOLF_ENABLE_LZ4
    )
endif()

 # set LZMA compile definitions
if (WOLF_ENABLE_LZMA)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        WOLF_ENABLE_LZMA
    )
endif()
       
# set Python3 compile definitions
if (WOLF_ENABLE_PYTHON3)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        WOLF_ENABLE_PYTHON3
    )
endif()
	
if (WOLF_ENABLE_QUIC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        WOLF_ENABLE_QUIC
    )
endif()

if (WOLF_ENABLE_HTTP1_1_WS)

    target_compile_definitions(${PROJECT_NAME} PUBLIC
        LIBUS_USE_LIBUV
        LIBUS_USE_OPENSSL
        #LIBUS_USE_WOLFSSL
        WOLF_ENABLE_HTTP1_1_WS
    )
endif()

# signing project
#if(IOS)
#    set_xcode_property(${PROJECT_NAME} CODE_SIGN_IDENTITY "Apple Development")  
#    set_xcode_property(${PROJECT_NAME} DEVELOPMENT_TEAM ${DEVELOPMENT_TEAM_ID})
#endif()

if (APPLE OR IOS)
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/io/w_io.c" PROPERTIES COMPILE_FLAGS "-x objective-c" )
endif()

if(MSVC)
    add_compile_options("/std:c++2a")
else()
    add_compile_options("-std=c++2a")
endif()
if (LINUX)
    add_compile_options(-fPIC)
endif()

if (WIN32 OR WIN64)
    target_link_libraries(${PROJECT_NAME} PRIVATE  expat apr-2 Shlwapi)
elseif(APPLE OR IOS)
    target_link_libraries(${PROJECT_NAME} PRIVATE  "-framework Foundation" expat)
    target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_BINARY_DIR}/_deps/apr-build/${CMAKE_BUILD_TYPE}/lib/libapr-1.a)
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE  expat ${CMAKE_CURRENT_BINARY_DIR}/_deps/apr-build/${CMAKE_BUILD_TYPE}/lib/libapr-2.a)
else()
    message(FATAL_ERROR "target_link_libraries not set")
endif()

if (WOLF_ENABLE_ACTIVEMQ)
    if(UNIX)
        target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_BINARY_DIR}/_deps/activemq-cpp-build/${CMAKE_BUILD_TYPE}/lib/libactivemq-cpp.a)
    else(MSVC)
        target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/activemq-cpp/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/libactivemq-cpp.lib)	
    endif()
endif()

if (WOLF_ENABLE_CURL)
    target_link_libraries(${PROJECT_NAME} PRIVATE libcurl)
endif()

#link fiber library
if (WOLF_ENABLE_FIBER)
    if(WIN64)
		if (CMAKE_BUILD_TYPE MATCHES Debug)
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/libboost_atomic-vc142-mt-gd-x64-1_75.lib)	
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/libboost_context-vc142-mt-gd-x64-1_75.lib)		
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/libboost_fiber-vc142-mt-gd-x64-1_75.lib)	
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/libboost_date_time-vc142-mt-gd-x64-1_75.lib)	
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/libboost_regex-vc142-mt-gd-x64-1_75.lib)		
		else()
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/libboost_atomic-vc142-mt-x64-1_75.lib)
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/libboost_context-vc142-mt-x64-1_75.lib)		
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/libboost_fiber-vc142-mt-s-x64-1_75.lib)	   
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/libboost_date_time-vc142-mt-s-x64-1_75.lib)	
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/libboost_regex-vc142-mt-s-x64-1_75.lib)	   			
		endif()
    elseif(WIN32)
		if (CMAKE_BUILD_TYPE MATCHES Debug)
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/libboost_atomic-vc142-mt-gd-x32-1_75.lib)
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/libboost_context-vc142-mt-gd-x32-1_75.lib)		
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/libboost_fiber-vc142-mt-gd-x32-1_75.lib)	
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/libboost_date_time-vc142-mt-gd-x32-1_75.lib)	
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/libboost_regex-vc142-mt-gd-x32-1_75.lib)				
		else()
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/libboost_atomic-vc142-mt-x32-1_75.lib)
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/libboost_context-vc142-mt-x32-1_75.lib)		
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/libboost_fiber-vc142-mt-s-x32-1_75.lib)	
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/libboost_date_time-vc142-mt-s-x32-1_75.lib)		
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/libboost_regex-vc142-mt-s-x32-1_75.lib)					
		endif()
    elseif(LINUX)
        if (CMAKE_BUILD_TYPE MATCHES Debug)
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/linux/x64/debug/libboost_atomic.a)
            target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/linux/x64/debug/libboost_context.a)		
            target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/linux/x64/debug/libboost_fiber.a)	
            target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/linux/x64/debug/libboost_date_time.a)	
            target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/linux/x64/debug/libboost_regex.a)				
        else()
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/linux/x64/release/libboost_atomic.a)
            target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/linux/x64/release/libboost_context.a)		
            target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/linux/x64/release/libboost_fiber.a)		
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/linux/x64/release/libboost_date_time.a)
			target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/boost/lib/linux/x64/release/libboost_regex.a)
        endif()
    endif()
endif()

#link uSockets
if (WOLF_ENABLE_HTTP1_1_WS)
    target_link_libraries(${PROJECT_NAME} PRIVATE  uv_a)
    target_link_libraries(${PROJECT_NAME} PRIVATE  zlibstatic)
    if(MSVC)
	    target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/openSSL/lib/${PREFIX_LIB_PATH}/libcrypto.lib)
        target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/openSSL/lib/${PREFIX_LIB_PATH}/libssl.lib)
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/openSSL/lib/${PREFIX_LIB_PATH}/libcrypto.a)
        target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/openSSL/lib/${PREFIX_LIB_PATH}/libssl.a)
    endif()
endif()

#link lua library
if(WOLF_ENABLE_LUA)
    target_link_libraries(${PROJECT_NAME} PRIVATE  luajit-static)
endif()

#link quiche library
if (WOLF_ENABLE_QUIC)
    target_link_libraries(${PROJECT_NAME} PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/_deps/quiche/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/quiche.lib)	
endif()

#link wolfssl library
#if (WOLF_ENABLE_SSL)
#    target_link_libraries(${PROJECT_NAME} PRIVATE wolfssl)	
#endif()

SET(TEST_PROJECT_NAME "${PROJECT_NAME}_test")
add_executable (${TEST_PROJECT_NAME} test.cpp)
target_link_libraries(${TEST_PROJECT_NAME} PRIVATE ${PROJECT_NAME})
