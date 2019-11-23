/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_system_pch.hpp
	Description		 : Pre-Compiled header of Wolf.System
	Comment          :
*/

#pragma once
                        
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
#include <sys/resource.h>

#endif

#include "w_std.h"
#include "w_logger.h"

