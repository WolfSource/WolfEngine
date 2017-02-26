/*
    Project          : Wolf Engine (http://WolfSource.io). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfSource.io or tweet @Wolf_Engine on twitter
    Name             : pch.h
    Description      : The pre-compiled header
    Comment          :
*/

#ifndef __PCH_H__
#define __PCH_H__

#if defined(__WIN32) || defined(__UNIVERSAL)

#include "w_target_ver.h"
        
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
        
#endif

#include <w_system_export.h>
#include <w_vulkan_export.h>
        
#include <memory>
#include <map>

#endif