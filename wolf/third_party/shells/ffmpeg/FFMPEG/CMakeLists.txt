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
    add_custom_command(OUTPUT ffmpeg_command
        COMMAND  export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:${CMAKE_BINARY_DIR}/third_party/svt-av1/svtav1-src/Build/linux/${CMAKE_BUILD_TYPE}:${CMAKE_BINARY_DIR}/third_party/dav1d/dav1d-build/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE}/meson-private:${CMAKE_BINARY_DIR}/third_party/vpx/vpx-build/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE} && 
		 ./configure
                 --enable-asm --enable-yasm
                 --enable-libdav1d --enable-libsvtav1
                 --extra-ldflags=-L${SVTAV1_LIBRARY}
                 --extra-ldflags=-L${DAV1D_LIBRARY}
                 --extra-ldflags=-L${VP9_LIBRARY}
                 --extra-cflags=-I${SVTAV1_INCLUDES}
                 --extra-cflags=-I${DAV1D_SOURCES}
                 --extra-cflags=-I${DAV1D_INCLUDES}
                 --extra-cflags=-I${VP9_INCLUDES}
                 --disable-doc --enable-ffplay ${VP9_OPTION}
                 --disable-ffprobe --enable-ffmpeg --enable-shared --enable-static --disable-bzlib
                     --disable-libopenjpeg --disable-iconv
                 --prefix=${CMAKE_BINARY_DIR}/FFMPEG/ffmpeg-build/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE}
                 --target-os=${TARGET_OS} --arch=${ARCH} ${CONFIGURE_FLAGS} && make clean && make && make install
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/FFMPEG/ffmpeg-src
        COMMENT "Building FFMPEG")
    
    add_custom_target(ffmpeg ALL
        DEPENDS ffmpeg_command
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/FFMPEG/ffmpeg-src
        VERBATIM)
elseif(WIN32 OR WIN64)
    string(REGEX REPLACE ":" "" MSYS_CMAKE_BINARY_DIR "${CMAKE_BINARY_DIR}")
    add_custom_command(OUTPUT ffmpeg_command
        		COMMAND cmd.exe /C "msys2_shell.cmd -use-full-path -no-start -here -mingw64 -c \" \
			export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/${MSYS_CMAKE_BINARY_DIR}/third_party/dav1d/dav1d-build/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE}/meson-private:/${MSYS_CMAKE_BINARY_DIR}/third_party/svt-av1/svtav1-src/Build/windows/ && \
			./configure \
			--disable-x86asm --enable-libdav1d --enable-libsvtav1\
			--extra-ldflags=\"-LIBPATH:${SVTAV1_LIBRARY}\" \
			--extra-ldflags=\"-LIBPATH:${DAV1D_LIBRARY}\" \
            --extra-ldflags=\"-LIBPATH:${VP9_LIBRARY}\" \
            --extra-ldflags=\"-LIBPATH:${VP9_LIBRARY}\" \
			--extra-cflags=\"-I${SVTAV1_INCLUDES}\" \
			--extra-cflags=\"-I${DAV1D_SOURCES}\" \
			--extra-cflags=\"-I${DAV1D_INCLUDES}\" \
            --extra-cflags=\"-I${VP9_INCLUDES}\" \
			--extra-cflags=\"-MTd\" \
			--extra-ldflags=\"/NODEFAULTLIB:library\" \
			--disable-doc --enable-ffplay ${VP9_OPTION} \
			--disable-ffprobe --enable-ffmpeg --enable-shared --enable-static --disable-bzlib \
			--disable-libopenjpeg --disable-iconv --disable-zlib\
			--prefix=${CMAKE_BINARY_DIR}/FFMPEG/ffmpeg-build/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE} \
			--target-os=${TARGET_OS} --arch=${ARCH} --toolchain=msvc ${CONFIGURE_FLAGS} && make clean && make && make install\""
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/FFMPEG/ffmpeg-src
        COMMENT "Building FFMPEG")
    
	if(VP9_ENABLE)
		add_custom_target(ffmpeg ALL
			DEPENDS ffmpeg_command
			DEPENDS dav1d
			DEPENDS svt1v1
			DEPENDS vpx
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/FFMPEG/ffmpeg-src
			VERBATIM)
	else()
		add_custom_target(ffmpeg ALL
			DEPENDS ffmpeg_command
			DEPENDS dav1d
			DEPENDS svt1v1
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/FFMPEG/ffmpeg-src
			VERBATIM)
	endif()

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
