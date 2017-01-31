/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_system_pch.h
	Description		 : Pre-Compiled header of Wolf.System
	Comment          :
*/


#ifndef __W_SYSTEM_PCH_H__
#define __W_SYSTEM_PCH_H__
        
#if _MSC_VER > 1000
#pragma once
#endif

                
#define WOLF_MAJOR_VERSION 0// when you make incompatible API changes.
#define WOLF_MINOR_VERSION 7// when you add functionality in a backwards - compatible manner.
#define WOLF_PATCH_VERSION 4// bug fixes
#define WOLF_DEBUG_VERSION 0// for debugging.

#ifndef W_UNUSED
#define W_UNUSED(p) {(void)(p); } 
#endif

#ifndef W_ARRAY_SIZE
#define W_ARRAY_SIZE(ARR)	(sizeof(ARR) / sizeof(ARR[0]))
#endif

#if defined(__WIN32) || defined(__UNIVERSAL)

#ifdef _DEBUG
	#pragma comment(lib, "tbbmalloc_debug.lib") 
#else
	#pragma comment(lib, "tbbmalloc.lib") 
#endif

#include "w_target_ver.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 
#endif

#endif

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <cstdlib>

#include <vector>
#include <tuple>

#if defined(__WIN32) || defined(__UNIVERSAL)

#include <tchar.h>
#include <tbb/scalable_allocator.h>
#include "w_logger.h"

#elif defined(__ANDROID)

//Java Native Interface library
#include <jni.h>
#include <android/asset_manager.h>
#include "w_std.h"
        
#elif defined(__linux)
        
#include "w_logger.h"

#endif

#endif //__W_SYSTEM_PCH_H__