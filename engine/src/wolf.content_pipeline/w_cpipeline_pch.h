/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_cpipeline_pch.h
	Description		 : Pre-Compiled header of Wolf.ContentPipeline
	Comment          :
*/

#pragma once

#include "w_cpipeline_export.h"

// disable VC++ 2K5 warnings about deprecated standard C functions
#ifdef _MSC_VER
#if _MSC_VER >= 1400
#define _CRT_SECURE_NO_DEPRECATE
#endif
#endif

#include <vector>
#include <map>
#include <list>
#include <queue>
#include <set>
#include <w_xml.h>
#include <fstream>
#include <sstream>

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <functional>
#include <string>
#include <assert.h>
#include <float.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm_extension.h>

#include <w_logger.h>

#ifdef __linux
#define __cdecl
#define _isnan(x) std::isnan(x)
#define _finite(x) finite(x)
#endif

#ifdef __WIN32

/*
    Starting with Windows 8, the DirectX SDK is included as part of the Windows SDK.
    more information at https://msdn.microsoft.com/en-us/library/windows/desktop/ee663275(v=vs.85).aspx
*/
//for AMD TOOTLE
#include <DirectXMath.h>

#endif

