/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_system_export.h
	Description		 : Dynamic library linkage header
	Comment          : 
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_SYSTEM_EXPORT_H__
#define __W_SYSTEM_EXPORT_H__

#if (defined(__WIN32) || defined(__UWP)) && !defined(__WOLF_SYSTEM_STATIC_LIB)

#ifndef WSYS_EXP

	#ifdef __WOLF_SYSTEM__
		#define WSYS_EXP __declspec(dllexport)
	#else
		#define WSYS_EXP __declspec(dllimport)
	#endif

#endif

#else

#ifndef WSYS_EXP
#define WSYS_EXP //dump
#endif

#endif

#endif //__W_SYSTEM_EXPORT_H__
