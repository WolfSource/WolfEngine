
if(NOT "D:/SourceCodes/WolfEngine/WolfEngine-cpp2b/wolf/shells/ffmpeg/build/third_party/dav1d/dav1d-download/dav1d-prefix/src/dav1d-stamp/dav1d-gitinfo.txt" IS_NEWER_THAN "D:/SourceCodes/WolfEngine/WolfEngine-cpp2b/wolf/shells/ffmpeg/build/third_party/dav1d/dav1d-download/dav1d-prefix/src/dav1d-stamp/dav1d-gitclone-lastrun.txt")
  message(STATUS "Avoiding repeated git clone, stamp file is up to date: 'D:/SourceCodes/WolfEngine/WolfEngine-cpp2b/wolf/shells/ffmpeg/build/third_party/dav1d/dav1d-download/dav1d-prefix/src/dav1d-stamp/dav1d-gitclone-lastrun.txt'")
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E rm -rf "D:/SourceCodes/WolfEngine/WolfEngine-cpp2b/wolf/shells/ffmpeg/build/third_party/dav1d/dav1d-src"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: 'D:/SourceCodes/WolfEngine/WolfEngine-cpp2b/wolf/shells/ffmpeg/build/third_party/dav1d/dav1d-src'")
endif()

# try the clone 3 times in case there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "C:/Program Files/Git/cmd/git.exe"  clone --no-checkout --config "advice.detachedHead=false" "https://code.videolan.org/videolan/dav1d.git" "dav1d-src"
    WORKING_DIRECTORY "D:/SourceCodes/WolfEngine/WolfEngine-cpp2b/wolf/shells/ffmpeg/build/third_party/dav1d"
    RESULT_VARIABLE error_code
    )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once:
          ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'https://code.videolan.org/videolan/dav1d.git'")
endif()

execute_process(
  COMMAND "C:/Program Files/Git/cmd/git.exe"  checkout master --
  WORKING_DIRECTORY "D:/SourceCodes/WolfEngine/WolfEngine-cpp2b/wolf/shells/ffmpeg/build/third_party/dav1d/dav1d-src"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: 'master'")
endif()

set(init_submodules TRUE)
if(init_submodules)
  execute_process(
    COMMAND "C:/Program Files/Git/cmd/git.exe"  submodule update --recursive --init 
    WORKING_DIRECTORY "D:/SourceCodes/WolfEngine/WolfEngine-cpp2b/wolf/shells/ffmpeg/build/third_party/dav1d/dav1d-src"
    RESULT_VARIABLE error_code
    )
endif()
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: 'D:/SourceCodes/WolfEngine/WolfEngine-cpp2b/wolf/shells/ffmpeg/build/third_party/dav1d/dav1d-src'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy
    "D:/SourceCodes/WolfEngine/WolfEngine-cpp2b/wolf/shells/ffmpeg/build/third_party/dav1d/dav1d-download/dav1d-prefix/src/dav1d-stamp/dav1d-gitinfo.txt"
    "D:/SourceCodes/WolfEngine/WolfEngine-cpp2b/wolf/shells/ffmpeg/build/third_party/dav1d/dav1d-download/dav1d-prefix/src/dav1d-stamp/dav1d-gitclone-lastrun.txt"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: 'D:/SourceCodes/WolfEngine/WolfEngine-cpp2b/wolf/shells/ffmpeg/build/third_party/dav1d/dav1d-download/dav1d-prefix/src/dav1d-stamp/dav1d-gitclone-lastrun.txt'")
endif()

