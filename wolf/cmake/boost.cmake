# fetch, include and link boost modules

set(BOOST_MASTER_LIBS 
    boost_leaf
)

if (WOLF_SYSTEM_FIBER)
    list(APPEND BOOST_MASTER_LIBS   
        boost_fiber
        boost_filesystem
        boost_format
    )
endif()

if (WOLF_SYSTEM_SOCKET)
    list(APPEND BOOST_MASTER_LIBS   
        boost_asio
        boost_chrono
        boost_coroutine
        boost_date_time
        boost_io
        boost_lexical_cast
        boost_container
        boost_numeric_conversion
        boost_tokenizer
        boost_ratio
        boost_rational
    )
endif()

if (WOLF_SYSTEM_SIG_SLOT)
    list(APPEND BOOST_MASTER_LIBS 
        boost_parameter
        boost_signals2
        boost_variant
    )
endif()

if (WOLF_SYSTEM_FIBER OR WOLF_SYSTEM_SIG_SLOT)
    list(APPEND BOOST_MASTER_LIBS 
        boost_io
    )
endif()

if (WOLF_SYSTEM_FIBER OR WOLF_SYSTEM_SOCKET)
    list(APPEND BOOST_MASTER_LIBS   
        boost_context
        boost_intrusive
        boost_pool
        boost_algorithm
        boost_exception
        boost_range
        boost_regex
        boost_unordered
        boost_system
        boost_variant2
    )
endif()

if (WOLF_SYSTEM_FIBER OR WOLF_SYSTEM_STACKTRACE)
    list(APPEND BOOST_MASTER_LIBS 
        boost_integer 
)
endif()

if (WOLF_SYSTEM_SOCKET OR WOLF_SYSTEM_SIG_SLOT)
    list(APPEND BOOST_MASTER_LIBS   
        boost_integer
    )
endif()

if (WOLF_SYSTEM_SOCKET OR WOLF_SYSTEM_STACKTRACE)
    list(APPEND BOOST_MASTER_LIBS   
        boost_align
        
    )
endif()

if (WOLF_SYSTEM_FIBER OR WOLF_SYSTEM_SOCKET OR WOLF_SYSTEM_STACKTRACE)
    list(APPEND BOOST_MASTER_LIBS
        boost_array
        boost_winapi
    )
endif()

if (WOLF_SYSTEM_FIBER OR WOLF_SYSTEM_SOCKET OR WOLF_SYSTEM_SIG_SLOT)
    list(APPEND BOOST_MASTER_LIBS   
        boost_smart_ptr
        boost_move
        boost_bind
        boost_concept_check
        boost_function
        boost_functional
        boost_iterator
        boost_mpl
        boost_tuple
        boost_utility
        boost_conversion
        boost_typeof
        boost_function_types
        boost_fusion
        boost_optional
        boost_type_index
    )
endif()

if (WOLF_SYSTEM_FIBER OR WOLF_SYSTEM_SOCKET OR WOLF_SYSTEM_SIG_SLOT OR WOLF_SYSTEM_STACKTRACE)
    list(APPEND BOOST_MASTER_LIBS
        boost_detail
        boost_assert
        boost_describe
        boost_static_assert
        boost_throw_exception
        boost_config
        boost_core
        boost_mp11
        boost_predef
        boost_type_traits
        boost_preprocessor
        boost_container_hash
    )
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

# make them available
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

if (WOLF_SYSTEM_FIBER)
    set_target_properties(boost_fiber_numa PROPERTIES FOLDER "boost")
endif()

# fetch boost stacktrace
if (WOLF_SYSTEM_STACKTRACE)
    message("fetching https://github.com/boostorg/stacktrace.git")
    FetchContent_Declare(
        boost_stacktrace
        GIT_REPOSITORY https://github.com/boostorg/stacktrace.git
        GIT_TAG        master
    )
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