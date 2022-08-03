
message("fetching https://github.com/boostorg/intrusive.git")
    FetchContent_Declare(
    boost_intrusive
    GIT_REPOSITORY https://github.com/boostorg/intrusive.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/filesystem.git")
    FetchContent_Declare(
    boost_filesystem
    GIT_REPOSITORY https://github.com/boostorg/filesystem.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/format.git")
    FetchContent_Declare(
    boost_format
    GIT_REPOSITORY https://github.com/boostorg/format.git
    GIT_TAG        master
)
message("fetching https://github.com/boostorg/atomic.git")
    FetchContent_Declare(
    boost_atomic
    GIT_REPOSITORY https://github.com/boostorg/atomic.git
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
message("fetching https://github.com/boostorg/fiber.git")
    FetchContent_Declare(
    boost_fiber
    GIT_REPOSITORY https://github.com/boostorg/fiber.git
    GIT_TAG        develop
)

set(BOOST_FIBER_LIBS 
    boost_atomic    
    boost_fiber
    boost_filesystem
    boost_format
    boost_intrusive
    boost_move   
    boost_range)
    
# make them available
FetchContent_MakeAvailable(${BOOST_FIBER_LIBS})

# add includes
list(APPEND INCLUDES
    ${boost_atomic_SOURCE_DIR}/include
    ${boost_fiber_SOURCE_DIR}/include
    ${boost_filesystem_SOURCE_DIR}/include
    ${boost_format_SOURCE_DIR}/include
    ${boost_intrusive_SOURCE_DIR}/include
    ${boost_move_SOURCE_DIR}/include
    ${boost_range_SOURCE_DIR}/include
)

# add libraries
list(APPEND LIBS ${BOOST_FIBER_LIBS})