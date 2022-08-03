# fetch boost basics
message("fetching https://github.com/boostorg/algorithm.git")
    FetchContent_Declare(
    boost_algorithm
    GIT_REPOSITORY https://github.com/boostorg/algorithm.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/align.git")
    FetchContent_Declare(
    boost_align
    GIT_REPOSITORY https://github.com/boostorg/align.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/array.git")
    FetchContent_Declare(
    boost_array
    GIT_REPOSITORY https://github.com/boostorg/array.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/bind.git")
    FetchContent_Declare(
    boost_bind
    GIT_REPOSITORY https://github.com/boostorg/bind.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/concept_check.git")
    FetchContent_Declare(
    boost_concept_check
    GIT_REPOSITORY https://github.com/boostorg/concept_check.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/container.git")
    FetchContent_Declare(
    boost_container
    GIT_REPOSITORY https://github.com/boostorg/container.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/container_hash.git")
    FetchContent_Declare(
    boost_container_hash
    GIT_REPOSITORY https://github.com/boostorg/container_hash.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/context.git")
    FetchContent_Declare(
    boost_context
    GIT_REPOSITORY https://github.com/boostorg/context.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/conversion.git")
    FetchContent_Declare(
    boost_conversion
    GIT_REPOSITORY https://github.com/boostorg/conversion.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/detail.git")
    FetchContent_Declare(
    boost_detail
    GIT_REPOSITORY https://github.com/boostorg/detail.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/exception.git")
    FetchContent_Declare(
    boost_exception
    GIT_REPOSITORY https://github.com/boostorg/exception.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/function.git")
    FetchContent_Declare(
    boost_function
    GIT_REPOSITORY https://github.com/boostorg/function.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/function_types.git")
    FetchContent_Declare(
    boost_function_types
    GIT_REPOSITORY https://github.com/boostorg/function_types.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/fusion.git")
    FetchContent_Declare(
    boost_fusion
    GIT_REPOSITORY https://github.com/boostorg/fusion.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/iterator.git")
    FetchContent_Declare(
    boost_iterator
    GIT_REPOSITORY https://github.com/boostorg/iterator.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/intrusive.git")
    FetchContent_Declare(
    boost_intrusive
    GIT_REPOSITORY https://github.com/boostorg/intrusive.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/io.git")
    FetchContent_Declare(
    boost_io
    GIT_REPOSITORY https://github.com/boostorg/io.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/move.git")
    FetchContent_Declare(
    boost_move
    GIT_REPOSITORY https://github.com/boostorg/move.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/mp11.git")
    FetchContent_Declare(
    boost_mp11
    GIT_REPOSITORY https://github.com/boostorg/mp11.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/mpl.git")
    FetchContent_Declare(
    boost_mpl
    GIT_REPOSITORY https://github.com/boostorg/mpl.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/optional.git")
    FetchContent_Declare(
    boost_optional
    GIT_REPOSITORY https://github.com/boostorg/optional.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/range.git")
    FetchContent_Declare(
    boost_range
    GIT_REPOSITORY https://github.com/boostorg/range.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/regex.git")
    FetchContent_Declare(
    boost_regex
    GIT_REPOSITORY https://github.com/boostorg/regex.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/smart_ptr.git")
    FetchContent_Declare(
    boost_smart_ptr
    GIT_REPOSITORY https://github.com/boostorg/smart_ptr.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/system.git")
    FetchContent_Declare(
    boost_system
    GIT_REPOSITORY https://github.com/boostorg/system.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/tuple.git")
    FetchContent_Declare(
    boost_tuple
    GIT_REPOSITORY https://github.com/boostorg/tuple.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/typeof.git")
    FetchContent_Declare(
    boost_typeof
    GIT_REPOSITORY https://github.com/boostorg/typeof.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/type_index.git")
    FetchContent_Declare(
    boost_type_index
    GIT_REPOSITORY https://github.com/boostorg/type_index.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/unordered.git")
    FetchContent_Declare(
    boost_unordered
    GIT_REPOSITORY https://github.com/boostorg/unordered.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/utility.git")
    FetchContent_Declare(
    boost_utility
    GIT_REPOSITORY https://github.com/boostorg/utility.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/variant2.git")
    FetchContent_Declare(
    boost_variant2
    GIT_REPOSITORY https://github.com/boostorg/variant2.git
    GIT_TAG        master
)

set(BOOST_BASIC_LIBS 
    boost_algorithm
    boost_align
    boost_array
    boost_bind
    boost_concept_check
    boost_container
    boost_container_hash
    boost_context
    boost_conversion
    boost_detail
    boost_exception
    boost_function
    boost_function_types
    boost_fusion
    boost_intrusive
    boost_io
    boost_iterator
    boost_move
    boost_mp11
    boost_mpl
    boost_optional
    boost_range
    boost_regex
    boost_smart_ptr
    boost_system
    boost_tuple
    boost_type_index
    boost_typeof
    boost_unordered
    boost_utility
    boost_variant2)

# make them available
FetchContent_MakeAvailable(${BOOST_BASIC_LIBS})

# add includes
list(APPEND INCLUDES
    ${boost_algorithm_SOURCE_DIR}/include
    ${boost_align_SOURCE_DIR}/include
    ${boost_array_SOURCE_DIR}/include
    ${boost_bind_SOURCE_DIR}/include
    ${boost_concept_check_SOURCE_DIR}/include
    ${boost_container_hash_SOURCE_DIR}/include
    ${boost_container_SOURCE_DIR}/include
    ${boost_context_SOURCE_DIR}/include
    ${boost_conversion_SOURCE_DIR}/include
    ${boost_detail_SOURCE_DIR}/include
    ${boost_exception_SOURCE_DIR}/include
    ${boost_function_SOURCE_DIR}/include
    ${boost_function_types_SOURCE_DIR}/include
    ${boost_fusion_SOURCE_DIR}/include
    ${boost_intrusive_SOURCE_DIR}/include
    ${boost_io_SOURCE_DIR}/include
    ${boost_iterator_SOURCE_DIR}/include
    ${boost_move_SOURCE_DIR}/include
    ${boost_mp11_SOURCE_DIR}/include
    ${boost_mpl_SOURCE_DIR}/include
    ${boost_optional_SOURCE_DIR}/include
    ${boost_range_SOURCE_DIR}/include
    ${boost_regex_SOURCE_DIR}/include
    ${boost_smart_ptr_SOURCE_DIR}/include
    ${boost_system_SOURCE_DIR}/include
    ${boost_tuple_SOURCE_DIR}/include
    ${boost_type_index_SOURCE_DIR}/include
    ${boost_typeof_SOURCE_DIR}/include
    ${boost_unordered_SOURCE_DIR}/include
    ${boost_utility_SOURCE_DIR}/include
    ${boost_variant2_SOURCE_DIR}/include
)

# add libraries
list(APPEND LIBS ${BOOST_BASIC_LIBS})   