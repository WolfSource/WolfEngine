 # link to ffmpeg
if (WOLF_MEDIA_FFMPEG)
  file(GLOB_RECURSE WOLF_MEDIA_FFMPEG_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/media/ffmpeg/*"
  )
  list(APPEND SRCS ${WOLF_MEDIA_FFMPEG_SRC})

  list(APPEND FFMPEG_LIBS 
    avcodec 
    avdevice 
    avfilter
    avformat
    avutil
    swresample
    swscale
    )

  add_library(ffmpeg INTERFACE)
  target_include_directories(ffmpeg INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/ffmpeg/include")
  target_link_directories(ffmpeg INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/ffmpeg/bin")

  foreach (lib_name ${FFMPEG_LIBS})
    set(lib_file "${CMAKE_CURRENT_SOURCE_DIR}/third_party/ffmpeg/lib/${TARGET_OS}/${lib_name}.${LIB_EXT}")
    target_link_libraries(ffmpeg INTERFACE ${lib_file})
  endforeach()

  list(APPEND LIBS ffmpeg)
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

  set(FETCHCONTENT_QUIET OFF)
  FetchContent_MakeAvailable(openal)

  file(GLOB_RECURSE WOLF_MEDIA_OPENAL_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/media/w_openal.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/media/w_openal.cpp"
  )

  list(APPEND SRCS ${WOLF_MEDIA_OPENAL_SRC})
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
    "${CMAKE_CURRENT_SOURCE_DIR}/media/w_image.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/media/w_image.cpp"
  )

  list(APPEND SRCS ${WOLF_MEDIA_STB_SRC})
  list(APPEND INCLUDES ${stb_SOURCE_DIR})

endif()

file(GLOB_RECURSE WOLF_MEDIA_TEST_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/media/test/*"
)
list(APPEND SRCS ${WOLF_MEDIA_TEST_SRC})
