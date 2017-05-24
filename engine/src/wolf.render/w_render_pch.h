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

#ifdef __DX12__
#pragma comment(lib, "d3d12.lib")
#elif defined(__DX11__)
#pragma comment(lib, "d3d11.lib")
#endif
#pragma comment(lib, "dxgi.lib")

#include <windows.h>
#include <wrl.h>

#ifdef __DX12__
#include <d3d12.h>
#elif defined(__DX11__)
#include <d3d11_3.h>
#endif

#if defined(__DX12__) || defined(__DX11__)
#include <dxgi1_4.h>
#include <DirectXMath.h>
#endif

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
