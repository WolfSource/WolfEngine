/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_basetsd.h
	Description		 : std helper functions and some typedefs
	Comment          :
*/

#ifndef __W_BASETSD_H__
#define __W_BASETSD_H__

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __WIN32

#include <limits.h>

#elif defined(__ANDROID)

//define dummy SAL for android
#ifndef _In_
#define _In_
#endif

#ifndef _In_z_
#define _In_z_
#endif

#ifndef _Out_
#define _Out_
#endif

#ifndef _Inout_
#define _Inout_
#endif

#ifdef __cplusplus
extern "C" {
#endif

	enum HRESULT : bool { S_FALSE = 0, S_OK = 1 };
	
	typedef unsigned char			BYTE;
	typedef unsigned int			UINT, *PUINT;
	typedef signed char				INT8, *PINT8;
	typedef signed short			INT16, *PINT16;
	typedef signed int				INT32, *PINT32;
	typedef __int64_t				INT64;
	typedef unsigned char			UINT8, *PUINT8;
	typedef unsigned short			UINT16, *PUINT16;
	typedef unsigned int			UINT32, *PUINT32;
	typedef __uint64_t				UINT64, *PUINT64;

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

#endif//ANDROID

#endif//__W_BASETSD_H__