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

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # don't 'fetch fmt for visual c++
    message("fetching https://github.com/fmtlib/fmt.git")
    FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG        9.1.0 # this will work for spdlog
    )
    set(FETCHCONTENT_QUIET OFF)
    FetchContent_MakeAvailable(fmt)
    list(APPEND INCLUDES
        ${fmt_SOURCE_DIR}/include
    )
    add_definitions(-DFMT_HEADER_ONLY)
endif()

# fetch boringssl, note that boringssl was already fetched by GRPC  
if (WOLF_SYSTEM_BORINGSSL AND (NOT WOLF_STREAM_GRPC))
    if (EMSCRIPTEN)
        message(FATAL_ERROR "the wasm32 target is not supported for WOLF_SYSTEM_BORINGSSL")
    endif()

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

if (WOLF_SYSTEM_GAMEPAD_CLIENT)
    file(GLOB_RECURSE WOLF_SYSTEM_GAMEPAD_CLIENT_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/system/gamepad/w_gamepad_client.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/gamepad/w_gamepad_client_emc.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/gamepad/w_gamepad_client_keymap.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/gamepad/w_gamepad_client_sdl.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/gamepad/w_gamepad_client_types.hpp"
    )
    list(APPEND SRCS ${WOLF_SYSTEM_GAMEPAD_CLIENT_SRC})

    if (NOT EMSCRIPTEN)
        message("fetching https://github.com/libsdl-org/SDL")
        FetchContent_Declare(
            SDL3-static
            GIT_REPOSITORY https://github.com/libsdl-org/SDL
            GIT_TAG        main
        )

        set(SDL_AUDIO OFF CACHE BOOL "SDL_AUDIO")
        set(SDL_DIRECTX OFF CACHE BOOL "SDL_DIRECTX")
        set(SDL_DISKAUDIO OFF CACHE BOOL "SDL_DISKAUDIO")
        set(SDL_DUMMYAUDIO OFF CACHE BOOL "SDL_DUMMYAUDIO")
        set(SDL_DUMMYVIDEO OFF CACHE BOOL "SDL_DUMMYVIDEO")
        set(SDL_FILE OFF CACHE BOOL "SDL_FILE")
        set(SDL_FILESYSTEM OFF CACHE BOOL "SDL_FILESYSTEM")
        set(SDL_METAL OFF CACHE BOOL "SDL_METAL")
        set(SDL_OFFSCREEN OFF CACHE BOOL "SDL_OFFSCREEN")
        set(SDL_OPENGL OFF CACHE BOOL "SDL_OPENGL")
        set(SDL_OPENGLES OFF CACHE BOOL "SDL_OPENGLES")
        set(SDL_RENDER OFF CACHE BOOL "SDL_RENDER")
        set(SDL_RENDER_D3D OFF CACHE BOOL "SDL_RENDER_D3D")
        set(SDL_RENDER_METAL OFF CACHE BOOL "SDL_RENDER_METAL")
        set(SDL_TEST OFF CACHE BOOL "SDL_TEST")
        set(SDL_TESTS OFF CACHE BOOL "SDL_TESTS")
        set(SDL_VIDEO OFF CACHE BOOL "SDL_VIDEO")
        set(SDL_VULKAN OFF CACHE BOOL "SDL_VULKAN")
        set(SDL_WASAPI OFF CACHE BOOL "SDL_WASAPI")
        set(SDL_SHARED OFF CACHE BOOL "SDL_SHARED")

        set(SDL_STATIC ON CACHE BOOL "SDL_STATIC")
        set(SDL_HIGHDAPI_JOYSTICK ON CACHE BOOL "SDL_HIGHDAPI_JOYSTICK")
        set(SDL_JOYSTICK ON CACHE BOOL "SDL_JOYSTICK")
        set(SDL_XINPUT ON CACHE BOOL "SDL_XINPUT")

        set(FETCHCONTENT_QUIET OFF)
        FetchContent_MakeAvailable(SDL3-static)

        list(APPEND INCLUDES
            ${SDL3-static_SOURCE_DIR}/include
            )
        list(APPEND LIBS SDL3-static)

        set_target_properties(
            SDL3-static
            uninstall
            PROPERTIES FOLDER "SDL") 
    endif()
endif()

