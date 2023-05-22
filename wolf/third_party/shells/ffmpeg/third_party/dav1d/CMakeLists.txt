#
# DAV1D
#
#cmake_policy(set CMP0040 OLD)

configure_file(CMakeLists-dav1d.txt.in
               dav1d-download/CMakeLists.txt)
			   
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/third_party/dav1d/dav1d-build/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE})
			   
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
				.
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third_party/dav1d/dav1d-download )
execute_process(COMMAND ${CMAKE_COMMAND} --build .
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third_party/dav1d/dav1d-download )
  
if(WIN32 OR WIN64)
	execute_process(COMMAND cmd.exe /c "msys2_shell.cmd -defterm -use-full-path -no-start -here -mingw64 -c \" \
		yes | pacman -S mingw-w64-x86_64-nasm \"")

    add_custom_command(OUTPUT dav1d_command
        COMMAND meson setup ${CMAKE_BINARY_DIR}/third_party/dav1d/dav1d-build/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE} --buildtype ${CMAKE_BUILD_TYPE_LOWER} && cd ${CMAKE_BINARY_DIR}/third_party/dav1d/dav1d-build/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE} && ninja
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/third_party/dav1d/dav1d-build/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE}/include/dav1d/version.h
		${CMAKE_BINARY_DIR}/third_party/dav1d/dav1d-src/include/dav1d/
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third_party/dav1d/dav1d-src/
        COMMENT "Building DAV1D")
elseif(APPLE)
    add_custom_command(OUTPUT dav1d_command
        COMMAND meson setup ${CMAKE_BINARY_DIR}/third_party/dav1d/dav1d-build/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE} --buildtype ${CMAKE_BUILD_TYPE_LOWER} && cd ${CMAKE_BINARY_DIR}/third_party/dav1d/dav1d-build/${PLATFORM}/${ARCH}/${CMAKE_BUILD_TYPE} && ninja
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third_party/dav1d/dav1d-src/
        COMMENT "Building DAV1D")
endif()

add_custom_target(dav1d ALL
    DEPENDS dav1d_command
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third_party/dav1d/dav1d-src/
    VERBATIM)
