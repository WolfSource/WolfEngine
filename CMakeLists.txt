cmake_minimum_required(VERSION 3.11)

project(wolf_ctest)

include(FetchContent)

message("fetching https://github.com/catchorg/Catch2.git")
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        devel
)
set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(Catch2)

# enabling clang-tidy
# can be enabled with .CLANG-TIDY from Visual Studio Code
# https://devblogs.microsoft.com/cppblog/visual-studio-code-c-december-2021-update-clang-tidy/
# can be enabled with .CLANG-TIDY from Visual Studio
# https://devblogs.microsoft.com/cppblog/code-analysis-with-clang-tidy-in-visual-studio/
set(CMAKE_C_CLANG_TIDY
     clang-tidy;
     -format-style=file;) 
set(CMAKE_CXX_CLANG_TIDY
     clang-tidy;
     -format-style=file;)  

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug")
endif()
string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

add_executable(${PROJECT_NAME} wolf/c-api/tests.cpp)

# link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2WithMain)
if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/target/${BUILD_TYPE}/wolf.dll.lib)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/target/${BUILD_TYPE}/wolf.dll.a)
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        ${CMAKE_CURRENT_SOURCE_DIR}/target/${BUILD_TYPE}/ 
        ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}
)
