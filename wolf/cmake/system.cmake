# fetch, include and link system modules

# fetch boost
include(cmake/boost.cmake)

# fetch boringSSL
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
		PROPERTIES FOLDER "boringSSL")
	
    set (GETOPT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/system)
    set (BORINGSSL_INCLUDE ${boringssl_SOURCE_DIR}/include)
    set (BORINGSSL_LIB_ssl ${boringssl_BINARY_DIR}/ssl/${CMAKE_BUILD_TYPE}/ssl.${LIB_EXT})
    set (BORINGSSL_LIB_crypto ${boringssl_BINARY_DIR}/crypto/${CMAKE_BUILD_TYPE}/crypto.${LIB_EXT})

    list(APPEND INCLUDES 
        ${BORINGSSL_INCLUDE}
    )
    list(APPEND LIBS ${BORINGSSL_LIB_ssl} ${BORINGSSL_LIB_crypto})
endif()

# fetch libunifex
if (WOLF_SYSTEM_EXECUTION)
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

#if (WOLF_SYSTEM_LIBEVENT)
#   	# fetch libevent
#    message("fetching https://github.com/libevent/libevent.git")
#    FetchContent_Declare(
#        libevent
#        GIT_REPOSITORY https://github.com/libevent/libevent.git
#        GIT_TAG        master
#    )
#    
#    if(CMAKE_BUILD_TYPE MATCHES Debug)
#        set(EVENT__DISABLE_DEBUG_MODE OFF CACHE BOOL "EVENT__DISABLE_DEBUG_MODE")
#    else()
#        set(EVENT__DISABLE_DEBUG_MODE ON CACHE BOOL "EVENT__DISABLE_DEBUG_MODE")
#    endif()
#
#    set(EVENT__DISABLE_BENCHMARK ON CACHE BOOL "EVENT__DISABLE_BENCHMARK")
#    set(EVENT__DISABLE_MBEDTLS ON CACHE BOOL "EVENT__DISABLE_MBEDTLS")
#    set(EVENT__DISABLE_OPENSSL ON CACHE BOOL "EVENT__DISABLE_OPENSSL")
#    set(EVENT__DISABLE_SAMPLES ON CACHE BOOL "EVENT__DISABLE_SAMPLES")
#    set(EVENT__DISABLE_TESTS ON CACHE BOOL "EVENT__DISABLE_TESTS")
#    set(EVENT__LIBRARY_TYPE "STATIC")
#    
#    FetchContent_MakeAvailable(libevent)
#    set (EVENT_INCLUDE_DIR ${libevent_SOURCE_DIR}/include)
#    set (EVENT_LIB event_static)
#
#    set_target_properties(
#		event_core_static
#        event_extra_static
#        event_static
#        uninstall
#		PROPERTIES FOLDER "Libevent")
#	
#    list(APPEND INCLUDES 
#        ${EVENT_INCLUDE_DIR}
#        ${libevent_BINARY_DIR}/include
#    )
#    list(APPEND LIBS ${EVENT_LIB})
#endif()

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

# fetch zlib
if (WOLF_SYSTEM_ZLIB)
	# get zlib
    message("fetching https://github.com/madler/zlib")
    FetchContent_Declare(
        zlibstatic
        GIT_REPOSITORY https://github.com/madler/zlib
        GIT_TAG        master
    )
    FetchContent_MakeAvailable(zlibstatic)
	set_target_properties(
		zlib zlibstatic minigzip example
		PROPERTIES FOLDER "zlib")

    list(APPEND INCLUDES 
        ${zlibstatic_SOURCE_DIR}
    )
    list(APPEND LIBS zlibstatic)
endif()