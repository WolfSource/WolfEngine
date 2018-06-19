/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_render_pch.h
	Description		 : Pre-Compiled header of Wolf.DirectX
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_VULKAN_PCH_H__
#define __W_VULKAN_PCH_H__

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <w_system_pch.h>

#if defined(__WIN32) || defined(__UWP)

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef __DX12__
#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>
#endif

#include <windows.h>
#include <wrl.h>

#endif

#include <memory>
#include <map>
#include <string.h>
#include <algorithm>
#include <thread>
#include <mutex>

//Wolf
#include <w_object.h>
#include <w_std.h>
#include <w_window.h>
#include <w_ireleasable.h>
#include <w_io.h>
#include <w_color.h>
#include <w_logger.h>

#endif
