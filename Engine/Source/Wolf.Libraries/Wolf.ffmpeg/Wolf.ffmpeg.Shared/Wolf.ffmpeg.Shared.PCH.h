/*
	Project			 : Wolf PlayOut, powered by Wolf Engine Copyright (c) Pooya Eimandar. https://github.com/PooyaEimandar/WolfEngine
					   Please direct any bug to hello@WolfStudio.co
	File Name        : fpch.h
	File Description : Pre-Compiled header
	Comment          :
*/

#pragma once

#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "postproc.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")

#ifdef QT_DLL
#pragma comment(lib, "qtmaind.lib")
#pragma comment(lib, "Qt5Cored.lib")
#pragma comment(lib, "Qt5Guid.lib")
#endif

#include "targetver.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#pragma comment(lib, "Mf.lib")//for MFCreateSourceResolver
#pragma comment(lib, "Mfplat.lib")//for MFCreateSourceResolver
#pragma comment(lib, "Mfuuid.lib")//Read information of file in windows

#include <W_Logger.h>
#include <mutex>
#include <mfidl.h>//Read information of file in windows
#include <mfapi.h>
