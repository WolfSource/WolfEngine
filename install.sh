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
vulkan
vulkan_mac)

values=(
builds.txt 
version.txt 
version.txt 
COPYING 
COPYING 
version.txt 
SPIRVExtension.vsix
COPYING)

for i in "${!keys[@]}"; do
   echo "uncompressing ${keys[$i]}.zip"
   unzip -q "${keys[$i]}.zip"
   if [ ! -f ./${keys[$i]}/${values[$i]} ]; then
     echo "could not find ${keys[$i]}/${values[$i]}"
   else
     echo "${keys[$i]} verified successfully"
   fi
done

cp -R "./vulkan_mac/Mac" "./vulkan"

echo "start building Wolf"
case "$OSTYPE" in
  darwin*)  xcodebuild clean build -workspace ../../engine/builds/xcode/wolf.engine.vulkan.macOS.xcworkspace -scheme test_vulkan_macOS -sdk macosx10.13 -configuration Debug ;; 
  linux*)   echo "LINUX" ;;
  *)        echo "OS: $OSTYPE" ;;
esac


