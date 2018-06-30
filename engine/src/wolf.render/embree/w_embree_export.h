/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfSource/Wolf.Engine/issues
	Website			 : https://WolfEngine.app
	Name			 : w_embree_export.h
	Description		 : dynamic library linkage header
	Comment          : 
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_EMBREE_EXPORT_H__
#define __W_EMBREE_EXPORT_H__

#ifdef __WIN32

#ifndef W_REN_EM

	#ifdef __WOLF_EMBREE__
		#define W_REN_EM __declspec(dllexport)
	#else
		#define W_REN_EM __declspec(dllimport)
	#endif

#endif

#elif defined(__linux) || defined(__APPLE__)

#ifndef W_REN_EM
#define W_REN_EM
#endif

#endif

#endif //__W_VULKAN_EXPORT_H__
