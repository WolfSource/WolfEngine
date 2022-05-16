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

#ifdef _MSC_VER
#define ASM __asm
#endif

#define WOLF_API __declspec(dllexport)

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <Windows.h>

#else

#include <time.h>

// defines
#define WOLF_API
#define ASM __asm__

#define _ALIGN_16_ 16
#define _ALIGN_32_ 32

#define _ALIGNMENT_16_ __attribute__((aligned(_ALIGN_16_)))
#define _ALIGNMENT_32_ __attribute__((aligned(_ALIGN_32_)))

#endif // _WIN32

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef _MSVC
#pragma pack(push, _ALIGN_16_)
#endif
	struct w_ubuf_t
	{
		uint8_t *data;
		size_t len;
	} _ALIGNMENT_16_;
	typedef struct w_ubuf_t *w_ubuf;

	struct w_buf_t
	{
		char *data;
		size_t len;
	} _ALIGNMENT_16_;
	typedef struct w_buf_t *w_buf;

#ifdef __cplusplus
}
#endif
