# fetch, include and link boost modules

set(BOOST_MASTER_LIBS 
    boost_leaf
)

if (NOT EMSCRIPTEN)
    if (WOLF_SYSTEM_SOCKET AND WOLF_SYSTEM_SSL)
        # disable TLS for asio
        add_definitions(-DASIO_NO_TS_EXECUTORS)
    endif()

    if (WOLF_SYSTEM_HTTP1_WS)
        if (NOT WOLF_SYSTEM_SOCKET)
            message(FATAL_ERROR "WOLF_SYSTEM_HTTP1_WS needs WOLF_SYSTEM_SOCKET module")
        endif()

        list(APPEND BOOST_MASTER_LIBS   
            boost_beast
            boost_endian
            boost_logic
            boost_static_string
        )
    endif()

    if (WOLF_TEST OR WOLF_SYSTEM_SOCKET)
        list(APPEND BOOST_MASTER_LIBS   
            boost_algorithm
            boost_asio
            boost_chrono
            boost_container
            boost_context
            boost_coroutine
            boost_date_time
            boost_exception
            boost_intrusive
            boost_io
            boost_lexical_cast
            boost_numeric_conversion
            boost_pool
            boost_range
            boost_ratio
            boost_rational
            boost_regex
            boost_system
            boost_tokenizer
            boost_unordered
            boost_variant2
        )
    endif()
    
    if (WOLF_SYSTEM_SIG_SLOT)
        list(APPEND BOOST_MASTER_LIBS 
            boost_io
            boost_parameter
            boost_signals2
            boost_variant
        )
    endif()
    
    if (WOLF_TEST OR WOLF_SYSTEM_SOCKET OR WOLF_SYSTEM_STACKTRACE)
        list(APPEND BOOST_MASTER_LIBS   
            boost_align
            boost_array
            boost_integer
            boost_winapi
    )
    endif()
    
    if (WOLF_TEST OR WOLF_SYSTEM_SOCKET OR WOLF_SYSTEM_SIG_SLOT)
        list(APPEND BOOST_MASTER_LIBS   
            boost_bind
            boost_concept_check
            boost_conversion
            boost_function
            boost_function_types
            boost_functional
            boost_fusion
            boost_iterator
            boost_move
            boost_mpl
            boost_optional
            boost_smart_ptr
            boost_tuple
            boost_type_index
            boost_typeof
            boost_utility
        )
    endif()
    
    if (WOLF_TEST OR WOLF_SYSTEM_SOCKET OR WOLF_SYSTEM_SIG_SLOT OR WOLF_SYSTEM_STACKTRACE)
        list(APPEND BOOST_MASTER_LIBS
            boost_assert
            boost_config
            boost_container_hash
            boost_core
            boost_describe
            boost_detail
            boost_mp11
            boost_predef
            boost_preprocessor
            boost_static_assert
            boost_throw_exception
            boost_type_traits
        )
    endif()
endif()

foreach (_var ${BOOST_MASTER_LIBS})
    string(REPLACE "boost_" "" lib_name ${_var})
    set(git_url "https://github.com/boostorg/${lib_name}.git")
    message("fetching ${git_url}")
        FetchContent_Declare(
        ${_var}
        GIT_REPOSITORY ${git_url}
        GIT_TAG        master
    )
endforeach()

if (WOLF_SYSTEM_HTTP1_WS AND NOT EMSCRIPTEN)
    set(Beast_BUILD_EXAMPLES OFF CACHE BOOL "Beast_BUILD_EXAMPLES")
    set(Beast_BUILD_TESTS OFF CACHE BOOL "Beast_BUILD_TESTS")
    set(Boost_USE_STATIC_LIBS ON CACHE BOOL "Beast_USE_STATIC_LIBS")
endif()

# make them available
set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(${BOOST_MASTER_LIBS})

# add includes
foreach (_var ${BOOST_MASTER_LIBS})
    set(source_dir "${${_var}_SOURCE_DIR}") 
    list(APPEND INCLUDES
        ${source_dir}/include
    )
    set_target_properties(${_var} PROPERTIES FOLDER "boost")
endforeach()

# add libraries
list(APPEND LIBS ${BOOST_MASTER_LIBS})   

# fetch boost stacktrace
if (WOLF_SYSTEM_STACKTRACE)
    message("fetching https://github.com/boostorg/stacktrace.git")
    FetchContent_Declare(
        boost_stacktrace
        GIT_REPOSITORY https://github.com/boostorg/stacktrace.git
        GIT_TAG        master
    )
    set(FETCHCONTENT_QUIET OFF)
    FetchContent_MakeAvailable(boost_stacktrace)
    list(APPEND INCLUDES 
        ${boost_stacktrace_SOURCE_DIR}/include
        ${boost_stacktrace_BINARY_DIR}/include
    )
    if(MSVC)
        list(APPEND LIBS boost_stacktrace_windbg)   
        set_target_properties(
            boost_stacktrace_basic 
            boost_stacktrace_noop
            boost_stacktrace_windbg
            PROPERTIES FOLDER "boost")
    else()
        list(APPEND LIBS boost_stacktrace_noop)
        set_target_properties(
            boost_stacktrace_basic 
            boost_stacktrace_noop
            PROPERTIES FOLDER "boost")
    endif() 
endif()

if (WOLF_TEST)
    message("fetching https://github.com/boostorg/test.git")
    FetchContent_Declare(
        boost_test
        GIT_REPOSITORY https://github.com/boostorg/test.git
        GIT_TAG        master
    )
    FetchContent_Populate(boost_test)
    list(APPEND INCLUDES
        ${boost_test_SOURCE_DIR}/include
    )
endif()