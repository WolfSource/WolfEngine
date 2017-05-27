/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_gui_export.h
	Description		 : Dynamic library linkage header
	Comment          : 
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_GUI_EXPORT_H__
#define __W_GUI_EXPORT_H__

#if defined(__WIN32) || defined(__UWP)

#ifndef WGUI_EXP

	#ifdef __WOLF_GUI__
		#define WGUI_EXP __declspec(dllexport)
	#else
		#define WGUI_EXP __declspec(dllimport)
	#endif

#endif


#elif defined(__ANDROID) || defined(__linux) || defined(__APPLE__)

#ifndef WGUI_EXP
#define WGUI_EXP //dump
#endif

#endif

#endif //__W_GUI_EXPORT_H__
