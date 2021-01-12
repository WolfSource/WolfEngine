# CMAKE required version
cmake_minimum_required (VERSION 3.11)
# set the project name
project (wolf_system)

# required packages
find_package(Git REQUIRED)
# FetchContent for cloning repositories, avaiable since CMAKE 3.11
include(FetchContent)

# CMAKE GUI Options
option(WOLF_ENABLE_AVX512 "Enable AVX512" OFF)
option(WOLF_ENABLE_CXX_EXTENSIONS "Enable CXX Extensions" OFF)
option(WOLF_ENABLE_ACTIVEMQ "Enable ActiveMQ" OFF)
option(WOLF_ENABLE_CURL "Enable CURL" OFF)
option(WOLF_ENABLE_DB_CASSANDRA "Enable Cassandra DB" OFF)
option(WOLF_ENABLE_FIBER "Enable Fiber" OFF)
option(WOLF_ENABLE_LUA "Enable Lua Scripting" OFF)
option(WOLF_ENABLE_LZ4 "Enable LZ4 Compression" OFF)
option(WOLF_ENABLE_LZMA "Enable LZMA Compression" OFF)
option(WOLF_ENABLE_PYTHON3 "Enable Python3 Scripting" OFF)
option(WOLF_ENABLE_QUIC "Enable QUIC protocol" OFF)
option(WOLF_ENABLE_WEBSOCKET "Enable Websocket" OFF)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ENABLE_CXX_EXTENSIONS)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# fetch libexpat
message("fetching https://github.com/libexpat/libexpat.git")
FetchContent_Declare(
  libexpat
  GIT_REPOSITORY https://github.com/libexpat/libexpat.git
  GIT_TAG        R_2_2_10
  SOURCE_SUBDIR  expat 
)
# fetch apr
message("fetching https://github.com/apache/apr.git")
FetchContent_Declare(
  apr
  GIT_REPOSITORY https://github.com/apache/apr.git
  GIT_TAG        1.7.0
)

# change some options of CMAKE dependencies
set(EXPAT_SHARED_LIBS OFF CACHE BOOL "EXPAT_SHARED_LIBS")
set(CMAKE_DEBUG_POSTFIX "" CACHE STRING "" FORCE)
set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "" FORCE)
set(CMAKE_MINSIZEREL_POSTFIX "" CACHE STRING "" FORCE)
set(CMAKE_RELWITHDEBINFO_POSTFIX "" CACHE STRING "" FORCE)

# After the following call, the CMake targets will be defined and available to the rest of the build
FetchContent_MakeAvailable(libexpat	apr)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
else()
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()
 
