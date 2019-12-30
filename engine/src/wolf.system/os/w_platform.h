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

    #define W_PLATROM_WIN

#elif defined(__APPLE__)

    #define W_PLATROM_OSX

#elif defined(__IOS__)

    #define W_PLATROM_IOS

#elif defined(__linux) || defined(__linux__)

    #define W_PLATROM_LINUX

#elif defined(_ANDROID)

    #define W_PLATROM_ANDROID

#endif

#if defined(W_PLATROM_OSX) || defined(W_PLATROM_IOS) || defined(W_PLATROM_LINUX) || defined(W_PLATROM_ANDROID)

    #define W_PLATFORM_UNIX

#endif

