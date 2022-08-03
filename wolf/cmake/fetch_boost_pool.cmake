# fetch boost pool deps
message("fetching https://github.com/boostorg/assert.git")
    FetchContent_Declare(
    boost_assert
    GIT_REPOSITORY https://github.com/boostorg/assert.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/config.git")
    FetchContent_Declare(
    boost_config
    GIT_REPOSITORY https://github.com/boostorg/config.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/core.git")
    FetchContent_Declare(
    boost_core
    GIT_REPOSITORY https://github.com/boostorg/core.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/integer.git")
    FetchContent_Declare(
    boost_integer
    GIT_REPOSITORY https://github.com/boostorg/integer.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/pool.git")
    FetchContent_Declare(
    boost_pool
    GIT_REPOSITORY https://github.com/boostorg/pool.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/predef.git")
    FetchContent_Declare(
    boost_predef
    GIT_REPOSITORY https://github.com/boostorg/predef.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/preprocessor.git")
FetchContent_Declare(
  boost_preprocessor
  GIT_REPOSITORY https://github.com/boostorg/preprocessor.git
  GIT_TAG        master
)
message("fetching https://github.com/boostorg/static_assert.git")
    FetchContent_Declare(
    boost_static_assert
    GIT_REPOSITORY https://github.com/boostorg/static_assert.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/throw_exception.git")
    FetchContent_Declare(
    boost_throw_exception
    GIT_REPOSITORY https://github.com/boostorg/throw_exception.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/type_traits.git")
    FetchContent_Declare(
    boost_type_traits
    GIT_REPOSITORY https://github.com/boostorg/type_traits.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/winapi.git")
    FetchContent_Declare(
    boost_winapi
    GIT_REPOSITORY https://github.com/boostorg/winapi.git
    GIT_TAG        master
)

set(BOOST_BASIC_LIBS 
    boost_assert 
    boost_config
    boost_core
    boost_integer
    boost_pool
    boost_predef
    boost_preprocessor
    boost_static_assert
    boost_throw_exception
    boost_type_traits
    boost_winapi)

# make them available
FetchContent_MakeAvailable(${BOOST_BASIC_LIBS})

# add includes
list(APPEND INCLUDES
    ${boost_assert_SOURCE_DIR}/include
    ${boost_config_SOURCE_DIR}/include
    ${boost_core_SOURCE_DIR}/include
    ${boost_integer_SOURCE_DIR}/include
    ${boost_pool_SOURCE_DIR}/include
    ${boost_predef_SOURCE_DIR}/include
    ${boost_static_assert_SOURCE_DIR}/include
    ${boost_throw_exception_SOURCE_DIR}/include
    ${boost_type_traits_SOURCE_DIR}/include
    ${boost_winapi_SOURCE_DIR}/include
)

# add libraries
list(APPEND LIBS ${BOOST_BASIC_LIBS})   