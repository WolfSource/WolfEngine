cmake_minimum_required(VERSION 3.0.0)
project(wolf.system.linux VERSION 1.68.0 DESCRIPTION "wolf.system.linux.x64")

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)

#[[if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif()
 set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)]]
# set the default path lib
if(UNIX)
        # LINUX
        if (CMAKE_BUILD_TYPE MATCHES Debug)
            set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../bin/linux/x64/Debug/)
        else()
            set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../bin/linux/x64/Release/)
        endif()
endif()

set(CMAKE_C_COMPILER "clang")#gcc
set(CMAKE_CXX_COMPILER "clang++")#g++
set(CMAKE_C_STANDARD 11)
#set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wno-undefined-inline -fPIC -std=c++11")
set( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fPIC -std=c++17" )
# includes
include(CPack)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../src/wolf.system/
        ${CMAKE_CURRENT_SOURCE_DIR}/../../src/wolf.system/base64
        ${CMAKE_CURRENT_SOURCE_DIR}/../../src/wolf.system/compression/msgpack/
        ${CMAKE_CURRENT_SOURCE_DIR}/../../src/wolf.system/log
        ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/apr/include/
        ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/apr/include/apr-1/apr/linux
        ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/apr/include/apr-1
        ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/curl/include/
        ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/libjpeg-turbo/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/libpng/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/nng/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/nng/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/openSSL/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/quiche/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/tls/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/zlib/include
        )

