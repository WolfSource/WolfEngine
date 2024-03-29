if(NOT DEFINED ENV{VCPKG_ROOT})
    message(FATAL_ERROR "VCPKG_ROOT environment variable is not set.")
endif()

if(QT_IS_ANDROID_MULTI_ABI_EXTERNAL_PROJECT)
    if(CMAKE_TOOLCHAIN_FILE MATCHES "android_x86_64")
        set(vcpkg_triplet "x64-android")
    elseif(CMAKE_TOOLCHAIN_FILE MATCHES "android_x86")
        set(vcpkg_triplet "x86-android")
    elseif(CMAKE_TOOLCHAIN_FILE MATCHES "android_arm64_v8a")
        set(vcpkg_triplet "arm64-android")
    elseif(CMAKE_TOOLCHAIN_FILE MATCHES "android_armv7")
        set(vcpkg_triplet "arm-neon-android")
    endif()
elseif(ANDROID_ABI)
    if(ANDROID_ABI STREQUAL "x86_64")
        set(vcpkg_triplet "x64-android")
    elseif(ANDROID_ABI STREQUAL "x86")
        set(vcpkg_triplet "x86-android")
    elseif(ANDROID_ABI STREQUAL "arm64-v8a")
        set(vcpkg_triplet "arm64-android")
    elseif(ANDROID_ABI STREQUAL "armeabi-v7a")
        set(vcpkg_triplet "arm-neon-android")
    endif()
elseif(EMSCRIPTEN)
    set(vcpkg_triplet "wasm32-emscripten")
else() # desktop
    if(WIN32)
        set(vcpkg_triplet "x64-windows")
    elseif(APPLE)
        set(vcpkg_triplet "x64-osx")
    elseif(UNIX)
        set(vcpkg_triplet "x64-linux")
    endif()
endif()

set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
if (LIBRARY_TYPE STREQUAL "STATIC" AND NOT EMSCRIPTEN)
    set(VCPKG_TARGET_TRIPLET ${vcpkg_triplet}-static CACHE STRING "vcpkg target triplet" FORCE)
else()
    set(VCPKG_TARGET_TRIPLET ${vcpkg_triplet} CACHE STRING "vcpkg target triplet" FORCE)
endif()

include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

function(vcpkg_install PACKAGE PACKAGE_NAME WITH_CONFIG)
    message(STATUS "finding ${PACKAGE}")
    if (WITH_CONFIG)
        find_package(${PACKAGE} CONFIG)
    else()
        find_package(${PACKAGE})
    endif()
    if(NOT ${PACKAGE}_FOUND)
        message(STATUS "installing ${PACKAGE} via vcpkg")
        execute_process(COMMAND vcpkg install ${PACKAGE_NAME} --triplet=${VCPKG_TARGET_TRIPLET})
        if (WITH_CONFIG)
            find_package(${PACKAGE} CONFIG REQUIRED)
        else()
            find_package(${PACKAGE} REQUIRED)
        endif()
    endif()
endfunction()
