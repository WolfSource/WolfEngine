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

#ifdef __WIN32

#include <Windows.h>
#include <limits.h>

#elif defined(__ANDROID) || defined(__linux) || defined(__APPLE__)

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

#ifndef _Out_
#define _Out_
#endif

#ifndef _Out_opt_
#define _Out_opt_
#endif

#ifndef _Inout_
#define _Inout_
#endif

#ifdef __cplusplus
extern "C" {
#endif

	enum HRESULT : uint8_t { S_OK = 0, S_FALSE = 1, E_INVALIDARG = 2, E_OUTOFMEMORY = 3};
	
	typedef unsigned char			BYTE;
	typedef __uint32_t				UINT, *PUINT;
	typedef int8_t                  INT8, *PINT8;
	typedef int16_t                 INT16, *PINT16;
	typedef int32_t                 INT32, *PINT32;
	typedef __int64_t				INT64;
    typedef INT64                   LONG_PTR;

    typedef unsigned char			UINT8, *PUINT8;
	typedef unsigned short			UINT16, *PUINT16;
	typedef __uint32_t              UINT32, *PUINT32;
	typedef __uint64_t				UINT64, *PUINT64;
    typedef UINT64                  UINT_PTR;
    
    typedef UINT_PTR                WPARAM;
    typedef LONG_PTR                LPARAM;
    typedef LONG_PTR                LRESULT;

	typedef unsigned long			ULONG, *PULONG;
	typedef unsigned short			USHORT, *PUSHORT;
	typedef unsigned char			UCHAR, *PUCHAR;

	typedef signed int				LONG32, *PLONG32;
	typedef unsigned int			ULONG32, *PULONG32;
	typedef unsigned int			DWORD32, *PDWORD32;


	typedef unsigned long			DWORD;
	typedef unsigned short			WORD;

 

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

#endif//__W_BASETSD_H__
