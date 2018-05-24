/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_system_pch.hpp
	Description		 : Pre-Compiled header of Wolf.System
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_SYSTEM_PCH_H__
#define __W_SYSTEM_PCH_H__
                        
#define WOLF_MAJOR_VERSION 1// Making incompatible API changes.
#define WOLF_MINOR_VERSION 64// Adding functionality in a backwards - compatible manner.
#define WOLF_PATCH_VERSION 1// bug fixes
#define WOLF_DEBUG_VERSION 1// for debugging.

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef W_UNUSED
#define W_UNUSED(p) {(void)(p); } 
#endif

#ifndef W_ARRAY_SIZE
#define W_ARRAY_SIZE(ARR)	(size_t)((sizeof(ARR) / sizeof(ARR[0])))
#endif

#if defined(__WIN32) || defined(__UWP)

#include "w_target_ver.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 
#endif

#endif //defined(__WIN32) || defined(__UWP)

// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>

#ifndef w_sprintf
    #ifdef _MSC_VER
        #define w_sprintf(s_, l_, f_, ...)                                      \
        sprintf_s((s_), (l_), (f_), __VA_ARGS__)
    #else
        #define w_sprintf(s_, l_, f_, ...)                                    \
        sprintf((s_), (f_), __VA_ARGS__)
    #endif
#endif

#ifdef __APPLE__
    #include <malloc/malloc.h>
#else
    #include <malloc.h>
#endif

#include <memory.h>
#include <cstdlib>
#include <tuple>

#if defined(__WIN32) || defined(__UWP)

#include <tchar.h>

#elif defined(__ANDROID)

//Java Native Interface library
#include <jni.h>
#include <android/asset_manager.h>

#endif

#include "w_std.h"
#include "w_logger.h"

#endif //__W_SYSTEM_PCH_H__