if (WOLF_SYSTEM_GAMEPAD_VIRTUAL)
  if (NOT WIN32)
    message(SEND_ERROR "WOLF_SYSTEM_GAMEPAD_VIRTUAL can only build for Windows")
  else()
    message("fetching https://github.com/ViGEm/ViGEmClient.git")
    FetchContent_Declare(
      ViGEmClient
      GIT_REPOSITORY https://github.com/ViGEm/ViGEmClient.git
      GIT_TAG        master
    )
    FetchContent_MakeAvailable(ViGEmClient)

    file(GLOB_RECURSE WOLF_SYSTEM_GAMEPAD_VIRTUAL_SRCS
      "${CMAKE_CURRENT_SOURCE_DIR}/system/gamepad/w_gamepad_virtual_pool.cpp"
      "${CMAKE_CURRENT_SOURCE_DIR}/system/gamepad/w_gamepad_virtual_pool.hpp"
      "${CMAKE_CURRENT_SOURCE_DIR}/system/gamepad/w_gamepad_virtual.cpp"
      "${CMAKE_CURRENT_SOURCE_DIR}/system/gamepad/w_gamepad_virtual.hpp"
    )
    list(APPEND SRCS ${WOLF_SYSTEM_GAMEPAD_VIRTUAL_SRCS})
    list(APPEND INCLUDES ${ViGEmClient_SOURCE_DIR}/include)
    list(APPEND LIBS 
      ViGEmClient
      Xinput.lib 
      SetupAPI.lib)
   endif()
endif()

# fetch spdlog
if (WOLF_SYSTEM_LOG)
    if (EMSCRIPTEN)
        message(FATAL_ERROR "the wasm32 target is not supported for WOLF_SYSTEM_LOG")
    endif()

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

    # use external fmt headers for other compilers except MSVC
    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_definitions(-DSPDLOG_FMT_EXTERNAL_HQ)
    endif()

endif()

if (WOLF_SYSTEM_LZ4)
  if (EMSCRIPTEN)
        message(FATAL_ERROR "the wasm32 target is not supported for WOLF_SYSTEM_LZ4")
  endif()
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
  if (EMSCRIPTEN)
        message(FATAL_ERROR "the wasm32 target is not supported for WOLF_SYSTEM_LZMA")
  endif()
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
    if (EMSCRIPTEN)
        message(FATAL_ERROR "the wasm32 target is not supported for WOLF_SYSTEM_MIMALLOC")
    endif()
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

# include socket/websocket sources
if (EMSCRIPTEN)
    if (WOLF_SYSTEM_HTTP1_WS)
        file(GLOB_RECURSE WOLF_SYSTEM_SOCKET_SRC
            "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_ws_client_emc.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_ws_client_emc.hpp"
        )
    endif()
else()
    if (WOLF_SYSTEM_SOCKET)
        file(GLOB_RECURSE WOLF_SYSTEM_SOCKET_SRC
            "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_socket_options.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_tcp_client.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_tcp_client.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_tcp_server.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_tcp_server.hpp"
            if (WOLF_SYSTEM_HTTP1_WS)
                "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_ws_client.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_ws_client.hpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_ws_server.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/system/socket/w_ws_server.hpp"
            endif()
        )
    endif()
endif()
list(APPEND SRCS ${WOLF_SYSTEM_SOCKET_SRC})

# fetch zlib 
if (WOLF_SYSTEM_ZLIB)
    if (EMSCRIPTEN)
        message(FATAL_ERROR "the wasm32 target is not supported for WOLF_SYSTEM_ZLIB")
    endif()
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

if (WOLF_SYSTEM_POSTGRESQL)
    find_package(PostgreSQL REQUIRED)

    file(GLOB_RECURSE WOLF_SYSTEM_POSTGRESQL_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/system/db/w_postgresql.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/db/w_postgresql.cpp"
    )

    list(APPEND SRCS ${WOLF_SYSTEM_POSTGRESQL_SRC})
    list(APPEND LIBS PostgreSQL::PostgreSQL)
endif()

if (EMSCRIPTEN)
    file (GLOB_RECURSE WOLF_SYSTEM_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/system/w_trace.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/w_gametime.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/w_gametime.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/w_trace.hpp"
    )
else()
    file (GLOB_RECURSE WOLF_SYSTEM_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/system/w_trace.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/w_gametime.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/w_gametime.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/w_trace.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/getopt.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/w_leak_detector.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/w_leak_detector.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/w_process.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/w_process.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/system/w_ring_buffer_spsc.hpp"
    )
endif()

file(GLOB_RECURSE WOLF_SYSTEM_TEST_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/system/test/*"
)

list(APPEND SRCS 
    ${WOLF_SYSTEM_SRC} 
    ${WOLF_SYSTEM_TEST_SRC}
)
