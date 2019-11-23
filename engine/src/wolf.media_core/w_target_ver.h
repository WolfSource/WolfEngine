/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_target_ver.h
	File Description : target version header
	Comment          : Including SDKDDKVer.h defines the highest available Windows platform.
					   If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
					   set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.
*/

#pragma once

#ifdef __WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <SDKDDKVer.h>

#endif

