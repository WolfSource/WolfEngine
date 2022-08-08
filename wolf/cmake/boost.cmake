# fetch, include and link boost modules

# boost signals2
set(BOOST_MASTER_LIBS 
    boost_assert
    boost_bind
    boost_concept_check
    boost_config
    boost_container_hash
    boost_conversion
    boost_core
    boost_detail
    boost_function
    boost_function_types
    boost_fusion
    boost_integer
    boost_io
    boost_iterator
    boost_move
    boost_mp11
    boost_mpl
    boost_optional
    boost_parameter
    boost_predef
    boost_preprocessor
    boost_signals2
    boost_smart_ptr
    boost_static_assert
    boost_throw_exception
    boost_tuple
    boost_type_index
    boost_type_traits
    boost_typeof
    boost_utility
    boost_variant
)

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
    set(source_dir "${_var}_SOURCE_DIR") 
    list(APPEND INCLUDES
        ${source_dir}/include
    )
endforeach()

# add libraries
list(APPEND LIBS ${BOOST_MASTER_LIBS})   