# create filter for each folder
file(GLOB_RECURSE BASE64_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/base64/*"
)
file(GLOB_RECURSE CHRONO_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/chrono/*"
)
file(GLOB_RECURSE COMPRESSION_LZ4_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/compression/lz4/*"
)
file(GLOB_RECURSE COMPRESSION_LZMA_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/compression/lzma/*"
)
file(GLOB_RECURSE COMPRESSION_MSGPACK_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/compression/msgpack/msgpack.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/compression/msgpack/objectc.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/compression/msgpack/unpack.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/compression/msgpack/version.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/compression/msgpack/vrefbuffer.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/compression/msgpack/zone.c"
)
file(GLOB_RECURSE COMPRESSION_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/compression/w_compress.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/compression/w_compress.h"
)
file(GLOB_RECURSE CONCURRENCY_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_atomic.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_atomic.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_concurrent_queue.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_concurrent_queue.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_condition_variable.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_condition_variable.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_mutex.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_mutex.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_signal.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_signal.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_sigslot.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_sigslot.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_thread.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_thread.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_thread_pool.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_thread_pool.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_timer.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/concurrency/w_timer.h"
)
file(GLOB_RECURSE FIBER_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/fiber/*"
)
file(GLOB_RECURSE IO_STB_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/io/stb/*"
)
file(GLOB_RECURSE IO_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/io/w_io.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/io/w_io.h"
)
file(GLOB_RECURSE JSON_RAPIDJSON_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/json/rapidjson/*"
)
file(GLOB_RECURSE LOG_SPDLOG_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/log/spdlog/src/async.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/log/spdlog/src/cfg.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/log/spdlog/src/color_sinks.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/log/spdlog/src/file_sinks.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/log/spdlog/src/fmt.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/log/spdlog/src/spdlog.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/log/spdlog/src/stdout_sinks.cpp"
)
file(GLOB_RECURSE LOG_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/log/logger.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/log/logger.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/log/w_log.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/log/w_log.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/log/w_log_config.h"
)
file(GLOB_RECURSE MEMORY_BUCKETS_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/memory/buckets/*"
)
file(GLOB_RECURSE MEMORY_HASH_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/memory/hash/*"
)
file(GLOB_RECURSE MEMORY_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/memory/w_array.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/memory/w_array.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/memory/w_hash.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/memory/w_hash.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/memory/w_mem_cache.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/memory/w_mem_cache.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/memory/w_mem_map.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/memory/w_mem_map.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/memory/w_mem_pool.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/memory/w_mem_pool.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/memory/w_shared_mem.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/memory/w_shared_mem.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/memory/w_string.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/memory/w_string.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/memory/w_table.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/memory/w_table.h"
)
file(GLOB_RECURSE NET_AMQ_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/net/amq/*"
)
file(GLOB_RECURSE NET_WS_USOCKETS_CRYPTO_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/net/ws/uSockets/crypto/*"
)
file(GLOB_RECURSE NET_WS_USOCKETS_EVENTING_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/net/ws/uSockets/eventing/*"
)
file(GLOB_RECURSE NET_WS_USOCKETS_INTERNAL_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/net/ws/uSockets/internal/*"
)
file(GLOB_RECURSE NET_WS_USOCKETS_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/net/ws/uSockets/bsd.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/net/ws/uSockets/context.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/net/ws/uSockets/libusockets.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/net/ws/uSockets/loop.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/net/ws/uSockets/socket.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/net/ws/uSockets/verson.txt"
)
file(GLOB_RECURSE NET_WS_UWEBSOCKETS_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/net/ws/uWebSockets/*"
)
file(GLOB_RECURSE NET_WS_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/net/ws/uws.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/net/ws/uws.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/net/ws/ws.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/net/ws/ws.h"
)
file(GLOB_RECURSE NET_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/net/w_net.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/net/w_net.h"
)
file(GLOB_RECURSE OS_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/os/*"
)
file(GLOB_RECURSE SCRIPT_LUA_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/script/lua/*"
)
file(GLOB_RECURSE SCRIPT_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/script/w_lua.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/script/w_lua.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/script/w_python.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/script/w_python.h"
)
file(GLOB_RECURSE WOLF_SRC_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/wolf.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/wolf.h"
)

# add includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/_deps/libexpat-src/expat/lib
	${CMAKE_CURRENT_BINARY_DIR}/_deps/apr-build
	${CMAKE_CURRENT_BINARY_DIR}/_deps/apr-src/include
	${CMAKE_CURRENT_SOURCE_DIR}/log
	${CMAKE_CURRENT_SOURCE_DIR}/compression/msgpack	
)
		
# add sources
add_library(${PROJECT_NAME} SHARED
	${BASE64_SRC_DIR}
	${CHRONO_SRC_DIR}
	${COMPRESSION_LZ4_SRC_DIR}
	${COMPRESSION_LZMA_SRC_DIR}
	${COMPRESSION_MSGPACK_SRC_DIR}
	${COMPRESSION_SRC_DIR}
	${CONCURRENCY_SRC_DIR}
	${FIBER_SRC_DIR}
	${IO_STB_SRC_DIR}
	${IO_SRC_DIR}
	${JSON_RAPIDJSON_SRC_DIR}
	${LOG_SPDLOG_SRC_DIR}
	${LOG_SRC_DIR}
	${MEMORY_BUCKETS_SRC_DIR}
	${MEMORY_HASH_SRC_DIR}
	${MEMORY_SRC_DIR}
	${NET_AMQ_SRC_DIR}
	${NET_WS_USOCKETS_CRYPTO_SRC_DIR}
	${NET_WS_USOCKETS_EVENTING_SRC_DIR}
	${NET_WS_USOCKETS_INTERNAL_SRC_DIR} 
	${NET_WS_USOCKETS_SRC_DIR}
	${NET_WS_UWEBSOCKETS_SRC_DIR}
	${NET_WS_SRC_DIR} 
	${NET_SRC_DIR}
	${OS_SRC_DIR}
	${SCRIPT_LUA_SRC_DIR}
	${SCRIPT_SRC_DIR}
	${WOLF_SRC_DIR}
 )

 source_group("base64" FILES ${BASE64_SRC_DIR}) 
 source_group("chrono" FILES ${CHRONO_SRC_DIR}) 
 source_group("compression/lz4" FILES ${COMPRESSION_LZ4_SRC_DIR}) 
 source_group("compression/lzma" FILES ${COMPRESSION_LZMA_SRC_DIR}) 
 source_group("compression/msgpack" FILES ${COMPRESSION_MSGPACK_SRC_DIR}) 
 source_group("compression" FILES ${COMPRESSION_SRC_DIR}) 
 source_group("concurrency" FILES ${CONCURRENCY_SRC_DIR}) 
 source_group("fiber" FILES ${FIBER_SRC_DIR}) 
 source_group("io/stb" FILES ${IO_STB_SRC_DIR}) 
 source_group("io" FILES ${IO_SRC_DIR}) 
 source_group("json/rapidjson" FILES ${JSON_RAPIDJSON_SRC_DIR}) 
 source_group("log/spdlog/src" FILES ${LOG_SPDLOG_SRC_DIR}) 
 source_group("log" FILES ${LOG_SRC_DIR}) 
 source_group("memory/buckets" FILES ${MEMORY_BUCKETS_SRC_DIR})
 source_group("memory/hash" FILES ${MEMORY_HASH_SRC_DIR})
 source_group("memory" FILES ${MEMORY_SRC_DIR}) 
 source_group("net/amq" FILES ${NET_AMQ_SRC_DIR})
 source_group("net/ws/uSockets/crypto" FILES ${NET_WS_USOCKETS_CRYPTO_SRC_DIR})
 source_group("net/ws/uSockets/eventing" FILES ${NET_WS_USOCKETS_EVENTING_SRC_DIR})
 source_group("net/ws/uSockets/internal" FILES ${NET_WS_USOCKETS_INTERNAL_SRC_DIR})
 source_group("net/ws/uSockets" FILES ${NET_WS_USOCKETS_SRC_DIR})
 source_group("net/ws/uWebSockets" FILES ${NET_WS_UWEBSOCKETS_SRC_DIR})
 source_group("net/ws" FILES ${NET_WS_SRC_DIR})
 source_group("net" FILES ${NET_SRC_DIR})
 source_group("os" FILES ${OS_SRC_DIR})
 source_group("script/lua" FILES ${SCRIPT_LUA_SRC_DIR})
 source_group("script" FILES ${SCRIPT_SRC_DIR})
 source_group("wolf" FILES ${WOLF_SRC_DIR})
  
 # set global compiler options
target_compile_definitions(${PROJECT_NAME} PUBLIC
        APR_DECLARE_EXPORT
		libapr_1_EXPORTS
		APU_DECLARE_STATIC
		APU_DSO_MODULE_BUILD
		XML_STATIC
		expat_EXPORTS
		_CRT_RAND_S
		NO_FSEEKO
		ASMV
		ASMINF
		SPDLOG_COMPILED_LIB
		SPDLOG_WCHAR_FILENAMES
		SPDLOG_USE_WCHAR
		LIBUS_USE_LIBUV
		LIBUS_USE_OPENSSL
		EV_STANDALONE
		)

if (CMAKE_BUILD_TYPE MATCHES Debug)	
	target_compile_definitions(${PROJECT_NAME} PUBLIC
		DEBUG
	)
else() 
	target_compile_definitions(${PROJECT_NAME} PUBLIC
		NDEBUG
	)
endif()

 # set AVX512 compile definition
if (ENABLE_AVX512)
	target_compile_definitions(${PROJECT_NAME} PUBLIC
		USE_AVX512=1
	)
endif()

 # set activeMQ compile definitions
if (W_ENABLE_ACTIVEMQ)
	target_compile_definitions(${PROJECT_NAME} PUBLIC
		W_ENABLE_ACTIVEMQ
		_HAS_AUTO_PTR_ETC=1
	)
endif()

 # set LUA compile definitions
if (W_ENABLE_LUA)
	target_compile_definitions(${PROJECT_NAME} PUBLIC
		W_ENABLE_LUA=1
	)
endif()
		
# set Python3 compile definitions
if (W_ENABLE_PYTHON3)
	target_compile_definitions(${PROJECT_NAME} PUBLIC
		W_ENABLE_PYTHON3=1
	)
endif()

# set QUIC compile definitions
if (W_ENABLE_QUIC)
	target_compile_definitions(${PROJECT_NAME} PUBLIC
		W_ENABLE_QUIC=1
	)
endif()

#set CURL compile definitions
if (W_ENABLE_CURL)
	target_compile_definitions(${PROJECT_NAME} PUBLIC
		W_ENABLE_CURL=1
		CURL_STATICLIB
	)
endif()

#set cassandra compile definitions
if (W_ENABLE_DB_CASSANDRA)
	target_compile_definitions(${PROJECT_NAME} PUBLIC
		W_ENABLE_DB_CASSANDRA=1
	)
endif()

if (WIN32)
	target_link_libraries(${PROJECT_NAME} PRIVATE  expat apr-1 Shlwapi)
else()
	target_link_libraries(${PROJECT_NAME} PRIVATE  expat apr-1)
endif()