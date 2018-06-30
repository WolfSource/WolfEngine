/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_render_export.h
	Description		 : dynamic library linkage header
	Comment          : 
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_RENDER_EXPORT_H__
#define __W_RENDER_EXPORT_H__

#if defined(__WIN32) || defined(__UWP)

#ifndef W_EXP

	#ifdef __WOLF_RENDER__
		#define W_EXP __declspec(dllexport)
	#else
		#define W_EXP __declspec(dllimport)
	#endif

#endif


#elif defined(__ANDROID) || defined(__linux) || defined(__IOS__) || defined(__APPLE__)

#ifndef W_EXP
#define W_EXP //dump
#endif

#endif

#endif //__W_VULKAN_EXPORT_H__
