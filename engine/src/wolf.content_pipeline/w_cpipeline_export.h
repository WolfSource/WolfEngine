/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_cpipeline_export.h
	Description		 : Dynamic library linkage header
	Comment          :
*/

#pragma once

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

