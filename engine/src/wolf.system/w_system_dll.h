/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_system_dll.h
	Description		 : Dynamic library linkage header
	Comment          : 
*/


#ifndef __W_SYSTEM_DLL_H__
#define __W_SYSTEM_DLL_H__

#ifdef _WIN32

#ifndef SYS_EXP

	#ifdef __WOLF_SYSTEM__
		#define SYS_EXP __declspec(dllexport)
	#else
		#define SYS_EXP __declspec(dllimport)
	#endif

#endif

#endif

#endif
