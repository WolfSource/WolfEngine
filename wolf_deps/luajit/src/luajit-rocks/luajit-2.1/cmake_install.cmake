# Install script for directory: /Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "./build")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1/src/luaconf.h"
    "/Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1/src/lua.h"
    "/Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1/src/lauxlib.h"
    "/Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1/src/lualib.h"
    "/Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1/src/lua.hpp"
    "/Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1/src/luajit.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1/libluajit.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libluajit.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libluajit.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libluajit.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1/luajit")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/luajit" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/luajit")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/luajit")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/lua/5.1/jit" TYPE FILE FILES
    "/Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1/src/jit/bc.lua"
    "/Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1/src/jit/v.lua"
    "/Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1/src/jit/dump.lua"
    "/Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1/src/jit/dis_x86.lua"
    "/Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1/src/jit/dis_x64.lua"
    "/Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1/src/jit/dis_arm.lua"
    "/Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1/src/jit/dis_ppc.lua"
    "/Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1/src/jit/dis_mips.lua"
    "/Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1/src/jit/dis_mipsel.lua"
    "/Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1/src/jit/bcsave.lua"
    "/Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1/src/jit/bc.lua"
    "/Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1/src/jit/p.lua"
    "/Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1/src/jit/zone.lua"
    "/Users/pooyaeimandar/Desktop/github/WolfEngine/WolfEngine/wolf_deps/luajit/src/luajit-rocks/luajit-2.1/vmdef.lua"
    )
endif()

