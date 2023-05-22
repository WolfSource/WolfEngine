#!/bin/bash

while [ "$1" != "" ]; do
    case $1 in
        --build_dir=* )             BUILD_DIR="${1#*=}"
                                    ;;
        esac
        shift
done

TOOLCHAIN="${NDK}/build/cmake/android.toolchain.cmake"

HOST_ROOT="${NDK}/toolchains/llvm/prebuilt/darwin-x86_64"
SYS_ROOT="${HOST_ROOT}/sysroot"
LIB_PATH="${HOST_ROOT}/lib64:${SYS_ROOT}/usr/lib/${ARCH}-linux-androideabi:${SYS_ROOT}/usr/lib/${ARCH}-linux-androideabi/${ANDROID_NATIVE_API_LEVEL}"
INC_PATH="${SYS_ROOT}/usr/include"

export PATH="${HOST_ROOT}/bin:${PATH}"
export CMAKE_PREFIX_PATH="${SYS_ROOT}"
export CMAKE_LIBRARY_PATH=${LIB_PATH}
export CMAKE_INCLUDE_PATH=${INC_PATH}

export CFLAGS="-DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN} \
  -DANDROID_STL=c++_shared \
  -DANDROID_TOOLCHAIN=clang \
  -DANDROID_PLATFORM=android-${ANDROID_NATIVE_API_LEVEL} \
  -DANDROID_ABI=${ANDROID_ABI}"

rm -rf ${BUILD_DIR}

DIR="$( dirname -- "$0"; )";

meson configure -Dbuiltin_mbedtls=true

meson setup --errorlogs  \
  --prefix=${NDK} \
  --includedir=${INC_PATH} \
  --libdir=${LIB_PATH} \
  --build.cmake-prefix-path=${SYS_ROOT} \
  --cross-file ${DIR}/android-${CPU}.ini  \
  --default-library=static \
  ${BUILD_DIR} .

ninja -C ${BUILD_DIR}
