echo off
setlocal

echo make wolf's dependencies ready
cd ./engine/deps/

set keys=assimp ffmpeg luaJIT nanomsg tbb simplygon libjpeg-turbo libpng zlib
set values=version.txt version.txt version.txt COPYING version.txt SimplygonSDKRuntimeReleasex64.dll version.txt version.txt version.txt

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

echo uncompressing boost.zip
   type boost.zip.001 boost.zip.002 boost.zip.003 boost.zip.004 > boost.zip
   PowerShell Expand-Archive -Path "boost.zip" "./"
   if exist boost/builds.txt (
       echo boost verified successfully
   ) else (
       echo could not find boost/builds.txt
   )

echo uncompressing vulkan.zip
   type vulkan.zip.001 vulkan.zip.002 > vulkan.zip
   PowerShell Expand-Archive -Path "vulkan.zip" "./"
   if exist vulkan/SPIRVExtension.vsix (
       echo vulkan verified successfully
   ) else (
       echo could not find vulkan/SPIRVExtension.vsix
   )

