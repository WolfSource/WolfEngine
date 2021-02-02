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

#ifndef W_PLATFORM_WIN
    #define W_PLATFORM_WIN
#endif

#elif defined(__APPLE__)

#ifndef W_PLATFORM_OSX
    #define W_PLATFORM_OSX
#endif

#elif defined(__IOS__)

#ifndef W_PLATFORM_IOS
    #define W_PLATFORM_IOS
#endif

#elif defined(__ANDROID_API__)

#ifndef W_PLATFORM_ANDROID
    #define W_PLATFORM_ANDROID
#endif

#elif defined(__linux) || defined(__linux__)

#ifndef W_PLATFORM_LINUX
    #define W_PLATFORM_LINUX
#endif

#endif

#if defined(W_PLATFORM_OSX) || defined(W_PLATFORM_IOS) || defined(W_PLATFORM_LINUX) || defined(W_PLATFORM_ANDROID)

#ifndef W_PLATFORM_UNIX
    #define W_PLATFORM_UNIX
#endif

#endif

#ifdef W_PLATFORM_WIN

    #include <SDKDDKVer.h>
    
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif // !WIN32_LEAN_AND_MEAN

    #ifndef NOMINMAX
    #define NOMINMAX 
    #endif // !NOMINMAX

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

#ifdef __cplusplus
enum class w_std_types
{
#else
typedef enum
{
#endif
    W_TYPE_NULL = 0x00,
    W_TYPE_BOOLEAN = 0x01,
    W_TYPE_POSITIVE_INT = 0x02,
    W_TYPE_NEGATIVE_INT = 0x03,
    W_TYPE_FLOAT = 0x0a,
    W_TYPE_DOUBLE = 0x04,
    W_TYPE_STRING = 0x05,
    W_TYPE_ARRAY = 0x06,
    W_TYPE_ARRAY_BEGIN = 0x0b,
    W_TYPE_ARRAY_END = 0x0c,
    W_TYPE_MAP = 0x07,
} 
#ifdef __cplusplus
;
#else
w_std_types;
#endif

#ifdef _MSC_VER
#define ASM __asm
#else

#define ASM __asm__
//define dummy SAL
#define _In_
#define _In_z_
#define _Out_
#define _Inout_
#define _Inout_z_
#define _In_opt_
#define _In_opt_z_
#define _Inout_opt_

#endif
