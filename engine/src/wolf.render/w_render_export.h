/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
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

#ifndef W_VK_EXP

	#ifdef __WOLF_RENDER_VULKAN__
		#define W_VK_EXP __declspec(dllexport)
	#else
		#define W_VK_EXP __declspec(dllimport)
	#endif

#endif


#elif defined(__ANDROID) || defined(__linux) || defined(__IOS__) || defined(__APPLE__)

#ifndef W_VK_EXP
#define W_VK_EXP //dump
#endif

#endif

#endif //__W_VULKAN_EXPORT_H__
