message("fetching https://github.com/boostorg/asio.git")
    FetchContent_Declare(
    boost_asio
    GIT_REPOSITORY https://github.com/boostorg/asio.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/chrono.git")
    FetchContent_Declare(
    boost_chrono
    GIT_REPOSITORY https://github.com/boostorg/chrono.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/coroutine.git")
    FetchContent_Declare(
    boost_coroutine
    GIT_REPOSITORY https://github.com/boostorg/coroutine.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/date_time.git")
    FetchContent_Declare(
    boost_date_time
    GIT_REPOSITORY https://github.com/boostorg/date_time.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/lexical_cast.git")
    FetchContent_Declare(
    boost_lexical_cast
    GIT_REPOSITORY https://github.com/boostorg/lexical_cast.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/numeric_conversion.git")
    FetchContent_Declare(
    boost_numeric_conversion
    GIT_REPOSITORY https://github.com/boostorg/numeric_conversion.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/ratio.git")
    FetchContent_Declare(
    boost_ratio
    GIT_REPOSITORY https://github.com/boostorg/ratio.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/rational.git")
    FetchContent_Declare(
    boost_rational
    GIT_REPOSITORY https://github.com/boostorg/rational.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/tokenizer.git")
    FetchContent_Declare(
    boost_tokenizer
    GIT_REPOSITORY https://github.com/boostorg/tokenizer.git
    GIT_TAG        master
)

set(BOOST_ASIO_LIBS 
    boost_asio
    boost_chrono
    boost_coroutine
    boost_date_time
    boost_lexical_cast
    boost_numeric_conversion
    boost_ratio
    boost_rational
    boost_tokenizer)

# make them available
FetchContent_MakeAvailable(${BOOST_ASIO_LIBS})

# add includes
list(APPEND INCLUDES
    ${boost_asio_SOURCE_DIR}/include
    ${boost_chrono_SOURCE_DIR}/include
    ${boost_coroutine_SOURCE_DIR}/include
    ${boost_date_time_SOURCE_DIR}/include
    ${boost_lexical_cast_SOURCE_DIR}/include
    ${boost_numeric_conversion_SOURCE_DIR}/include
    ${boost_ratio_SOURCE_DIR}/include
    ${boost_rational_SOURCE_DIR}/include
    ${boost_tokenizer_SOURCE_DIR}/include
)

# add libraries
list(APPEND LIBS ${BOOST_ASIO_LIBS}) 