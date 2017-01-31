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

#if _MSC_VER > 1000
#pragma once
#endif

#if defined(__WIN32) || defined(__UNIVERSAL)

#ifndef WSYS_EXP

	#ifdef __WOLF_SYSTEM__
		#define WSYS_EXP __declspec(dllexport)
	#else
		#define WSYS_EXP __declspec(dllimport)
	#endif

#endif


#elif defined(__ANDROID) || defined(__linux)

#ifndef WSYS_EXP
#define WSYS_EXP //dump
#endif

#endif

#endif //__W_SYSTEM_DLL_H__
