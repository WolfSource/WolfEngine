/*
	Project			 : Wolf Engine (http://WolfSource.io). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/Wolf.Engine - Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_target_ver.h
	File Description : target version header
	Comment          : Including SDKDDKVer.h defines the highest available Windows platform.
					   If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
					   set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.
*/

#ifndef __W_TARGET_VER_H__
#define __W_TARGET_VER_H__

#ifdef __WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <SDKDDKVer.h>

#endif

#endif
