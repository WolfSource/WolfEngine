### requires WOLF_EXTERN_DEPS_DIR and PREFIX_LIB_PATH

include_guard()

include(FetchContent)

# A hack that lets FetchContent cooperate with find_package
set(HANDLED_BY_EXTERNAL "") # a list of packages for find_package to ignore

# Note: when you shadow a function, the previous definition
# is still available by prefixing with an underscore

# Idea from C++Now2017: Daniel Pfeifer "Effective CMake"
# https://youtu.be/bsXLMQ6WgIk?t=3159
# Note: there appears to be a typo in the presentation slides.
# We need to check ${ARGV0}, not ${ARG0}.
macro(find_package)
    if(NOT "${ARGV0}" IN_LIST HANDLED_BY_EXTERNAL)
        _find_package(${ARGV})
    else()
        message(DEBUG "${ARGV0} already fetched")
    endif()
endmacro()

function(wolf_find_expat)
    message(STATUS "wolf dependency libexpat")
    FetchContent_Declare(
        expat
        GIT_REPOSITORY https://github.com/libexpat/libexpat.git
        GIT_TAG        master
        SOURCE_SUBDIR  expat
    )
    list(APPEND HANDLED_BY_EXTERNAL expat)
    FetchContent_MakeAvailable(expat)
endfunction()

function(wolf_find_apr)
    message(STATUS "wolf dependency apr")

    set(APR_MINIMAL_BUILD ON CACHE BOOL "APR_MINIMAL_BUILD")

    FetchContent_Declare(
        apr
        GIT_REPOSITORY https://github.com/apache/apr.git
        GIT_TAG        trunk
    )

    list(APPEND HANDLED_BY_EXTERNAL apr)
    FetchContent_MakeAvailable(apr)

    FetchContent_GetProperties(apr)
    target_include_directories(apr-2 PUBLIC "${apr_BUILD_DIR}")
    target_include_directories(apr-2 PUBLIC "${apr_SOURCE_DIR}/include")
endfunction()

function(wolf_find_libcurl)
    message(STATUS "wolf dependency libcurl")
    FetchContent_Declare(
        libcurl
        GIT_REPOSITORY https://github.com/curl/curl.git
        GIT_TAG        master
    )
    list(APPEND HANDLED_BY_EXTERNAL libcurl)
    FetchContent_MakeAvailable(libcurl)
endfunction()

function(wolf_find_luajit)
    message(STATUS "wolf dependency luajit")
    FetchContent_Declare(
        luajit
        GIT_REPOSITORY https://github.com/torch/luajit-rocks.git
        GIT_TAG        master
    )
    list(APPEND HANDLED_BY_EXTERNAL luajit)
    FetchContent_MakeAvailable(luajit)
    # target static: luajit-static
endfunction()

function(wolf_find_libuv)
    message(STATUS "wolf dependency libuv")
    FetchContent_Declare(
        libuv
        GIT_REPOSITORY https://github.com/libuv/libuv.git
        GIT_TAG        master
    )
    list(APPEND HANDLED_BY_EXTERNAL libuv)
    FetchContent_MakeAvailable(libuv)
endfunction()

function(wolf_find_zlib)
    message(STATUS "wolf dependency zlib")
    FetchContent_Declare(
        zlib
        GIT_REPOSITORY https://github.com/madler/zlib.git
        GIT_TAG        master
    )
    list(APPEND HANDLED_BY_EXTERNAL zlib)
    FetchContent_MakeAvailable(zlib)
endfunction()

function(wolf_find_wolfssl)
    message(STATUS "wolf dependency wolfssl")
    FetchContent_Declare(
        wolfssl
        GIT_REPOSITORY https://github.com/wolfSSL/wolfssl.git
        GIT_TAG        master
    )
    list(APPEND HANDLED_BY_EXTERNAL wolfssl)
    FetchContent_MakeAvailable(wolfssl)
    # target: wolfssl
endfunction()

function(wolf_find_openssl)
    message(STATUS "wolf dependency openssl")

    list(APPEND HANDLED_BY_EXTERNAL OpenSSL)

    add_library(OpenSSL::SSL INTERFACE)
    add_library(OpenSSL::Crypto INTERFACE)

    target_include_directories(OpenSSL::SSL "${WOLF_EXTERN_DEPS_DIR}/openSSL/include")
    target_include_directories(OpenSSL::Crypto "${WOLF_EXTERN_DEPS_DIR}/openSSL/include")

    target_link_libraries(OpenSSL::SSL INTERFACE
        "${WOLF_EXTERN_DEPS_DIR}/openSSL/lib/${PREFIX_LIB_PATH}/libssl.${STATIC_LIB_FORMAT}")
    target_link_libraries(OpenSSL::Crypto INTERFACE
        "${WOLF_EXTERN_DEPS_DIR}/openSSL/lib/${PREFIX_LIB_PATH}/libcrypto.${STATIC_LIB_FORMAT}")
endfunction()

