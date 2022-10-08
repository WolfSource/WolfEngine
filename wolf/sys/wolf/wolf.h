/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

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

#ifdef __clang__
#define W_ALIGNMENT_8 __attribute__((packed)) __attribute__((aligned(8)))
#define W_ALIGNMENT_16 __attribute__((packed)) __attribute__((aligned(16)))
#define W_ALIGNMENT_32 __attribute__((packed)) __attribute__((aligned(32)))
#define W_ALIGNMENT_64 __attribute__((packed)) __attribute__((aligned(64)))
#define W_ALIGNMENT_128 __attribute__((packed)) __attribute__((aligned(128)))
#else
#define W_ALIGNMENT_16
#define W_ALIGNMENT_32
#define W_ALIGNMENT_64
#define W_ALIGNMENT_128
#endif

#ifdef __cplusplus
}
#endif