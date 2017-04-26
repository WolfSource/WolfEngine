/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/Wolf.Engine - Please direct any bug to https://github.com/WolfSource/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_cpipeline_export.h
	Description		 : Dynamic library linkage header
	Comment          :
*/

#ifndef __W_CPIPELINE_EXPORT_H__
#define __W_CPIPELINE_EXPORT_H__

#if _MSC_VER > 1000
#pragma once
#endif

#if defined(__WIN32) || defined(__UWP)

#ifndef WCP_EXP

#ifdef __WOLF_CONTENT_PIPELINE__
#define WCP_EXP __declspec(dllexport)
#else
#define WCP_EXP __declspec(dllimport)
#endif

#endif


#elif defined(__ANDROID) || defined(__linux) || defined(__IOS__) || defined(__APPLE__)

#ifndef WCP_EXP
#define WCP_EXP //dump
#endif

#endif

#endif //__W_CPIPELINE_EXPORT_H__
