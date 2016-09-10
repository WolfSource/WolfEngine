/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_directx_pch.h
	Description		 : Pre-Compiled header of Wolf.DirectX
	Comment          :
*/

#ifndef __W_DIRECTX_PCH_H__
#define __W_DIRECTX_PCH_H__

#ifndef NOMINMAX
#define NOMINMAX
#endif

//system
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "windowscodecs.lib")

//for gui edit box
#pragma comment(lib, "usp10.lib")

#if defined(WIN32)
#pragma comment(lib, "Shlwapi.lib")//For PathFindExtension function
#endif

//DirectX
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dxguid.lib")//neccessery for CLSID_D2D1BitmapSource, CLSID_D2D1DirectionalBlur.

//contains vector, w_logger
#include <w_system_pch.h>

#if defined(__DX12__)

#pragma comment(lib, "d3d12.lib")

#include <d3d12.h>
#include <d2d1_3.h>
#include <d2d1_3helper.h>
#include <d2d1effects_2.h>
#include <dwrite_3.h>
#include <dxgi1_4.h>

#elif defined(__DX11_X__)

#pragma comment(lib, "d3d11.lib")

#include <d3d11_2.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d2d1effects.h>
#include <dwrite_1.h>
#include <dxgi1_3.h>
#include <dxgi1_2.h>

#endif
#include <DirectXMath.h>
#include <DirectXColors.h>

//System
#if defined(__WINRT)
#include <agile.h>
#endif

#include "w_directX_dll.h"
#include <wrl\client.h>
#include <memory>
#include <map>
#include <string.h>
#include <algorithm>
#include <thread>
#include <ppl.h>
#include <ppltasks.h>
#include <Wincodec.h> // for IWICImagingFactory2

//C++Amp
#include <amp.h>
#include <amp_graphics.h>

//Wolf
#include <w_object.h>
#include <w_window.h>
#include <w_IReleasable.h>
#include <w_io.h>
#include "w_directX_dll.h"
#include "w_directX_math_helper.h"
#include "w_game.h"
#include <w_color.h>

#endif
