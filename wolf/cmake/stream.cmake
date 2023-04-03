# fetch gRPC
if (WOLF_STREAM_GRPC)
    if (EMSCRIPTEN)
        message(FATAL_ERROR "the wasm32 target is not supported for WOLF_STREAM_GRPC")
    endif()

    # enable zlib
    set(WOLF_SYSTEM_ZLIB TRUE)

    message("fetching https://github.com/grpc/grpc")
    FetchContent_Declare(
        grpc
        GIT_REPOSITORY https://github.com/grpc/grpc
        GIT_TAG        master
    )
    
    set(ABSL_BUILD_TESTING OFF CACHE BOOL "ABSL_BUILD_TESTING")

    set(gRPC_BUILD_CSHARP_EXT OFF CACHE BOOL "GRPC_BUILD_CSHARP_EXT")
    set(gRPC_BUILD_GRPC_CPP_PLUGIN ON CACHE BOOL "gRPC_BUILD_GRPC_CPP_PLUGIN")
    set(gRPC_BUILD_GRPC_CSHARP_PLUGIN OFF CACHE BOOL "gRPC_BUILD_GRPC_CSHARP_PLUGIN")
    set(gRPC_BUILD_GRPC_NODE_PLUGIN OFF CACHE BOOL "gRPC_BUILD_GRPC_NODE_PLUGIN")
    set(gRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN OFF CACHE BOOL "gRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN")
    set(gRPC_BUILD_GRPC_PHP_PLUGIN OFF CACHE BOOL "gRPC_BUILD_GRPC_PHP_PLUGIN")
    set(gRPC_BUILD_GRPC_PYTHON_PLUGIN OFF CACHE BOOL "gRPC_BUILD_GRPC_PYTHON_PLUGIN")
    set(gRPC_BUILD_GRPC_RUBY_PLUGIN OFF CACHE BOOL "gRPC_BUILD_GRPC_RUBY_PLUGIN")
    set(gRPC_BUILD_TESTS OFF CACHE BOOL "gRPC_BUILD_TESTS")
    
    set(RE2_BUILD_TESTING OFF CACHE BOOL "RE2_BUILD_TESTING")

    set(FETCHCONTENT_QUIET OFF)
    FetchContent_MakeAvailable(grpc)

    file(GLOB_RECURSE WOLF_STREAM_GRPC_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/stream/grpc/*"
    )
    list(APPEND SRCS 
        ${WOLF_STREAM_GRPC_SRC} 
    )
    list(APPEND INCLUDES
        ${grpc_SOURCE_DIR}/include
    )
    list(APPEND LIBS grpc)
    
    set_target_properties(
        acountry 
        address_sorting
        adig
        ahost
        bssl
        c-ares
        crypto
        example
        gpr
        grpc
        grpc_authorization_provider
        grpc_cpp_plugin
        grpc_plugin_support
        grpc_unsecure
        grpc++
        grpc++_alts
        grpc++_error_details
        grpc++_reflection
        grpc++_unsecure
        grpcpp_channelz
        libprotobuf
        libprotobuf-lite
        libprotoc
        minigzip
        plugins
        protoc
        re2
        ssl
        tools
        tools_c
        tools_cxx
        upb
        PROPERTIES FOLDER "gRPC")

endif()

if (WOLF_STREAM_JANUS)

    file(GLOB_RECURSE WOLF_STREAM_JANUS_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/stream/janus/*"
    )

    list(APPEND SRCS 
        ${WOLF_STREAM_JANUS_SRC}
    )

endif()

# fetch lsquic
if (WOLF_STREAM_QUIC)
    if (EMSCRIPTEN)
        message(FATAL_ERROR "WOLF_STREAM_QUIC is not supported for wasm32 target")
    endif()
    if (NOT WOLF_SYSTEM_SSL)
        message(FATAL_ERROR "WOLF_SYSTEM_SSL is required for WOLF_STREAM_QUIC")
    endif()
    
    message("fetching https://github.com/litespeedtech/lsquic.git")
    FetchContent_Declare(
        lsquic
        GIT_REPOSITORY https://github.com/litespeedtech/lsquic.git
        GIT_TAG        master
    )

    set(LSQUIC_TESTS OFF CACHE BOOL "LSQUIC_TESTS")

    set(FETCHCONTENT_QUIET OFF)
    FetchContent_MakeAvailable(lsquic)

    list(APPEND INCLUDES
        ${lsquic_SOURCE_DIR}/include
        ${lsquic_SOURCE_DIR}/wincompat
    )
    list(APPEND LIBS lsquic)
    
endif()

if (WOLF_STREAM_RIST)
    if (EMSCRIPTEN)
        message(FATAL_ERROR "the wasm32 target is not supported for WOLF_STREAM_RIST")
    endif()

    set(RIST_TARGET "rist")
    message("fetching https://code.videolan.org/rist/librist.git")
    FetchContent_Declare(
        ${RIST_TARGET}
        GIT_REPOSITORY https://code.videolan.org/rist/librist.git
        GIT_TAG        master
      )
    
    set(FETCHCONTENT_QUIET OFF)
    FetchContent_MakeAvailable(${RIST_TARGET})
      
    if (ANDROID)
      add_custom_command(OUTPUT rist_command.out COMMAND
      /bin/bash "${CMAKE_CURRENT_SOURCE_DIR}/third_party/shells/librist/librist-android.sh" --build_dir=${rist_BINARY_DIR}
       WORKING_DIRECTORY ${rist_SOURCE_DIR})
      add_custom_target(rist ALL DEPENDS rist_command.out)
      
      list(APPEND LIBS
        ${rist_BINARY_DIR}/librist.a)
    else ()
      STRING(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
      
      add_custom_command(OUTPUT rist_command.out COMMAND cmd /c "meson setup ${rist_BINARY_DIR} --backend vs2022 --default-library static --buildtype ${CMAKE_BUILD_TYPE_LOWER} & meson compile -C ${rist_BINARY_DIR}" WORKING_DIRECTORY ${rist_SOURCE_DIR})
      add_custom_target(rist ALL DEPENDS rist_command.out)

      list(APPEND LIBS
        ws2_32
        ${rist_BINARY_DIR}/librist.a)
        
    endif()
    
    list(APPEND INCLUDES
        ${rist_BINARY_DIR}
        ${rist_BINARY_DIR}/include
        ${rist_BINARY_DIR}/include/librist
        ${rist_SOURCE_DIR}/src
        ${rist_SOURCE_DIR}/include
        ${rist_SOURCE_DIR}/include/librist
        ${rist_SOURCE_DIR}/contrib
        ${rist_SOURCE_DIR}/contrib/mbedtls/include
      )
      
      file(GLOB_RECURSE WOLF_STREAM_RIST_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/stream/rist/*"
      )
      list(APPEND SRCS ${WOLF_STREAM_RIST_SRC})
endif()

if (WOLF_STREAM_WEBRTC)
    if (EMSCRIPTEN)
        message(FATAL_ERROR "the wasm32 target is not supported for WOLF_STREAM_WEBRTC")
    endif()

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

    list(APPEND SRCS 
        ${WOLF_STREAM_WEBRTC_CAPTURER_SRC}
        ${WOLF_STREAM_WEBRTC_DATA_SRC}
        ${WOLF_STREAM_WEBRTC_INTERCEPTOR_SRC}
        ${WOLF_STREAM_WEBRTC_MEDIA_SRC}
        ${WOLF_STREAM_WEBRTC_PEER_SRC}
    )
endif()

file(GLOB_RECURSE WOLF_STREAM_TEST_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/stream/test/*"
)
list(APPEND SRCS ${WOLF_STREAM_TEST_SRC})

