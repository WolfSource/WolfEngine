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
                        
#define WOLF_MAJOR_VERSION 1// when you make incompatible API changes.
#define WOLF_MINOR_VERSION 30// when you add functionality in a backwards - compatible manner.
#define WOLF_PATCH_VERSION 9// bug fixes
#define WOLF_DEBUG_VERSION 13// for debugging.

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef W_UNUSED
#define W_UNUSED(p) {(void)(p); } 
#endif

#ifndef W_ARRAY_SIZE
#define W_ARRAY_SIZE(ARR)	(sizeof(ARR) / sizeof(ARR[0]))
#endif

#if defined(__WIN32) || defined(__UWP)

#ifndef w_sprintf
#define w_sprintf(s_, l_, f_, ...)                                      \
    sprintf_s((s_), (l_), (f_), __VA_ARGS__)
#endif

#ifdef _DEBUG
	#pragma comment(lib, "tbb_debug.lib") 
	#pragma comment(lib, "tbbmalloc_debug.lib") 
#else
	#pragma comment(lib, "tbb.lib")
	#pragma comment(lib, "tbbmalloc.lib") 
#endif

#include "w_target_ver.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 
#endif

#else

#ifndef w_sprintf
#define w_sprintf(s_, l_, f_, ...)                                    \
    snprintf((s_), (l_), (f_), __VA_ARGS__)
#endif

#endif //defined(__WIN32) || defined(__UWP)

// C RunTime Header Files
#include <stdlib.h>

#ifdef __APPLE__
    #include <malloc/malloc.h>
#else
    #include <malloc.h>
#endif

#include <memory.h>
#include <cstdlib>

#include <vector>
#include <tuple>

#if defined(__WIN32) || defined(__UWP)

#include <tchar.h>
#include <tbb/scalable_allocator.h>

#elif defined(__ANDROID)

//Java Native Interface library
#include <jni.h>
#include <android/asset_manager.h>
#include "w_std.h"

#endif

#include "w_logger.h"

#endif //__W_SYSTEM_PCH_H__
