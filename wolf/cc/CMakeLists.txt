cmake_minimum_required(VERSION 3.22)

project(wolf_cc)

# required packages
find_package(Git REQUIRED)
if (LINUX)
    find_package(Wget REQUIRED)
    message("make sure libtool was installed.(apt-get install libtool-bin)")
endif()
# FetchContent for cloning repositories, avaiable since CMAKE 3.11
include(FetchContent)

set(LIBRARY_TYPE "STATIC" CACHE STRING "Library type")
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# fetch libnng
message("fetching https://github.com/nanomsg/nng.git")
FetchContent_Declare(
  libnng
  GIT_REPOSITORY https://github.com/nanomsg/nng.git
  GIT_TAG        master
)

# change some options of CMAKE dependencies
# set(EXPAT_SHARED_LIBS OFF CACHE BOOL "EXPAT_SHARED_LIBS")

FetchContent_MakeAvailable(libnng)

# add includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/_deps/libnng-src/include
)

add_library(${PROJECT_NAME} ${LIBRARY_TYPE}
    wolf.cpp
    dllmain.cpp
)
target_link_libraries(${PROJECT_NAME} PRIVATE nng)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -fPIC)