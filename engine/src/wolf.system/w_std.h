/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_std.h
	Description		 : std helper functions and some typedefs
	Comment          :
*/

#pragma once

#include <string>

#if defined(__WIN32) || defined(__UWP)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <limits.h>
#include <stdint.h>
#include <inttypes.h>

#else

//define dummy SAL for android and linux
#ifndef _In_
#define _In_
#endif

#ifndef _In_opt_
#define _In_opt_
#endif

#ifndef _In_z_
#define _In_z_
#endif

#ifndef _In_opt_z_
#define _In_opt_z_
#endif

#ifndef _Out_
#define _Out_
#endif

#ifndef _Out_opt_
#define _Out_opt_
#endif

#ifndef _Inout_
#define _Inout_
#endif

#ifndef _Inout_z_
#define _Inout_z_
#endif

#ifndef _Inout_opt_z_
#define _Inout_opt_z_
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __ANDROID
	#include <unistd.h>
    using std::size_t;
#endif
		
    typedef unsigned char			UCHAR, *PUCHAR;
    typedef unsigned long			ULONG , *PULONG;
    typedef unsigned short			USHORT, *PUSHORT;
    typedef unsigned int			ULONG32, *PULONG32;
    typedef unsigned int			DWORD32, *PDWORD32;
    typedef signed int				LONG32, *PLONG32;
    typedef unsigned long			DWORD;
	typedef unsigned short			WORD;
    typedef unsigned char			BYTE;
                
	// minimum signed 64 bit value
#define _I64_MIN		(-9223372036854775807 - 1)
	// maximum signed 64 bit value
#define _I64_MAX		9223372036854775807
	// maximum unsigned 64 bit value
#define _UI64_MAX		0xffffffffffffffff

#ifdef __cplusplus
}//extern "C"
#endif

#include <sstream>
#include <sys/stat.h>
#include <memory>

namespace std
{
	template <typename T>
	std::string to_string(T _Val)
	{
		std::ostringstream os;
		os << _Val;
		std::string _str = os.str();
		os.clear();
		return _str;
	}

	template <typename T>
	std::wstring to_wstring(T _Val)
	{
		auto _str = to_string(_Val);
		auto _wstr = std::wstring(_str.begin(), _str.end());
		_str.clear();
		return _wstr;
	}
}

#endif//ANDROID && __linux && __APPLE__

#include <memory>
#include <vector>

enum W_RESULT : uint8_t { W_PASSED = 0, W_FAILED, W_INVALIDARG, W_OUTOFMEMORY, W_INVALID_FILE_ATTRIBUTES };

namespace std
{
	template<typename T>
	inline int binary_search_find_index(_In_ std::vector<T>& pV, _In_ const T& pData)
	{
		auto _iter = std::lower_bound(pV.begin(), pV.end(), pData);
		if (_iter == pV.end() || *_iter != pData)
		{
			return -1;
		}
		else
		{
			return std::distance(pV.begin(), _iter);
		}
	}
}

#define COM_RELEASE(x)			{ if (x) { x->Release(); x = nullptr;	} }
#define COMPTR_RELEASE(x)		{ if (x) { auto _x = x.GetAddressOf(); (*_x)->Release(); (*_x) = nullptr; x = nullptr; } }

#define SAFE_DELETE(x)			{ if (x) { delete x; x = nullptr;		} }
#define SAFE_DELETE_ARRAY(ar)   { if (ar){ delete[] ar; ar = nullptr;	} }

#define SAFE_DELETE_SHARED(x)   { if (x) { while(x.use_count() != 0) x.reset(); x = nullptr;} }

#define UNIQUE_RELEASE(x)		{ if (x) { x->release(); x.reset(nullptr); } }
#define SHARED_RELEASE(x)		{ if (x) { x->release(); while(x.use_count() != 0) x.reset(); x = nullptr;} }
#define SAFE_RELEASE(x)			{ if (x) { x->release(); delete x; x = nullptr;	} }

using defer = std::shared_ptr<void>;

