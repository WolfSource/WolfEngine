/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/Wolf.Engine - Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_media_core_export.h
	Description		 : Dynamic library linkage header
	Comment          :
*/

#ifndef __W_MEDIA_CORE_EXPORT_H__
#define __W_MEDIA_CORE_EXPORT_H__

#ifdef _WIN32

#ifndef WMC_EXP

	#ifdef __WOLF_MEDIA_CORE__
		#define WMC_EXP __declspec(dllexport)
	#else
		#define WMC_EXP __declspec(dllimport)
	#endif

#endif // !1

#endif

#endif