function(wolf_find_boost)
    message(STATUS "wolf dependency boost")

    list(APPEND HANDLED_BY_EXTERNAL Boost)

    add_library(Boost INTERFACE)

    target_include_directories(Boost INTERFACE "${WOLF_EXTERN_DEPS_DIR}/boost/include")

    if(WIN64)
		if (CMAKE_BUILD_TYPE MATCHES Debug)
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/win/x64/${CMAKE_BUILD_TYPE}/libboost_atomic-vc142-mt-gd-x64-1_75.lib)
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/win/x64/${CMAKE_BUILD_TYPE}/libboost_context-vc142-mt-gd-x64-1_75.lib)
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/win/x64/${CMAKE_BUILD_TYPE}/libboost_fiber-vc142-mt-gd-x64-1_75.lib)
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/win/x64/${CMAKE_BUILD_TYPE}/libboost_date_time-vc142-mt-gd-x64-1_75.lib)
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/win/x64/${CMAKE_BUILD_TYPE}/libboost_regex-vc142-mt-gd-x64-1_75.lib)
		else()
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/win/x64/${CMAKE_BUILD_TYPE}/libboost_atomic-vc142-mt-x64-1_75.lib)
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/win/x64/${CMAKE_BUILD_TYPE}/libboost_context-vc142-mt-x64-1_75.lib)
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/win/x64/${CMAKE_BUILD_TYPE}/libboost_fiber-vc142-mt-s-x64-1_75.lib)
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/win/x64/${CMAKE_BUILD_TYPE}/libboost_date_time-vc142-mt-s-x64-1_75.lib)
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/win/x64/${CMAKE_BUILD_TYPE}/libboost_regex-vc142-mt-s-x64-1_75.lib)
		endif()
    elseif(WIN32)
		if (CMAKE_BUILD_TYPE MATCHES Debug)
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/win/x86/${CMAKE_BUILD_TYPE}/libboost_atomic-vc142-mt-gd-x32-1_75.lib)
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/win/x86/${CMAKE_BUILD_TYPE}/libboost_context-vc142-mt-gd-x32-1_75.lib)
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/win/x86/${CMAKE_BUILD_TYPE}/libboost_fiber-vc142-mt-gd-x32-1_75.lib)
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/win/x86/${CMAKE_BUILD_TYPE}/libboost_date_time-vc142-mt-gd-x32-1_75.lib)
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/win/x86/${CMAKE_BUILD_TYPE}/libboost_regex-vc142-mt-gd-x32-1_75.lib)
		else()
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/win/x86/${CMAKE_BUILD_TYPE}/libboost_atomic-vc142-mt-x32-1_75.lib)
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/win/x86/${CMAKE_BUILD_TYPE}/libboost_context-vc142-mt-x32-1_75.lib)
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/win/x86/${CMAKE_BUILD_TYPE}/libboost_fiber-vc142-mt-s-x32-1_75.lib)
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/win/x86/${CMAKE_BUILD_TYPE}/libboost_date_time-vc142-mt-s-x32-1_75.lib)
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/win/x86/${CMAKE_BUILD_TYPE}/libboost_regex-vc142-mt-s-x32-1_75.lib)
		endif()
    elseif(LINUX)
        if (CMAKE_BUILD_TYPE MATCHES Debug)
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/linux/x64/debug/libboost_atomic.a)
            target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/linux/x64/debug/libboost_context.a)
            target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/linux/x64/debug/libboost_fiber.a)
            target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/linux/x64/debug/libboost_date_time.a)
            target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/linux/x64/debug/libboost_regex.a)
        else()
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/linux/x64/release/libboost_atomic.a)
            target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/linux/x64/release/libboost_context.a)
            target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/linux/x64/release/libboost_fiber.a)
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/linux/x64/release/libboost_date_time.a)
			target_link_libraries(Boost INTERFACE ${WOLF_EXTERN_DEPS_DIR}/boost/lib/linux/x64/release/libboost_regex.a)
        endif()
    endif()
endfunction()

function(wolf_find_quiche)
    message(STATUS "wolf dependency quiche")

    list(APPEND HANDLED_BY_EXTERNAL quiche)

    add_library(quiche INTERFACE)

    target_include_directories(quiche INTERFACE "${WOLF_EXTERN_DEPS_DIR}/quiche/include")

    target_link_libraries(quiche INTERFACE "${WOLF_EXTERN_DEPS_DIR}/quiche/lib/${PREFIX_LIB_PATH}/${CMAKE_BUILD_TYPE}/quiche.lib")
endfunction()

function(wolf_find_activemq_cpp)
    if (NOT UNIX)
        message(FATAL_ERROR "wolf dependency active-mq: it's only available for *nix systems.")
    endif()

    message(STATUS "wolf dependency activemq-cpp")

    FetchContent_Declare(
        activemq_cpp
        GIT_REPOSITORY https://github.com/apache/activemq-cpp
        GIT_TAG        master
    )

    list(APPEND HANDLED_BY_EXTERNAL activemq_cpp)

    FetchContent_GetProperties(activemq_cpp)
    if (NOT activemq_cpp_POPULATED)
        FetchContent_Populate(activemq_cpp)

        message(STATUS "building activemq-cpp at ${activemq_cpp_BINARY_DIR}")

        find_program(MAKE_PROGRAM make REQUIRED)

        add_custom_target(
            activemq-cpp-build
            COMMAND chmod +x ./autogen.sh
            COMMAND ./autogen.sh
            COMMAND ./configure --prefix=${activemq_cpp_BINARY_DIR}
            COMMAND ${MAKE_PROGRAM} install
            WORKING_DIRECTORY "${activemq_cpp_SOURCE_DIR}/activemq-cpp"
            BYPRODUCTS "${activemq_cpp_BINARY_DIR}"
        )

        add_library(activemq-cpp INTERFACE)
        add_library(activemq-cpp::activemq-cpp ALIAS activemq-cpp)

        target_include_directories(activemq-cpp INTERFACE
            ${activemq_cpp_BINARY_DIR}/include/activemq-3.10.0/)
        target_link_libraries(activemq-cpp INTERFACE
            ${activemq_cpp_BINARY_DIR}/lib/libactivemq-cpp.a)

        add_dependencies(activemq-cpp activemq-cpp-build)
    endif()
endfunction()

