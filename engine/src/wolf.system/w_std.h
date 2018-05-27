/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_std.h
	Description		 : std helper functions and some typedefs
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_STD_H__
#define __W_STD_H__

#if defined(__WIN32) || defined(__UWP)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <limits.h>
#include <stdint.h>

#else

#include <inttypes.h>

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
                

                //	typedef __uint32_t				UINT, *PUINT;
//	typedef int8_t                  INT8, *PINT8;
//	typedef int16_t                 INT16, *PINT16;
//	typedef int32_t                 INT32, *PINT32;
//	typedef __int64_t				INT64;
//    typedef INT64                   LONG_PTR;
//
//    typedef unsigned char			UINT8, *PUINT8;
//	typedef unsigned short			UINT16, *PUINT16;
//	typedef __uint32_t              UINT32, *PUINT32;
//	typedef __uint64_t				UINT64, *PUINT64;
//    typedef UINT64                  UINT_PTR;
//    
//                  typedef UINT_PTR                WPARAM;
//                  typedef LONG_PTR                LPARAM;
//                  typedef LONG_PTR                LRESULT;





 

	// minimum signed 64 bit value
#define _I64_MIN		(-9223372036854775807 - 1)
	// maximum signed 64 bit value
#define _I64_MAX		9223372036854775807
	// maximum unsigned 64 bit value
#define _UI64_MAX		0xffffffffffffffff

#ifdef __cplusplus
}//extern "C"
#endif

#include <string>
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

enum W_RESULT : uint8_t { W_PASSED = 0, W_FAILED, W_INVALIDARG, W_OUTOFMEMORY, W_INVALID_FILE_ATTRIBUTES };

#include <vector>
typedef std::vector<uint8_t> w_vector_uint8_t;
typedef std::vector<uint16_t> w_vector_uint16_t;
typedef std::vector<uint32_t> w_vector_uint32_t;
typedef std::vector<float>	w_vector_float;


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

#include "python_exporter/py_std.h"

#endif//__W_STD_H__
