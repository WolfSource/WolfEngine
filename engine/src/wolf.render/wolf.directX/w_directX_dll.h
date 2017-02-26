/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : W_DX_DLL_Linkage.h
	Description		 : Dynamic library linkage header
	Comment          :
*/


#ifndef __W_DIRECTX_DLL_H__
#define __W_DIRECTX_DLL_H__

#ifdef _WIN32

#ifndef DX_EXP

	#ifdef __WOLF_DIRECTX__
		#define DX_EXP __declspec(dllexport)
	#else
		#define DX_EXP __declspec(dllimport)
	#endif

#endif

#endif

#endif