cmake_minimum_required(VERSION 2.8.2)

project(dav1d-download NONE)

include(ExternalProject)
ExternalProject_Add(dav1d
  GIT_REPOSITORY    https://code.videolan.org/videolan/dav1d.git
  GIT_TAG           master
  SOURCE_DIR        "D:/SourceCodes/WolfEngine/WolfEngine-cpp2b/wolf/shells/ffmpeg/build/third_party/dav1d/dav1d-src"
  BINARY_DIR        "D:/SourceCodes/WolfEngine/WolfEngine-cpp2b/wolf/shells/ffmpeg/build/third_party/dav1d/dav1d-build"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)
