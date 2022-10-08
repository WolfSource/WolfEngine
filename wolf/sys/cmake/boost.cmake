# fetch, include and link boost modules

if (WOLF_SYSTEM_STACKTRACE)
    set(BOOST_MASTER_LIBS 
        boost_align
        boost_array
        boost_assert
        boost_config
        boost_container_hash
        boost_core
        boost_detail
        boost_integer
        boost_predef
        boost_preprocessor
        boost_static_assert
        boost_throw_exception
        boost_type_traits
        boost_winapi 
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
    FetchContent_MakeAvailable(boost_stacktrace)
    list(APPEND INCLUDES 
        ${boost_stacktrace_SOURCE_DIR}/include
        ${boost_stacktrace_BINARY_DIR}/include
    )
    if(MSVC)
        list(APPEND LIBS boost_stacktrace_windbg)   
    else()
        list(APPEND LIBS boost_stacktrace_noop)
    endif()
endif()

if (WOLF_SYSTEM_STACKTRACE)
    if(MSVC)
        set_target_properties(
            boost_stacktrace_basic 
            boost_stacktrace_noop
            boost_stacktrace_windbg
            PROPERTIES FOLDER "boost")
    else()
        set_target_properties(
            boost_stacktrace_basic 
            boost_stacktrace_noop
            PROPERTIES FOLDER "boost")
    endif()
endif()