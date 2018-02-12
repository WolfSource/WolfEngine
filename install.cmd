echo off
setlocal

echo make wolf's dependencies ready
cd ./engine/dependencies/

set keys=boost ffmpeg luaJIT msgpack nanomsg tbb vulkan
set values=builds.txt version.txt version.txt COPYING COPYING version.txt SPIRVExtension.vsix

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




