#!/bin/bash

echo "make wolf's dependencies ready"
cd ./engine/deps/

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

echo "$OSTYPE"
echo "start building Wolf"

if [[ "$OSTYPE" == "darwin" ]]; then
  tar -zxvf ./vulkan/macOS.tar.gz -C ./vulkan/ 
  xcodebuild clean build -workspace ../../engine/builds/xcode/wolf.engine.vulkan.osx.xcworkspace -scheme test_vulkan_osx -sdk macosx10.13 -configuration Debug
  echo "All done successfully"
elif [[ "$OSTYPE" == "linux-gnu" ]]; then
  echo "building wolf.system.linux" 
  cd   "$PWD/../src/wolf.system/"
  cmake . 
  make
  echo "building wolf.media_core.linux" 
  cd   "$PWD/../wolf.media_core/"
  cmake . 
  make
  echo "building wolf.content_pipeline.linux" 
  cd   "$PWD/../wolf.content_pipeline/"
  cmake . 
  make
  echo "All done successfully"
else
  echo "platform not supported yet"
fi


