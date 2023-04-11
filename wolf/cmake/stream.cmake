# fetch gRPC
if (WOLF_STREAM_GRPC)
    if (EMSCRIPTEN)
        message(FATAL_ERROR "the wasm32 target is not supported for WOLF_STREAM_GRPC")
    endif()
    vcpkg_install(asio-grpc asio-grpc TRUE)
    list(APPEND LIBS asio-grpc::asio-grpc)

    file(GLOB_RECURSE WOLF_STREAM_GRPC_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/stream/grpc/*"
    )

    list(APPEND SRCS 
        ${WOLF_STREAM_GRPC_SRC}
    )

    # TODO (Pooya): Not working
    asio_grpc_protobuf_generate(
        GENERATE_GRPC GENERATE_MOCK_CODE
        OUT_VAR "RAFT_PROTO_SOURCES"
        OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/protos"
        IMPORT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/protos"
        PROTOS  "${CMAKE_CURRENT_SOURCE_DIR}/protos/raft.proto")

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

