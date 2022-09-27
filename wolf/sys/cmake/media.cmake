# link to ffmpeg
if (WOLF_MEDIA_FFMPEG)
  file(GLOB_RECURSE FFMPEG_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/media/*"
  )
  list(APPEND SRCS ${FFMPEG_SRCS})
  list(APPEND INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ffmpeg/include)

  list(APPEND FFMPEG_LIBS 
    avcodec 
    avdevice 
    avfilter
    avformat
    avutil
    dav1d
    SvtAv1Dec
    SvtAv1Enc
    swresample
    swscale
    vpx)

  foreach (lib_name ${FFMPEG_LIBS})
    list(APPEND LIBS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ffmpeg/lib/${TARGET_OS}/${CMAKE_BUILD_TYPE}/${lib_name}.${LIB_EXT})
  endforeach()
  
endif()