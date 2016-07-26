/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/Wolf.Engine - Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_cpipeline_dll.h
	Description		 : Dynamic library linkage header
	Comment          :
*/

#ifndef __W_CPIPELINE_DLL_H__
#define __W_CPIPELINE_DLL_H__

#ifdef _WIN32

#ifndef  CP_EXP

	#ifdef __WOLF_CONTENT_PIPELINE__
		#define CP_EXP __declspec(dllexport)
	#else
		#define CP_EXP __declspec(dllimport)
	#endif

#endif

#endif

#endif