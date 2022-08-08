# fetch, include and link stream modules

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

if (WOLF_STREAM_QUIC)
    
    CHECK_FUNCTION_EXISTS(sendmmsg HAVE_SENDMMSG)
    CHECK_FUNCTION_EXISTS(recvmmsg HAVE_RECVMMSG)

    if (HAVE_SENDMMSG)
        add_definitions(
            -DHAVE_SENDMMSG
        )
    endif()

    if (HAVE_RECVMMSG)
        add_definitions(
            -DHAVE_RECVMMSG
        )
    endif()
    
    if (NOT WOLF_SYSTEM_BORINGSSL)
        message( FATAL_ERROR "'WOLF_STREAM_QUIC' needs 'WOLF_SYSTEM_BORINGSSL' = ON" )
    endif()
    if (NOT WOLF_SYSTEM_ZLIB)
        message( FATAL_ERROR "'WOLF_STREAM_QUIC' needs 'WOLF_SYSTEM_ZLIB' = ON" )
    endif()

    # get quic
    message("fetching https://github.com/litespeedtech/lsquic.git")
    FetchContent_Declare(
        lsquic
        GIT_REPOSITORY https://github.com/litespeedtech/lsquic.git
        GIT_TAG        master
    )
    
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set(LSQUIC_DEVEL ON CACHE BOOL "LSQUIC_DEVEL")
    else()
        set(LSQUIC_DEVEL OFF CACHE BOOL "LSQUIC_DEVEL")
    endif()

    set(LSQUIC_BIN OFF CACHE BOOL "LSQUIC_BIN")
    set(LSQUIC_SHARED_LIB OFF CACHE BOOL "LSQUIC_SHARED_LIB")
    set(LSQUIC_TESTS OFF CACHE BOOL "LSQUIC_TESTS")
    
    FetchContent_MakeAvailable(lsquic)
	
    if (WIN32)
        list(APPEND INCLUDES 
            ${lsquic_SOURCE_DIR}/wincompat
        )
    endif()

    list(APPEND INCLUDES
        ${lsquic_SOURCE_DIR}/src
        ${lsquic_SOURCE_DIR}/include
        ${lsquic_BINARY_DIR}/include
    )
    list(APPEND LIBS lsquic)
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