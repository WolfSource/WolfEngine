#!/bin/bash

echo "make wolf's dependencies ready"
cd ./engine/dependencies/

keys=(
boost 
ffmpeg 
luaJIT 
msgpack 
nanomsg 
tbb 
vulkan)

values=(
builds.txt 
version.txt 
version.txt 
COPYING 
COPYING 
version.txt 
SPIRVExtension.vsix)

for i in "${!keys[@]}"; do
   echo "uncompressing ${keys[$i]}.zip"
   unzip -q "${keys[$i]}.zip"
   if [ ! -f ./${keys[$i]}/${values[$i]} ]; then
     echo "could not find ${keys[$i]}/${values[$i]}"
   else
     echo "${keys[$i]} verified successfully"
   fi
done

echo "start building Wolf"
xcodebuild clean build -workspace ../../engine/builds/xcode/wolf.engine.vulkan.metal.macOS.xcworkspace -scheme wolf.system.macOS -sdk macosx10.13 -configuration Debug
