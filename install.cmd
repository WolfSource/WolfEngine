echo off
setlocal

echo make wolf's dependencies ready
cd ./engine/deps/

set keys=assimp boost ffmpeg luaJIT nanomsg tbb simplygon
set values=version.txt builds.txt version.txt version.txt COPYING version.txt SimplygonSDKRuntimeReleasex64.dll

set i=0
(for %%k in (%keys%) do (
   echo uncompressing %%k.zip
   PowerShell Expand-Archive -Path "%%k.zip" "./"
   if exist %%k/!vector[!i!]! (
       echo could not find %%k/!vector[!i!]!
   ) else (
       echo %%k verified successfully
   )
))

echo uncompressing vulkan.zip
   type vulkan.zip.001 vulkan.zip.002 > vulkan.zip
   PowerShell Expand-Archive -Path "vulkan.zip" "./"
   if exist vulkan/SPIRVExtension.vsix (
       echo vulkan verified successfully
   ) else (
       echo could not find vulkan/SPIRVExtension.vsix
   )




