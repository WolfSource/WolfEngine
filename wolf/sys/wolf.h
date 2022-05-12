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

// define dummies
#define WOLF_API

#define ASM __asm__

#endif // _WIN32

#include <stdio.h>
#include <stdint.h>

#ifndef _ALIGNMENT_
#define _ALIGNMENT_ 16
#endif

#ifdef _MSVC
#pragma pack(push, _ALIGNMENT_)
#endif
	typedef struct w_buf_t
	{
		uint8_t *data;
		size_t len;
	} w_buf_t __attribute__((aligned(_ALIGNMENT_)));
	typedef struct w_buf_t *w_buf;

	int is_buf_valid(w_buf p_buf);

#ifdef __cplusplus
}
#endif