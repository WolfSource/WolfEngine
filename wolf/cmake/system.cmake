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

# fetch fmt
message("fetching https://github.com/fmtlib/fmt.git")
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        master
)
set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(fmt)
list(APPEND INCLUDES
    ${fmt_SOURCE_DIR}/include
)
add_definitions(-DFMT_HEADER_ONLY)

# fetch boringssl, note that boringssl was already fetched by GRPC  
if (WOLF_SYSTEM_BORINGSSL AND (NOT WOLF_STREAM_GRPC))
    message("fetching https://github.com/google/boringssl.git")
    FetchContent_Declare(
        boringssl
        GIT_REPOSITORY https://github.com/google/boringssl.git
        GIT_TAG        master
    )
    set(FETCHCONTENT_QUIET OFF)
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

    set(FETCHCONTENT_QUIET OFF)
    FetchContent_MakeAvailable(spdlog)
    
    list(APPEND INCLUDES
        ${spdlog_SOURCE_DIR}/include
    )
    list(APPEND LIBS spdlog)

    file(GLOB_RECURSE WOLF_SYSTEM_LOG_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/system/log/*"
    )
    
    list(APPEND SRCS 
        ${WOLF_SYSTEM_LOG_SRC} 
    )
endif()

if (WOLF_SYSTEM_LZ4)
  message("fetching https://github.com/lz4/lz4.git")
  FetchContent_Declare(
    lz4_static
    GIT_REPOSITORY https://github.com/lz4/lz4.git
    GIT_TAG        dev
    SOURCE_SUBDIR  build/cmake
  )
  set(FETCHCONTENT_QUIET OFF)
  FetchContent_MakeAvailable(lz4_static)

  file(GLOB_RECURSE WOLF_SYSTEM_LZ4_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/system/compression/w_lz4.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/system/compression/w_lz4.hpp"
  )
  list(APPEND SRCS ${WOLF_SYSTEM_LZ4_SRCS})
  list(APPEND INCLUDES ${lz4_SOURCE_DIR}/include)
  list(APPEND LIBS lz4_static)   

  set_target_properties(
    lz4_static
    lz4c
    lz4cli
    PROPERTIES FOLDER "lz4") 

endif()

if (WOLF_SYSTEM_LZMA)
  message("fetching https://github.com/WolfEngine/lzma.git")
  FetchContent_Declare(
    lzma
    GIT_REPOSITORY https://github.com/WolfEngine/lzma.git
    GIT_TAG        main
  )
  set(FETCHCONTENT_QUIET OFF)
  FetchContent_MakeAvailable(lzma)

  file(GLOB_RECURSE WOLF_SYSTEM_LZMA_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/system/compression/w_lzma.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/system/compression/w_lzma.hpp"
  )
  list(APPEND SRCS ${WOLF_SYSTEM_LZMA_SRCS})
  list(APPEND INCLUDES ${lzma_SOURCE_DIR}/src)
  list(APPEND LIBS lzma)  
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
    
    set(FETCHCONTENT_QUIET OFF)
    FetchContent_MakeAvailable(mimalloc-static)

    list(APPEND INCLUDES
        ${mimalloc-static_SOURCE_DIR}/include
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
        "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_tcp_fiber_server.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_tcp_fiber_server.hpp"
    )
    list(APPEND SRCS 
        ${WOLF_SYSTEM_SOCKET_SRC} 
        ${WOLF_SYSTEM_SOCKET_TEST_SRC}
    )
endif()

# fetch zlib 
if (WOLF_SYSTEM_ZLIB)
    # note that zlib was already fetched by GRPC 
    if (NOT WOLF_STREAM_GRPC)
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
            zlib
            zlibstatic 
            PROPERTIES FOLDER "zlib")
    else()
        set_target_properties(
            example
            minigzip
            zlib
            zlibstatic 
            PROPERTIES FOLDER "zlib")
    endif()
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
    "${CMAKE_CURRENT_SOURCE_DIR}/system/test/*"
)

list(APPEND SRCS 
    ${WOLF_SYSTEM_SRC} 
    ${WOLF_SYSTEM_TEST_SRC}
)

if (WOLF_SYSTEM_GAMEPAD)
    message("fetching https://github.com/libsdl-org/SDL")
    FetchContent_Declare(
            sdl
            GIT_REPOSITORY https://github.com/libsdl-org/SDL
            GIT_TAG        release-2.26.1
    )

    set(FETCHCONTENT_QUIET OFF)
    FetchContent_MakeAvailable(sdl)

    list(APPEND INCLUDES
            ${sdl_SOURCE_DIR}/include
            )
    list(APPEND LIBS SDL2)

    file(GLOB_RECURSE WOLF_SYSTEM_GAMEPAD_SRC
            "${CMAKE_CURRENT_SOURCE_DIR}/system/gamepad/w_gamepad.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/system/gamepad/w_gamepad_emscripten.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/system/gamepad/w_gamepad_sdl.cpp"
            )
    list(APPEND SRCS
            ${WOLF_SYSTEM_GAMEPAD_SRC}
            )
endif()
