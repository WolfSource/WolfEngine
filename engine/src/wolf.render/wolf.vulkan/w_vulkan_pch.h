/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_vulkan_pch.h
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

//contains vector, w_logger
#include <w_system_pch.h>

#include <memory>
#include <map>
#include <string.h>
#include <algorithm>
#include <thread>

//Wolf
#include <w_object.h>
#include <w_window.h>
#include <w_ireleasable.h>
#include <w_io.h>
#include <w_color.h>

#endif
