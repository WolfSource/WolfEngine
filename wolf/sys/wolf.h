/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/WolfEngine/issues
	Website			 : https://WolfEngine.github.io
	Name			 : wolf.h
	Description		 : the base headers for WOLF
	Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _WIN32

#define WOLF_API __declspec(dllexport)

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <Windows.h>

#else

#define WOLF_API

#endif // _WIN32

#include <stdio.h>
#include <stdint.h>

#ifndef ALIGNMENT_16
#define ALIGNMENT_16 16
#endif

#ifdef _MSVC
#pragma pack(push, ALIGNMENT_16)
#endif
	typedef struct buf
	{
		uint8_t *data;
		size_t len;
	} buf __attribute__((aligned(ALIGNMENT_16)));
	typedef struct buf *buf_t;

	/// <div rustbindgen opaque></div>
	int is_buf_valid(buf_t p_buf);

#ifdef __cplusplus
}
#endif