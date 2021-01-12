# CMakeLists 

# CMAKE required version
cmake_minimum_required (VERSION 3.0)

# set the project name
project (WOLF_ENGINE)

option(MyOption "MyOption" OFF)

# add the executable
add_executable(WOLF_ENGINE /engine/src/wolf.system/wolf.c)