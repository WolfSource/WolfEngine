# <Copyright Holder>. Copyright (C) <Copyright Year(s)>. <License>.

#
# Project Properties
#
cmake_minimum_required (VERSION 3.18)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif()

PROJECT (dependencies)
set (APPLICATION_NAME "dependencies")
set (APPLICATION_CODENAME "${PROJECT_NAME}")
set (APPLICATION_VERSION_MAJOR 1)
set (APPLICATION_VERSION_MINOR 0)
set (APPLICATION_VERSION_PATCH 0)
set (APPLICATION_VERSION_TYPE SNAPSHOT)
set (APPLICATION_VERSION_STRING "${APPLICATION_VERSION_MAJOR}.${APPLICATION_VERSION_MINOR}.${APPLICATION_VERSION_PATCH}-${APPLICATION_VERSION_TYPE}")
set (APPLICATION_VENDOR_ID "com.alpha")
set (APPLICATION_VENDOR_NAME "ALPHA")
set (APPLICATION_VENDOR_URL "alpha.com")
set (APPLICATION_ID "${APPLICATION_VENDOR_ID}.${PROJECT_NAME}")

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
	set (CMAKE_PROJECT_NAME ${PROJECT_NAME}d)
else()
	set (CMAKE_PROJECT_NAME ${PROJECT_NAME})
endif()

set(TARGET "" CACHE STRING "target platform")

include(FetchContent)

if("${TARGET}" STREQUAL "ANDROID")
    set(ANDROID TRUE)
elseif("${TARGET}" STREQUAL "IOS")
    set(IOS TRUE)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    if (WIN32)
        set(WIN64 TRUE)
		set(WIN32 FALSE)
		set(ANDROID FALSE)
    endif()
endif()

# Note: Do not modify following properties
if(WIN64)
    set(ARCH "x86_64" CACHE STRING "target architecture")
    set(TARGET_OS "win64")
    set(PLATFORM "win")
elseif(WIN32)
    set(ARCH "x86" CACHE STRING "target architecture")
    set(TARGET_OS "win32")
    set(PLATFORM "win")
elseif(ANDROID)
    set(ARCH "armv7-a" CACHE STRING "target architecture")
    set(TARGET_OS "android")
    set(PLATFORM "android")
elseif(IOS)
    set(ARCH "arm64" CACHE STRING "target architecture")
    set(TARGET_OS "darwin")
    set(PLATFORM "ios")
elseif(APPLE)
    set(ARCH "x86_64" CACHE STRING "target architecture")
    set(TARGET_OS "darwin")
    set(PLATFORM "osx")
endif()

if(IOS)
     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -arch ${ARCH}")
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -arch ${ARCH}")
endif()

if(ANDROID)
    if(NOT CMAKE_TOOLCHAIN_FILE)
            message(FATAL_ERROR "'CMAKE_TOOLCHAIN_FILE' variable is not set. Please set 'CMAKE_TOOLCHAIN_FILE' to the Android toolchain's path.")
    endif()
    if(NOT NDK)
            message(FATAL_ERROR "'NDK' path is not specified. Please set 'NDK' to the Android NDK's path.")
    endif()

    set(ANDROID_TOOLCHAIN_FILE CMAKE_TOOLCHAIN_FILE)
endif()

#
# Debugging Options
#
# set (CMAKE_VERBOSE_MAKEFILE 0) # Use 1 for debugging, 0 for release

if(WIN32 OR WIN64)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

#
# Project Output Paths
#
set (MAINFOLDER ${PROJECT_SOURCE_DIR})

if(NOT EXECUTABLE_OUTPUT_PATH)
    message(WARNING "EXECUTABLE_OUTPUT_PATH has not been specified by the user. Use \"${MAINFOLDER}/bin/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE}\" as the default path.")
    set (EXECUTABLE_OUTPUT_PATH "${MAINFOLDER}/bin/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE}")
endif()
if(NOT LIBRARY_OUTPUT_PATH)
    message(WARNING "LIBRARY_OUTPUT_PATH has not been specified by the user. Use \"${MAINFOLDER}/lib/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE}\" as the default path.")
    set (LIBRARY_OUTPUT_PATH "${MAINFOLDER}/lib/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE}")
endif()

#
# Project Search Paths
#
LIST (APPEND CMAKE_PREFIX_PATH "${MAINFOLDER}")

#
# FFMPEG
#
set (FFMPEGFOLDER "${MAINFOLDER}/FFMPEG/ffmpeg-src")
INCLUDE_DIRECTORIES("${FFMPEGFOLDER}")

#
# Configure Files
#

FILE (GLOB_RECURSE CONFIGINPUTS1 FFMPEG/*.in.h.cmake)
FILE (GLOB_RECURSE CONFIGINPUTS2 FFMPEG/*.h.in.cmake)
LIST (APPEND CONFIGINPUTS "${CONFIGINPUTS1}")
LIST (APPEND CONFIGINPUTS "${CONFIGINPUTS2}")

FOREACH (CONFIGINPUT ${CONFIGINPUTS})
    STRING (REPLACE ".in.h.cmake" ".h" CONFIGOUTPUT1 "${CONFIGINPUT}")
    STRING (REPLACE ".h.in.cmake" ".h" CONFIGOUTPUT2 "${CONFIGOUTPUT1}")
    CONFIGURE_FILE ("${CONFIGINPUT}" "${CONFIGOUTPUT2}")
ENDFOREACH (CONFIGINPUT ${CONFIGINPUTS})

#
# Add Build Targets
#
ADD_SUBDIRECTORY(FFMPEG)