add_library(wolf.system.linux STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/base64/chromiumbase64.c
        ${CMAKE_CURRENT_SOURCE_DIR}/base64/fastavx512bwbase64.c
#        ${CMAKE_CURRENT_SOURCE_DIR}/base64/fastavxbase64.c
#        ${CMAKE_CURRENT_SOURCE_DIR}/base64/klompavxbase64.c
        ${CMAKE_CURRENT_SOURCE_DIR}/base64/quicktimebase64.c
        ${CMAKE_CURRENT_SOURCE_DIR}/base64/scalarbase64.c
        ${CMAKE_CURRENT_SOURCE_DIR}/chrono/w_chrono.c
        ${CMAKE_CURRENT_SOURCE_DIR}/chrono/w_gametime.c
        ${CMAKE_CURRENT_SOURCE_DIR}/chrono/w_timespan.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lz4/lz4.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lz4/lz4frame.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lz4/lz4hc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lz4/xxhash.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/7zAlloc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/7zArcIn.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/7zBuf.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/7zBuf2.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/7zCrc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/7zCrcOpt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/7zDec.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/7zFile.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/7zStream.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/Aes.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/AesOpt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/Alloc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/Bcj2.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/Bcj2Enc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/Bra.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/Bra86.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/BraIA64.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/CpuArch.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/Delta.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/DllSecur.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/LzFind.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/Lzma2Dec.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/Lzma86Dec.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/Lzma86Enc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/LzmaDec.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/LzmaLib.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/Ppmd7.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/Ppmd7Dec.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/Ppmd7Enc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/Sha256_lzma.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/Sort.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/Xz.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/XzCrc64.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/XzCrc64Opt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/XzIn.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/msgpack/objectc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/msgpack/unpack.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/msgpack/version.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/msgpack/vrefbuffer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/msgpack/zone.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compression/w_compress.c
        ${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_async.c
        ${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_atomic.c
        ${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_concurrent_queue.c
        ${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_condition_variable.c
        ${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_mutex.c
        ${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_signal.c
        ${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_thread.c
        ${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_thread_pool.c
        ${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_timer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/concurrency/libev/ev.c
        ${CMAKE_CURRENT_SOURCE_DIR}/concurrency/libev/event.c
        ${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_sigslot.c
        #        ${CMAKE_CURRENT_SOURCE_DIR}/fiber/w_fiber.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/io/w_io.c
        ${CMAKE_CURRENT_SOURCE_DIR}/log/logger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log/w_log.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log/spdlog/src/async.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log/spdlog/src/cfg.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log/spdlog/src/color_sinks.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log/spdlog/src/file_sinks.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log/spdlog/src/fmt.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log/spdlog/src/spdlog.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log/spdlog/src/stdout_sinks.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/memory/w_array.c
        ${CMAKE_CURRENT_SOURCE_DIR}/memory/w_hash.c
        ${CMAKE_CURRENT_SOURCE_DIR}/memory/w_mem_cache.c
        ${CMAKE_CURRENT_SOURCE_DIR}/memory/w_mem_map.c
        ${CMAKE_CURRENT_SOURCE_DIR}/memory/w_mem_pool.c
        ${CMAKE_CURRENT_SOURCE_DIR}/memory/w_shared_mem.c
        ${CMAKE_CURRENT_SOURCE_DIR}/memory/w_string.c
        ${CMAKE_CURRENT_SOURCE_DIR}/memory/w_table.c
        ${CMAKE_CURRENT_SOURCE_DIR}/net/w_net.c
        ${CMAKE_CURRENT_SOURCE_DIR}/os/w_cpu.c
        ${CMAKE_CURRENT_SOURCE_DIR}/os/w_process.c
        ${CMAKE_CURRENT_SOURCE_DIR}/os/w_window.c
        ${CMAKE_CURRENT_SOURCE_DIR}/script/w_lua.c
        ${CMAKE_CURRENT_SOURCE_DIR}/wolf.c
        )

# pre processors
target_compile_definitions(wolf.system.linux PUBLIC
        DEBUG HAVE_CONFIG_H
        BUILDING_LIBCURL
        USE_JMP
        XML_ENABLE_VISIBILITY=1
        HAVE_EXPAT_CONFIG_H
        expat_EXPORTS
        SPDLOG_COMPILED_LIB
        APU_DECLARE_STATIC
        SPDLOG_WCHAR_FILENAMES
        NNG_PLATFORM_POSIX
        APR_DECLARE_EXPORT
        APU_DSO_MODULE_BUILD
        LIBUS_USE_KQUEUE
        CURL_STATICLIB
        LIBUS_USE_OPENSSL
        _CRT_RAND_S
        XML_STATIC
        ASMINF
        EV_STANDALONE
        _CRT_SECURE_NO_DEPRECATE
        ASMV
        NO_FSEEKO
        NNG_LITTLE_ENDIAN
        LIBUS_USE_LIBUV
        _CRT_SECURE_NO_WARNINGS
        NNG_PRIVATE
        NNG_MAX_TASKQ_THREADS=16
        NNG_HAVE_SNPRINTF=1
        NNG_HAVE_CONDVAR=1
        NNG_HAVE_STRNLEN=1
        NNG_TRANSPORT_WS
        NNG_TRANSPORT_WSS
        NNG_SUPP_HTTP
        NNG_STATIC_LIB
        _CRT_NONSTDC_NO_DEPRECATE
        W_PLATFORM_LINUX
        libapr_1_EXPORTS
        LIBUS_USE_LIBUV
        )

if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(wolf.system.linux PUBLIC _DEBUG DEBUG)
endif()

# compiler options
target_compile_options(wolf.system.linux PRIVATE -fPIC -m64)

# libs
#link_directories(/usr/local/lib)
if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/apr/lib/linux/x64/debug/libapr-1.a)
    target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/curl/lib/linux/x64/debug/libcurl.a)
    target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/libjpeg-turbo/lib/linux/x64/debug/libturbojpeg.a)
    target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/libjpeg-turbo/lib/linux/x64/debug/libjpeg.a)
    target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/libpng/lib/linux/x64/debug/libpng15.a)
    target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/nng/lib/linux/x64/debug/libnng.a)
    target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/openSSL/lib/linux/x64/debug/libssl.a)
    target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/openSSL/lib/linux/x64/debug/libcrypto.a)
    target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/tls/lib/linux/x64/debug/libmbedcrypto.a)
    target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/tls/lib/linux/x64/debug/libmbedtls.a)
    target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/tls/lib/linux/x64/debug/libmbedx509.a)

else()
    target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/apr/lib/linux/x64/release/libapr-1.a)
    target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/curl/lib/linux/x64/release/libcurl.a)
    target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/libjpeg-turbo/lib/linux/x64/release/libturbojpeg.a)
    target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/libjpeg-turbo/lib/linux/x64/release/libjpeg.a)
    target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/libpng/lib/linux/x64/release/libpng15.a)
    target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/nng/lib/linux/x64/release/libnng.a)
    target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/openSSL/lib/linux/x64/release/libssl.a)
    target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/openSSL/lib/linux/x64/release/libcrypto.a)
    target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/tls/lib/linux/x64/release/libmbedcrypto.a)
    target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/tls/lib/linux/x64/release/libmbedtls.a)
    target_link_libraries(wolf.system.linux ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/tls/lib/linux/x64/release/libmbedx509.a)

endif()

target_link_libraries(wolf.system.linux xcb anl rt nsl pthread)
