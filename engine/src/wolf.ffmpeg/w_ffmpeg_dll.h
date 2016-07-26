/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/Wolf.Engine - Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_ffmpeg_dll.h
	Description		 : Dynamic library linkage header
	Comment          :
*/

#ifndef __W_FFMPEG_DLL_H__
#define __W_FFMPEG_DLL_H__

#ifdef _WIN32

#ifndef FF_EXP

	#ifdef __WOLF_FFMPEG__
		#define FF_EXP __declspec(dllexport)
	#else
		#define FF_EXP __declspec(dllimport)
	#endif

#endif // !1

#endif

#endif