 # link to ffmpeg
if (WOLF_MEDIA_FFMPEG)
  file(GLOB_RECURSE WOLF_MEDIA_FFMPEG_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/media/ffmpeg/av_frame.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/media/ffmpeg/av_frame.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/media/ffmpeg/av_packet.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/media/ffmpeg/av_packet.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/media/ffmpeg/ffmpeg.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/media/ffmpeg/ffmpeg.hpp"
  )
  list(APPEND SRCS ${WOLF_MEDIA_FFMPEG_SRC})
  list(APPEND INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ffmpeg/include)

  list(APPEND FFMPEG_LIBS 
    avcodec 
    avdevice 
    avfilter
    avformat
    avutil
    swresample
    swscale
    )

  foreach (lib_name ${FFMPEG_LIBS})
    list(APPEND LIBS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ffmpeg/lib/${TARGET_OS}/${lib_name}.${LIB_EXT})
  endforeach()
endif()

# link openAL
if (WOLF_MEDIA_OPENAL)
  message("fetching https://github.com/kcat/openal-soft.git")
 
  FetchContent_Declare(
    openal
    GIT_REPOSITORY https://github.com/kcat/openal-soft.git
    GIT_TAG        master
  )
  
  set(ALSOFT_EXAMPLES OFF CACHE BOOL "ALSOFT_EXAMPLES")
  set(ALSOFT_INSTALL_EXAMPLES OFF CACHE BOOL "ALSOFT_INSTALL_EXAMPLES")

  FetchContent_MakeAvailable(openal)

  file(GLOB_RECURSE WOLF_MEDIA_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/media/w_openal.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/media/w_openal.cpp"
  )

  list(APPEND SRCS ${WOLF_MEDIA_SRC})
  list(APPEND INCLUDES ${openal_SOURCE_DIR}/include)
  list(APPEND LIBS ${openal_BINARY_DIR}/${CMAKE_BUILD_TYPE}/OpenAL32.${LIB_EXT})    

  set_target_properties(
    build_version
    common
    ex-common
    OpenAL
    openal-info
    PROPERTIES FOLDER "openAL") 

endif()

if (WOLF_MEDIA_STB)
  message("fetching https://github.com/nothings/stb.git")
 
  FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
  )
  
  FetchContent_Populate(stb)

  list(APPEND INCLUDES ${stb_SOURCE_DIR})

endif()

file(GLOB_RECURSE WOLF_MEDIA_TEST_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/media/test/av_frame_convert.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/media/test/openal.hpp"

)
list(APPEND SRCS ${WOLF_MEDIA_TEST_SRC})