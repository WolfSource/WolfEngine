/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_platform.h
    Description      : wolf supported platforms and defines
    Comment          :
*/

#pragma once

#if defined(_WIN32) || defined(_WIN64)

    #define W_PLATFORM_WIN

#elif defined(__APPLE__)

    #define W_PLATFORM_OSX

#elif defined(__IOS__)

    #define W_PLATFORM_IOS

#elif defined(__linux) || defined(__linux__)

    #define W_PLATFORM_LINUX

#elif defined(_ANDROID)

    #define W_PLATFORM_ANDROID

#endif

#if defined(W_PLATFORM_OSX) || defined(W_PLATFORM_IOS) || defined(W_PLATFORM_LINUX) || defined(W_PLATFORM_ANDROID)

    #define W_PLATFORM_UNIX

#endif

#ifdef W_PLATFORM_WIN

    #include <SDKDDKVer.h>

    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif

    #ifndef NOMINMAX 
        #define NOMINMAX 
    #endif

    #include <Windows.h>
    #include <WinSock2.h>
    #include <ws2ipdef.h>

    //DLL export
    #ifndef W_SYSTEM_EXPORT
        #define W_SYSTEM_EXPORT __declspec(dllexport)
    #endif

    #define WOLF_MAIN()                                                                                    \
        int APIENTRY WinMain(HINSTANCE pHInstance, HINSTANCE pPrevHInstance, PSTR pSTR, int pCmdshow)
#else

    //dummy
    #ifndef W_SYSTEM_EXPORT
        #define W_SYSTEM_EXPORT
    #endif

    #include <unistd.h>
    #define WOLF_MAIN()                                                                                    \
        int main(int pArgc, const char * pArgv[])

#endif

typedef int W_RESULT;
