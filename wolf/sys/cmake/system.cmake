# fetch lz4
if (WOLF_SYSTEM_LZ4)
  message("fetching https://github.com/lz4/lz4.git")
  FetchContent_Declare(
    lz4_static
    GIT_REPOSITORY https://github.com/lz4/lz4.git
    GIT_TAG        dev
    SOURCE_SUBDIR  build/cmake
  )
  FetchContent_MakeAvailable(lz4_static)
  list(APPEND LIBS lz4_static)    
endif()

# fetch lzma
if (WOLF_SYSTEM_LZMA)
  message("fetching https://github.com/WolfEngine/lzma.git")
  FetchContent_Declare(
    lzma
    GIT_REPOSITORY https://github.com/WolfEngine/lzma.git
    GIT_TAG        main
  )
  FetchContent_MakeAvailable(lzma)
  list(APPEND LIBS lzma)    
endif()

# fetch vigem client
if (WOLF_SYSTEM_GAMEPAD_VIRTUAL)
  if (NOT WIN32)
    message( FATAL_ERROR "ViGem can only build for Windows")
  endif()

  message("fetching https://github.com/ViGEm/ViGEmClient.git")
  FetchContent_Declare(
  ViGEmClient
  GIT_REPOSITORY https://github.com/ViGEm/ViGEmClient.git
  GIT_TAG        master
  )
  FetchContent_MakeAvailable(ViGEmClient)
  list(APPEND LIBS 
    ViGEmClient
    Xinput.lib 
    SetupAPI.lib)
endif()
