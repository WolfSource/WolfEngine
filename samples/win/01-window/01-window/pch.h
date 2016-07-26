/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : pch.h
	Description		 : Pre-Compiled header
	Comment          :
*/

#ifndef _PCH_H_
#define _PCH_H_

#ifdef __WIN32

	#include <w_target_ver.h>

	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif

	#include <Windows.h>

#endif

#include <memory>

#endif
