/*
    Project: Wolf Engine. Copyright � 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

#endif

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#ifndef W_UNUSED
#define W_UNUSED(p) (void)(p) 
#endif

#ifndef W_API
#ifdef _MSC_VER
#define W_API __declspec(dllexport)
#pragma warning(disable : 4471) // disable forward enum declaration
#else
#define W_API 
#endif
#endif

#define W_MAX_PATH 260 // maximum length of full pathname or name

#ifdef _MSC_VER
    #define ASM __asm
#else
#define ASM __asm__
    //The Microsoft source-code annotation language (SAL) dummies
    // find out more at https://docs.microsoft.com/en-us/cpp/code-quality/understanding-sal?view=msvc-170
#define _In_
#define _In_z_
#define _In_opt_
#define _In_opt_z_
#define _Out_
#define _Inout_
#define _Inout_z_
#define _Inout_opt_

#endif

#ifdef __cplusplus
}
#endif