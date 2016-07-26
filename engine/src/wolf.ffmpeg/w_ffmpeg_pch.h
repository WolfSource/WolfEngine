/*
	Project			 : Wolf Engine (http://WolfSource.io). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/Wolf.Engine - Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_ffmpeg_pch.h
	Description		 : Pre-Compiled header
	Comment          :
*/


#ifndef __W_FFMPEG_PCH_H__
#define __W_FFMPEG_PCH_H__

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

#include "w_target_ver.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#pragma comment(lib, "Mf.lib")//for MFCreateSourceResolver
#pragma comment(lib, "Mfplat.lib")//for MFCreateSourceResolver
#pragma comment(lib, "Mfuuid.lib")//Read information of file in windows

#include "w_ffmpeg_dll.h"

#include <w_logger.h>
#include <mutex>
#include <mfidl.h>//Read information of file in windows
#include <mfapi.h>

#include <tbb/parallel_for.h>


#endif