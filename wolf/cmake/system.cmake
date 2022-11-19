# fetch gsl
message("fetching https://github.com/microsoft/GSL.git")
FetchContent_Declare(
  gsl
  GIT_REPOSITORY https://github.com/microsoft/GSL.git
  GIT_TAG        main
  SOURCE_SUBDIR  include
)
FetchContent_Populate(gsl)

list(APPEND INCLUDES
    ${gsl_SOURCE_DIR}/include
)

# fetch boringssl
if (WOLF_SYSTEM_BORINGSSL)
    message("fetching https://github.com/google/boringssl.git")
    FetchContent_Declare(
        boringssl
        GIT_REPOSITORY https://github.com/google/boringssl.git
        GIT_TAG        master
    )
    FetchContent_MakeAvailable(boringssl)
    
    # we will need these for lsquic
    set(BORINGSSL_INCLUDE ${boringssl_SOURCE_DIR}/include)
    set(BORINGSSL_LIB_ssl ${boringssl_BINARY_DIR}/ssl/${CMAKE_BUILD_TYPE})
    set(BORINGSSL_LIB_crypto ${boringssl_BINARY_DIR}/crypto/${CMAKE_BUILD_TYPE})

    list(APPEND INCLUDES 
        ${BORINGSSL_INCLUDE}
        ${boringssl_BINARY_DIR}/include
    )
    list(APPEND LIBS ssl crypto)   
    
    add_definitions(-DLIBUS_USE_OPENSSL)

    set_target_properties(
        all_tests 
        boringssl_gtest
        boringssl_gtest_main
        bssl
        bssl_shim
        crypto
        crypto_test
        crypto_test_data
        decrepit
        decrepit_test
        fipsmodule
        fips_specific_tests_if_any
        global_target
        handshaker
        run_tests
        ssl
        ssl_test
        test_support_lib
        urandom_test
        PROPERTIES FOLDER "boringSSL") 
endif()

# fetch http websocket
if (WOLF_SYSTEM_HTTP_WS)
    file(GLOB_RECURSE WOLF_SYSTEM_HTTP_WS_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_ws_server.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_ws_server.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_ws_client.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_ws_client.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_http1_server.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_http1_server.hpp"
    )
endif()

# fetch spdlog
if (WOLF_SYSTEM_LOG)
    message("fetching https://github.com/gabime/spdlog.git")
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.x
    )
    set(SPDLOG_WCHAR_FILENAMES OFF CACHE BOOL "SPDLOG_WCHAR_FILENAMES")
    set(SPDLOG_WCHAR_SUPPORT OFF CACHE BOOL "SPDLOG_WCHAR_SUPPORT")

    FetchContent_MakeAvailable(spdlog)
    
    list(APPEND INCLUDES
        ${spdlog_SOURCE_DIR}/include
    )
    list(APPEND LIBS spdlog)

    file(GLOB_RECURSE WOLF_SYSTEM_LOG_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/system/log/*"
    )
endif()

# fetch mimalloc
if (WOLF_SYSTEM_MIMALLOC)
    message("fetching https://github.com/microsoft/mimalloc.git")
    FetchContent_Declare(
        mimalloc-static
        GIT_REPOSITORY https://github.com/microsoft/mimalloc.git
        GIT_TAG        master
    )
    set(MI_BUILD_OBJECT OFF CACHE BOOL "MI_BUILD_OBJECT")
    set(MI_BUILD_SHARED OFF CACHE BOOL "MI_BUILD_SHARED")
    set(MI_BUILD_TESTS OFF CACHE BOOL "MI_BUILD_TESTS")
    
    FetchContent_MakeAvailable(mimalloc-static)

    list(APPEND INCLUDES
        ${mimalloc_static_SOURCE_DIR}/include
    )
    list(APPEND LIBS mimalloc-static)
endif()

if (WOLF_SYSTEM_SOCKET)
    file(GLOB_RECURSE WOLF_SYSTEM_SOCKET_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_socket_options.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_tcp_client.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_tcp_client.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_tcp_server.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_tcp_server.hpp"
    )
    file(GLOB_RECURSE WOLF_SYSTEM_SOCKET_TEST_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/test/tcp.hpp"
    )
endif()

# fetch zlib
if (WOLF_SYSTEM_ZLIB)
    message("fetching https://github.com/madler/zlib.git")
    FetchContent_Declare(
        zlibstatic
        GIT_REPOSITORY https://github.com/madler/zlib.git
        GIT_TAG        master
    )
    set(MI_BUILD_OBJECT OFF CACHE BOOL "MI_BUILD_OBJECT")
    set(MI_BUILD_SHARED OFF CACHE BOOL "MI_BUILD_SHARED")
    set(MI_BUILD_TESTS OFF CACHE BOOL "MI_BUILD_TESTS")
    
    FetchContent_MakeAvailable(zlibstatic)

    list(APPEND INCLUDES
        ${zlib_SOURCE_DIR}/include
    )
    list(APPEND LIBS zlibstatic)

    set_target_properties(
        example
        minigzip
        zlib
        zlibstatic 
        PROPERTIES FOLDER "zlib")
endif()

file(GLOB_RECURSE WOLF_SYSTEM_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/system/getopt.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/system/w_trace.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/system/w_gametime.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/system/w_gametime.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/system/w_leak_detector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/system/w_leak_detector.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/system/w_process.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/system/w_process.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/system/w_trace.hpp"
)
file(GLOB_RECURSE WOLF_SYSTEM_TEST_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/system/test/gametime.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/system/test/process.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/system/test/signal_slot.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/system/test/trace.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/system/test/tcp.hpp"
)
