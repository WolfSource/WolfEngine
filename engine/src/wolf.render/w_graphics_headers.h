/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_graphics_header.h
	Description		 : The include header for graphics devices. Wolf.Engine supports two render APIs, the first one is DirectX 12 
						which supports both Windows 10 and Universal Windows P(UWP) and the second one is Vulkan which supports
						Windows, Linux, Android and OSX/IOS(with MoltenVK)  
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_GRAPHICS_HEADERS_H__
#define __W_GRAPHICS_HEADERS_H__

#ifdef __DX12__

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>

using Microsoft::WRL::ComPtr;

#elif defined (__VULKAN__) 
	#ifdef __WIN32
		#ifndef VK_USE_PLATFORM_WIN32_KHR
			#define VK_USE_PLATFORM_WIN32_KHR
		#endif
	#elif defined(__linux) && !defined(__ANDROID)
		#ifndef VK_USE_PLATFORM_XCB_KHR
			#define VK_USE_PLATFORM_XCB_KHR
		#endif
	#endif

    #if defined(__iOS__) || defined(__APPLE__)

#ifdef __iOS__
        #ifndef VK_USE_PLATFORM_IOS_MVK
            #define VK_USE_PLATFORM_IOS_MVK
        #endif
#else
        #ifndef VK_USE_PLATFORM_MACOS_MVK
            #define VK_USE_PLATFORM_MACOS_MVK
        #endif
#endif

        #include <vulkan/vulkan.h>
        #include <MoltenVK/vk_mvk_moltenvk.h>
        #include <unistd.h>

	#elif defined(__ANDROID)
		#include "vk_android/vulkan_wrapper.h"
	#else
		#include <vulkan/vulkan.hpp>
	#endif
#endif

#ifdef __ANDROID
using std::size_t;
#endif

#if defined(__linux) ||  defined(__APPLE__) || defined(__ANDROID)
#include <w_std.h>
#endif

#ifdef __VULKAN__
#define DEFAULT_FENCE_TIMEOUT 1000000000
#endif

enum w_memory_property_flag_bits
{
#ifdef __VULKAN__
    W_MEMORY_PROPERTY_DEVICE_LOCAL_BIT = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    W_MEMORY_PROPERTY_HOST_VISIBLE_BIT = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
    W_MEMORY_PROPERTY_HOST_COHERENT_BIT = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    W_MEMORY_PROPERTY_HOST_CACHED_BIT = VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
    W_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT,
    W_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM = VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM
#endif
};

#endif
