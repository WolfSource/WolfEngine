/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_media_core_export.h
	Description		 : Dynamic library linkage header
	Comment          :
*/


#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_MEDIA_CORE_EXPORT_H__
#define __W_MEDIA_CORE_EXPORT_H__

#if defined(__WIN32) || defined(__UWP)

#ifndef WMC_EXP

	#ifdef __WOLF_MEDIA_CORE__
		#define WMC_EXP __declspec(dllexport)
	#else
		#define WMC_EXP __declspec(dllimport)
	#endif

#endif


#elif defined(__ANDROID) || defined(__linux) || defined(__APPLE__)

#ifndef WMC_EXP
#define WMC_EXP //dump
#endif

#endif

#endif //__W_MEDIA_CORE_EXPORT_H__

