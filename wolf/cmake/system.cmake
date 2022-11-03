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

# fetch WOLF_SYSTEM_BORINGSSL
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

# fetch WOLF_SYSTEM_LIBUV
if (WOLF_SYSTEM_LIBUV)
    message("fetching https://github.com/libuv/libuv.git")
    FetchContent_Declare(
        uv_a
        GIT_REPOSITORY https://github.com/libuv/libuv.git
        GIT_TAG        master
    )
    FetchContent_MakeAvailable(uv_a)
    list(APPEND INCLUDES 
        ${uv_a_SOURCE_DIR}/include
        ${uv_a_BINARY_DIR}/include
    )
    list(APPEND LIBS uv_a)   
    
    add_definitions(-DLIBUS_USE_LIBUV)

    set_target_properties(
        uv
        uv_a
        PROPERTIES FOLDER "libuv") 
endif()

if (WOLF_SYSTEM_LOG)
    message("fetching https://github.com/gabime/spdlog.git")
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.x
    )
    set(SPDLOG_WCHAR_FILENAMES ON CACHE BOOL "SPDLOG_WCHAR_FILENAMES")
    set(SPDLOG_WCHAR_SUPPORT ON CACHE BOOL "SPDLOG_WCHAR_SUPPORT")

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
   file(GLOB_RECURSE WOLF_SYSTEM_USOCKET_CRYPTO_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/system/usocket/crypto/*"
   )
   file(GLOB_RECURSE WOLF_SYSTEM_USOCKET_EVENTING_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/system/usocket/eventing/*"
   )
   file(GLOB_RECURSE WOLF_SYSTEM_USOCKET_INTERNAL_EVENTING_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/system/usocket/internal/eventing/*"
   )
   file(GLOB_RECURSE WOLF_SYSTEM_USOCKET_INTERNAL_NETWORKING_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/system/usocket/internal/networking/*"
   )
   file(GLOB_RECURSE WOLF_SYSTEM_USOCKET_SRC_INTERNAL_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/system/usocket/crypto/internal.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/usocket/crypto/loop_data.h"
   )
   file(GLOB_RECURSE WOLF_SYSTEM_USOCKET_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/system/usocket/bsd.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/usocket/context.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/usocket/libusockets.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/usocket/loop.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/usocket/quic.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/usocket/quic.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/usocket/socket.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/usocket/udp.c"
   )
   list(APPEND INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/system/usocket
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