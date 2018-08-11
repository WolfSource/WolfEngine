#!/bin/bash

echo "make wolf's dependencies ready"
cd ./engine/dependencies/

keys=(
assimp
boost 
ffmpeg 
luaJIT 
nanomsg 
tbb 
vulkan)

values=(
version.txt
builds.txt 
version.txt 
version.txt  
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
case "$OSTYPE" in
  darwin*)  
            xcodebuild clean build -workspace ../../engine/builds/xcode/wolf.engine.vulkan.osx.xcworkspace -scheme test_vulkan_osx -sdk macosx10.13 -configuration Debug 
            ;; 
  linux*)   echo "LINUX" ;;
  *)        echo "OS: $OSTYPE" ;;
esac


