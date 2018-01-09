/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/Wolf.Engine - Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
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

