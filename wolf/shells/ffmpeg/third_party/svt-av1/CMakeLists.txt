#
# SVT-AV1
#
#cmake_policy(set CMP0040 OLD)

configure_file(CMakeLists-svtav1.txt.in
               svtav1-download/CMakeLists.txt)
			   			   
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
				.
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third_party/svt-av1/svtav1-download )
execute_process(COMMAND ${CMAKE_COMMAND} --build .
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third_party/svt-av1/svtav1-download )

if(WIN32 OR WIN64)
    add_custom_command(OUTPUT svt1v1_command
        COMMAND build.bat 2022 ${CMAKE_BUILD_TYPE}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third_party/svt-av1/svtav1-src/Build/windows
        COMMENT "Building SVTAV1")
elseif(APPLE)
    add_custom_command(OUTPUT svt1v1_command
        COMMAND ./build.sh ${CMAKE_BUILD_TYPE}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third_party/svt-av1/svtav1-src/Build/linux
        COMMENT "Building SVTAV1")
endif()

add_custom_target(svt1v1 ALL
    DEPENDS svt1v1_command
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third_party/svt-av1/svtav1-src
    VERBATIM)
