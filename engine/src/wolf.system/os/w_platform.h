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

