 # link to ffmpeg
if (WOLF_MEDIA_FFMPEG)
  file(GLOB_RECURSE WOLF_MEDIA_FFMPEG_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/media/ffmpeg/*"
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
  set(LIBTYPE "STATIC" CACHE STRING "STATIC")

  set(FETCHCONTENT_QUIET OFF)
  FetchContent_MakeAvailable(openal)
  
  file(GLOB_RECURSE WOLF_MEDIA_OPENAL_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/media/w_openal.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/media/w_openal.cpp"
  )
   
  list(APPEND SRCS ${WOLF_MEDIA_OPENAL_SRC})
  list(APPEND INCLUDES ${openal_SOURCE_DIR}/include)
  list(APPEND LIBS OpenAL::OpenAL)    

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

  file(GLOB_RECURSE WOLF_MEDIA_STB_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/media/w_image.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/media/w_image.hpp"
  )

  list(APPEND SRCS ${WOLF_MEDIA_STB_SRC})
  list(APPEND INCLUDES ${stb_SOURCE_DIR})

endif()

if (WOLF_MEDIA_GSTREAMER)
  file(GLOB_RECURSE WOLF_MEDIA_GSTREAMER_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/media/gst/*"
    "${CMAKE_CURRENT_SOURCE_DIR}/media/gst/audio/*"
    "${CMAKE_CURRENT_SOURCE_DIR}/media/gst/core/*"
    "${CMAKE_CURRENT_SOURCE_DIR}/media/gst/elements/*"
    "${CMAKE_CURRENT_SOURCE_DIR}/media/gst/video/*"
  )

  find_package(PkgConfig REQUIRED)

  pkg_check_modules(gstreamer REQUIRED IMPORTED_TARGET
        gstreamer-1.0
        gstreamer-video-1.0
        gstreamer-audio-1.0)

  add_library(gstreamer-lib INTERFACE)

  target_compile_options(gstreamer-lib INTERFACE ${gstreamer_CFLAGS})
  target_include_directories(gstreamer-lib INTERFACE ${gstreamer_INCLUDE_DIRS})
  target_link_directories(gstreamer-lib BEFORE INTERFACE ${gstreamer_LIBRARY_DIRS})
  target_link_libraries(gstreamer-lib INTERFACE ${gstreamer_LIBRARIES})

  list(APPEND SRCS ${WOLF_MEDIA_GSTREAMER_SRC})
  list(APPEND LIBS gstreamer-lib)
endif()

file(GLOB_RECURSE WOLF_MEDIA_TEST_SRC
"${CMAKE_CURRENT_SOURCE_DIR}/media/test/*"
)
list(APPEND SRCS ${WOLF_MEDIA_TEST_SRC})
