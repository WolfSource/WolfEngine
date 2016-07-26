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

#ifdef WIN32

#ifdef _DEBUG

	#pragma comment(lib, "tbbmalloc_debug.lib") 
	#pragma comment(lib, "libboost_system-vc140-mt-gd-1_61.lib")
	#pragma comment(lib, "libboost_filesystem-vc140-mt-gd-1_61.lib") 

#else

	#pragma comment(lib, "tbbmalloc.lib") 
	#pragma comment(lib, "libboost_system-vc140-mt-1_61.lib")
	#pragma comment(lib, "libboost_filesystem-vc140-mt-1_61.lib") 

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
#include <tchar.h>

#include <vector>
#include <tuple>
#include <tbb/scalable_allocator.h>

#include "w_system.h"

#include <boost/bind.hpp>

#endif