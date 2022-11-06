#
# VPX
#
#cmake_policy(set CMP0040 OLD)

configure_file(CMakeLists-vpx.txt.in
               vpx-download/CMakeLists.txt)
			   			   
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
				.
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third_party/vpx/vpx-download )
execute_process(COMMAND ${CMAKE_COMMAND} --build .
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third_party/vpx/vpx-download )
  
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/third_party/vpx/vpx-build/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE})

if(WIN64)
	set(CONFIGURE_FLAGS "--target=x86_64-win64-vs17 --enable-static-msvcrt")
elseif(WIN32)
	set(CONFIGURE_FLAGS "--target=x86-win32-vs17 --enable-static-msvcrt")
endif()

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
	set(VPX_LIB_NAME "vpxmtd.lib")
else()
	set(VPX_LIB_NAME "vpxmt.lib")
endif()

if(WIN64 OR WIN32)
    add_custom_command(OUTPUT vpx_command
		COMMAND cmd.exe /c "msys2_shell.cmd -use-full-path -no-start -here -mingw64 -c \" \
		yes | pacman -S mingw-w64-x86_64-nasm\
		&& ${CMAKE_BINARY_DIR}/third_party/vpx/vpx-src/configure ${CONFIGURE_FLAGS} \
        && make \""
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third_party/vpx/vpx-build/${PLATFORM}/
        COMMENT "Building LIBVPX")
    add_custom_command(OUTPUT vpx_rename_command
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/third_party/vpx/vpx-build/${PLATFORM}/${ARCH_S}/${CMAKE_BUILD_TYPE}/${VPX_LIB_NAME} 
		${CMAKE_BINARY_DIR}/third_party/vpx/vpx-build/${PLATFORM}/${ARCH_S}/${CMAKE_BUILD_TYPE}/vpx.lib
		DEPENDS vpx_command
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third_party/vpx/vpx-build/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE}
        COMMENT "Building LIBVPX")
		
	add_custom_target(vpx ALL
		DEPENDS vpx_command
		DEPENDS vpx_rename_command
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third_party/vpx/vpx-build/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE}
		VERBATIM)
elseif(APPLE)
    add_custom_command(OUTPUT vpx_command
        COMMAND ${CMAKE_BINARY_DIR}/third_party/vpx/vpx-src/configure
        COMMAND make
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third_party/vpx/vpx-build/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE}
        COMMENT "Building LIBVPX")
		
	add_custom_target(vpx ALL
		DEPENDS vpx_command
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third_party/vpx/vpx-build/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE}
		VERBATIM)
endif()
