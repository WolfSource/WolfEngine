call cd /D d:\SourceCodes\WolfEngine\virtual_gamepad\wolf_stream/deps/live555/
 call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC/vcvarsall.bat" x64
 call "C:\Program Files\Git\bin\sh.exe" -c  " cd d:/SourceCodes/WolfEngine/virtual_gamepad/wolf_stream/deps/live555/ &&  curl http://www.live555.com/liveMedia/public/live555-latest.tar.gz > ./live555-latest.tar.gz &&  tar -xzvf live555-latest.tar.gz --strip-components 1  " 
call attrib -r d:/SourceCodes/WolfEngine/virtual_gamepad/wolf_stream/deps/live555//win32config
echo NODEBUG=1 > win32config
echo TARGETOS = WINNT >> win32config
echo UI_OPTS =       $(guilflags) $(guilibsdll) >> win32config
echo CONSOLE_UI_OPTS =   $(conlflags) $(conlibsdll) >> win32config
echo CPU=amd64 >> win32config
echo COMPILE_OPTS =      $(INCLUDES) $(cdebug) $(cflags) $(cvarsdll) -I. /EHsc /O2 /MD /GS /D "win64" /Oy- /Oi /D "NDEBUG" /D "NO_GETIFADDRS" -I"C:/Program Files/OpenSSL-Win64/include" >> win32config
echo C =         c >> win32config
echo C_COMPILER =        cl >> win32config
echo C_FLAGS =       $(COMPILE_OPTS) >> win32config
echo CPP =           cpp >> win32config
echo CPLUSPLUS_COMPILER =    $(C_COMPILER) >> win32config
echo CPLUSPLUS_FLAGS =   $(COMPILE_OPTS) >> win32config
echo OBJ =           obj >> win32config
echo LINK =              link -out: >> win32config
echo LIBRARY_LINK =      lib -out: >> win32config
echo LINK_OPTS_0 =       $(linkdebug) ws2_32.lib /NXCOMPAT >> win32config
echo LIBRARY_LINK_OPTS = >> win32config
echo LINK_OPTS =     $(LINK_OPTS_0) $(UI_OPTS) >> win32config
echo CONSOLE_LINK_OPTS = $(LINK_OPTS_0) $(CONSOLE_UI_OPTS) >> win32config
echo SERVICE_LINK_OPTS =     kernel32.lib advapi32.lib shell32.lib ws2_32.lib -subsystem:console,$(APPVER) >> win32config
echo LIB_SUFFIX =        lib >> win32config
echo LIBS_FOR_CONSOLE_APPLICATION = >> win32config
echo LIBS_FOR_GUI_APPLICATION = >> win32config
echo MULTIMEDIA_LIBS =   winmm.lib >> win32config
echo EXE =           .exe >> win32config
echo PLATFORM = Windows >> win32config
echo rc32 = rc.exe >> win32config
echo .rc.res: >> win32config
echo    $(rc32) $^< >> win32config
call genWindowsMakefiles.cmd 
 cd liveMedia
 nmake -f liveMedia.mak
 cd ..\groupsock
 nmake -f groupsock.mak
 cd ..\UsageEnvironment
 nmake -f UsageEnvironment.mak
 cd ..\BasicUsageEnvironment
 nmake -f BasicUsageEnvironment.mak
 mkdir "d:/SourceCodes/WolfEngine/virtual_gamepad/wolf_stream/deps/live555/build/Debug/usr/local//lib"
 copy "d:\SourceCodes\WolfEngine\virtual_gamepad\wolf_stream\deps\live555\BasicUsageEnvironment\libBasicUsageEnvironment.lib" "d:\SourceCodes\WolfEngine\virtual_gamepad\wolf_stream/deps/live555/build/Debug/usr/local//lib"
 copy "d:\SourceCodes\WolfEngine\virtual_gamepad\wolf_stream\deps\live555\groupsock\libgroupsock.lib" "d:\SourceCodes\WolfEngine\virtual_gamepad\wolf_stream/deps/live555/build/Debug/usr/local//lib"
 copy "d:\SourceCodes\WolfEngine\virtual_gamepad\wolf_stream\deps\live555\liveMedia\libliveMedia.lib" "d:\SourceCodes\WolfEngine\virtual_gamepad\wolf_stream/deps/live555/build/Debug/usr/local//lib"
 copy "d:\SourceCodes\WolfEngine\virtual_gamepad\wolf_stream\deps\live555\UsageEnvironment\libUsageEnvironment.lib" "d:\SourceCodes\WolfEngine\virtual_gamepad\wolf_stream/deps/live555/build/Debug/usr/local//lib"
  