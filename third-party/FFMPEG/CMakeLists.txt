#
# FFMPEG
#
#cmake_policy(set CMP0040 OLD)

if(IOS)
    set(DEPLOYMENT_TARGET 11.0)
    execute_process(COMMAND sh -c "xcode-select -print-path" OUTPUT_VARIABLE XCODE_ROOT OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(DEVROOT "${XCODE_ROOT}/Platforms/iPhoneOS.platform/Developer")
    set(SDKROOT "${DEVROOT}/SDKs/iPhoneOS.sdk")
    set(CFLAGS "-arch ${ARCH}")
    set(CFLAGS "${CFLAGS}  -isysroot ${SDKROOT} -mios-version-min=${DEPLOYMENT_TARGET}")
elseif(ANDROID)
    set(BASEDIR $(pwd))
#    set(NDK /home/siminbadri/Documents/android-ndk-r23)
    set(TOOLCHAIN ${NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin)
    set(SYSROOT ${NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot)
    set(CFLAGS "-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2 -fno-strict-overflow -fstack-protector-all -lssp -fpie
        -I${TOOLCHAIN}/../include/c++/4.9.x/")
endif()

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    set(CONFIGURE_FLAGS "--enable-debug")
    if(NOT IOS AND NOT APPLE)
        set(CONFIGURE_FLAGS "${CONFIGURE_FLAGS} --extra-cflags=-MDd --extra-ldflags=/NODEFAULTLIB:libcmt")
    endif()
endif()

configure_file(CMakeLists-FFMPEG.txt.in
               FFMPEG-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE
				.
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/FFMPEG/FFMPEG-download )
execute_process(COMMAND ${CMAKE_COMMAND} --build .
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/FFMPEG/FFMPEG-download )

if(IOS)
    execute_process(COMMAND sh -c "\
                 ./configure \
                 --enable-asm --enable-yasm \
                 --disable-doc --disable-ffplay \
                 --disable-ffprobe --disable-ffmpeg --enable-shared --disable-static --disable-bzlib \
                 --disable-libopenjpeg --disable-iconv --disable-zlib --disable-audiotoolbox --enable-cross-compile \
                 --prefix=${CMAKE_BINARY_DIR}/FFMPEG/ffmpeg-build/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE} \
                 --cc=${XCODE_ROOT}/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang \
                 --cxx=${XCODE_ROOT}/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++ --extra-cflags=\"${CFLAGS}\" \
                 --extra-ldflags=\"${CFLAGS}\"\
                 --target-os=${TARGET_OS} --arch=${ARCH} ${CONFIGURE_FLAGS} && make clean && make && make install"
                 WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/FFMPEG/ffmpeg-src)
elseif(APPLE)
    execute_process(COMMAND sh -c "\
                 cd ${CMAKE_BINARY_DIR}/FFMPEG/ffmpeg-src && \
                 ./configure \
                 --enable-asm --enable-yasm \
                 --disable-doc --disable-ffplay \
                 --disable-ffprobe --disable-ffmpeg --enable-shared --disable-static --disable-bzlib \
                 --disable-libopenjpeg --disable-iconv --disable-zlib \
                 --prefix=${CMAKE_BINARY_DIR}/FFMPEG/ffmpeg-build/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE} \
                 --target-os=${TARGET_OS} --arch=${ARCH} ${CONFIGURE_FLAGS} && make clean && make && make install")
elseif(WIN32 OR WIN64)
    execute_process(COMMAND msys2_shell.cmd -use-full-path -no-start -here -defterm -mingw64 -c "\
				 cd ${CMAKE_BINARY_DIR}/FFMPEG/ffmpeg-src && \
				 ./configure \
				 --disable-x86asm \
				 --disable-doc --enable-ffplay \
				 --disable-ffprobe --enable-ffmpeg --enable-shared --disable-static --disable-bzlib \
				 --disable-libopenjpeg --disable-iconv --disable-zlib \
				 --prefix=${CMAKE_BINARY_DIR}/FFMPEG/ffmpeg-build/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE} \
				 --target-os=${TARGET_OS} --arch=${ARCH} --toolchain=msvc ${CONFIGURE_FLAGS} && make clean && make && make install")
elseif(ANDROID)
    execute_process(COMMAND sh "-c" "./configure \
				 --disable-asm \
				 --disable-doc --disable-ffplay \
				 --disable-ffprobe --disable-ffmpeg --enable-shared --disable-static --disable-bzlib \
				 --disable-libopenjpeg --disable-iconv --disable-zlib \
				 --prefix=${CMAKE_BINARY_DIR}/FFMPEG/ffmpeg-build/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE} \
				 --target-os=${TARGET_OS} --arch=${ARCH} ${CONFIGURE_FLAGS} \
				 --cpu=${ARCH} --cc=${TOOLCHAIN}/armv7a-linux-androideabi19-clang \
				 --cxx=${TOOLCHAIN}/armv7a-linux-androideabi19-clang++ \
				 --nm=${TOOLCHAIN}/llvm-nm --ar=${TOOLCHAIN}/llvm-ar \
				 --ranlib=${TOOLCHAIN}/llvm-ranlib --strip=${TOOLCHAIN}/llvm-strip \
				 --ld=${TOOLCHAIN}/ld --enable-pic --enable-cross-compile \
				 --extra-cflags="${CFLAGS}" \
				 --extra-ldflags=\"-Wl,soname=$(SLIBNAME) -z notext $LDFLAGS -L${SYSROOT}/usr/lib/arm-linux-androideabi/19\""
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/FFMPEG/ffmpeg-src)

# Modify FFMPEG generated Makefile
execute_process(COMMAND sed -i -e "s/HAVE_CBRT 0/HAVE_CBRT 1/g; \
				   s/HAVE_CBRTF 0/HAVE_CBRTF 1/g; \
				   s/HAVE_COPYSIGN 0/HAVE_COPYSIGN 1/g; \
				   s/HAVE_ERF 0/HAVE_ERF 1/g; \
				   s/HAVE_HYPOT 0/HAVE_HYPOT 1/g; \
				   s/HAVE_INET_ATON 0/HAVE_INET_ATON 1/g; \
				   /#define getenv(x) NULL/d; \
				   s/HAVE_ISINF 0/HAVE_ISINF 1/g; \
				   s/HAVE_ISNAN 0/HAVE_ISNAN 1/g; \
				   s/HAVE_RINT 0/HAVE_RINT 1/g; \
				   s/HAVE_LRINT 0/HAVE_LRINT 1/g; \
				   s/HAVE_LRINTF 0/HAVE_LRINTF 1/g; \
				   s/HAVE_ROUND 0/HAVE_ROUND 1/g; \
				   s/HAVE_ROUNDF 0/HAVE_ROUNDF 1/g; \
				   s/HAVE_TRUNC 0/HAVE_TRUNC 1/g; \
				   s/HAVE_TRUNCF 0/HAVE_TRUNCF 1/g" ${CMAKE_BINARY_DIR}/FFMPEG/ffmpeg-src/config.h)

# Modify config.mak
execute_process(COMMAND sed -i -e "s/SHFLAGS=-shared -Wl,-soname,$(SLIBNAME)/SHFLAGS=-shared -soname $(SLIBNAME)/g"
			${CMAKE_BINARY_DIR}/FFMPEG/ffmpeg-src/ffbuild/config.mak)

execute_process(COMMAND bash "-c" "make clean && make && make install" WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/FFMPEG/ffmpeg-src)
endif()
