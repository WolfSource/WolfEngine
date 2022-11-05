# link to ffmpeg
if (WOLF_MEDIA_FFMPEG)
  file(GLOB_RECURSE FFMPEG_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/media/av_frame.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/media/av_frame.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/media/av_packet.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/media/av_packet.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/media/ffmpeg.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/media/ffmpeg.cpp"
  )
  list(APPEND SRCS ${FFMPEG_SRCS})
  list(APPEND INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ffmpeg/include)

  list(APPEND FFMPEG_LIBS 
    avcodec 
    avdevice 
    avfilter
    avformat
    avutil
    postproc
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
  FetchContent_MakeAvailable(openal)

  file(GLOB_RECURSE OPENAL_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/media/openal.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/media/openal.cpp"
  )

  list(APPEND SRCS ${OPENAL_SRCS})
  list(APPEND INCLUDES ${openal_SOURCE_DIR}/include)
  list(APPEND LIBS ${openal_BINARY_DIR}/${CMAKE_BUILD_TYPE}/OpenAL32.${LIB_EXT})    

endif()