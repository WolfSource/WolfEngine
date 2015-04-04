/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @PooyaEimandar on twitter
	Name			 : Wolf.DirectX.Shared.PCH.h
	Description		 : Pre-Compiled header of Wolf.DirectX
	Comment          :
*/

#pragma once

#include "Wolf.System.Shared.PCH.h"

//for DirectX
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dxguid.lib")//neccessery for CLSID_D2D1BitmapSource, CLSID_D2D1DirectionalBlur.
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "dinput8.lib")

#if defined(__WIN32)
#pragma comment(lib, "Shlwapi.lib")//For PathFindExtension function
#endif

#ifdef MAYA

//For using MGlobal
#pragma comment(lib, "OpenMaya.lib")

#endif

#include <wrl/client.h>
#include <memory>
#include <d3d11_1.h>
#include <d2d1_1.h>
#include <dwrite_1.h>
#include <dxgi1_2.h>
#include <d2d1_1helper.h>
#include <d2d1effects.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <W_IReleasable.h>
#include <W_Logger.h>
#include <W_IO.h>
#include <string.h>
#include <algorithm>
#include <thread>
#include <ppl.h>
#include <ppltasks.h>

#ifdef MAYA

#include <maya/MObject.h>

#endif // MAYA
