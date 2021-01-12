/*
    Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website			 : https://WolfEngine.App
    Name			 : w_media_export.h
    Description		 : the media export header
    Comment          :
*/

#pragma once

#include <os/w_platform.h>

#ifdef W_PLATFORM_WIN

    //DLL export
    #ifndef W_MEDIA_EXPORT
        #define W_MEDIA_EXPORT __declspec(dllexport)
    #endif

#else

    //dummy
    #ifndef W_MEDIA_EXPORT
        #define W_MEDIA_EXPORT
    #endif

#endif
