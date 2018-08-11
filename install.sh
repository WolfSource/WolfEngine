#!/bin/bash

echo "make wolf's dependencies ready"
cd ./engine/dependencies/

keys=(
assimp
boost 
ffmpeg 
luaJIT 
nanomsg 
tbb)

values=(
version.txt
builds.txt 
version.txt 
version.txt  
COPYING 
version.txt)

for i in "${!keys[@]}"; do
   echo "uncompressing ${keys[$i]}.zip"
   unzip -q "${keys[$i]}.zip"
   if [ ! -f ./${keys[$i]}/${values[$i]} ]; then
     echo "could not find ${keys[$i]}/${values[$i]}"
   else
     echo "${keys[$i]} verified successfully"
   fi
done

echo "uncompressing vulkan.zip"
cat vulkan.zip.001 > vulkan.zip 
cat vulkan.zip.002 >> vulkan.zip
unzip -q "vulkan.zip" 
if [ ! -f ./vulkan/SPIRVExtension.vsix ]; then
  echo "could not find vulkan/SPIRVExtension.vsix"
else
  echo "vulkan verified successfully"
fi

tar -zxvf ./vulkan/macOS.tar.gz -C ./vulkan/

echo "start building Wolf"
case "$OSTYPE" in
  darwin*)  
            xcodebuild clean build -workspace ../../engine/builds/xcode/wolf.engine.vulkan.osx.xcworkspace -scheme test_vulkan_osx -sdk macosx10.13 -configuration Debug 
            ;; 
  linux*)   echo "LINUX" ;;
  *)        echo "OS: $OSTYPE" ;;
esac


