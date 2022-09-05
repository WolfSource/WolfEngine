if (WOLF_STREAM_RIST)
  file(GLOB_RECURSE RIST_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/stream/rist.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/stream/rist.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/stream/rist_data_block.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/stream/rist_data_block.cpp"
  )
  list(APPEND SRCS ${RIST_SRCS})

  message("fetching https://code.videolan.org/rist/librist.git")
  FetchContent_Declare(
    librist
    GIT_REPOSITORY https://code.videolan.org/rist/librist.git
    GIT_TAG        master
  )
  FetchContent_MakeAvailable(librist)

  STRING(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)

  add_custom_command(OUTPUT rist_command.out COMMAND cmd /c "meson setup ${librist_BINARY_DIR} --backend vs2022 --default-library static --buildtype ${CMAKE_BUILD_TYPE_LOWER} &
       meson compile -C ${librist_BINARY_DIR}"
      WORKING_DIRECTORY ${librist_SOURCE_DIR})

  add_custom_target(rist ALL DEPENDS rist_command.out)

  list(APPEND INCLUDES
    ${librist_BINARY_DIR}
    ${librist_BINARY_DIR}/include
    ${librist_BINARY_DIR}/include/librist
    ${librist_SOURCE_DIR}/src
    ${librist_SOURCE_DIR}/include
    ${librist_SOURCE_DIR}/include/librist
    ${librist_SOURCE_DIR}/contrib
    ${librist_SOURCE_DIR}/contrib/mbedtls/include
  )
  list(APPEND LIBS
    ws2_32
    ${librist_BINARY_DIR}/librist.a)  
endif()