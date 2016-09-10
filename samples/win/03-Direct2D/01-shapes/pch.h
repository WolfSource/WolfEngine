/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : pch.h
	Description		 : Pre-Compiled header
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#ifndef __PCH_H__
#define __PCH_H__

#ifdef WIN32

#pragma comment(lib, "Wolf.System.Win32.lib")
#pragma comment(lib, "Wolf.DirectX11_X.Win32.lib")

#include <w_target_ver.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

#endif

#include <memory>

#endif