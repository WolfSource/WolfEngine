/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_std.h
	Description		 : std helper functions and some typedefs
	Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32

    #include <SDKDDKVer.h>
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <Windows.h>
	#include <WinSock2.h>
	#include <ws2ipdef.h>

#define WOLF_MAIN()                                                                                    \
    int APIENTRY WinMain(HINSTANCE pHInstance, HINSTANCE pPrevHInstance, PSTR pSTR, int pCmdshow)

#else
    #include <unistd.h>

    #define WOLF_MAIN()                                                                                    \
    int main(int pArgc, const char * pArgv[])

#endif

#include <apr-1/apr.h>
#include <apr-1/apr_general.h>
#include <stdbool.h>
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <limits.h>
#include <os/w_platform.h>

//#include <errno.h>

#define W_ASSERT(c, fmt)            { if (!c) { printf(fmt); assert(c);} }
#define W_ASSERT_P(c, fmt, ...)     { if (!c) { printf((fmt), __VA_ARGS__); assert(c);} }
#define W_SAFE_DELETE(x)            { if (x)  { delete x; x = NULL;                  } }
#define W_SAFE_DELETE_ARRAY(ar)     { if (ar) { delete[] ar; ar = NULL;              } }
#define W_SAFE_RELEASE(x)           { if (x)  { x->release(); delete x; x = NULL;    } }

#ifdef __cplusplus
#if defined(__WIN32) && !defined(__WOLF_SYSTEM_STATIC_LIB)
    //DLL export
    #ifndef W_SYSTEM_EXPORT
    #define W_SYSTEM_EXPORT __declspec(dllexport)
    #endif
#else
    //fake
    #ifndef W_SYSTEM_EXPORT
    #define W_SYSTEM_EXPORT
    #endif
#endif
#endif

#ifdef _MSC_VER
    #define ASM __asm
#else
    #define ASM __asm__

    //define dummy SAL
    #define _In_
    #define _In_z_
    #define _Out_
    #define _Inout_
    #define _Inout_z_

#endif

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 4096
#endif

#define WOLF_MAJOR_VERSION 2    // Making incompatible API changes
#define WOLF_MINOR_VERSION 0    // Adding functionality in a backwards - compatible manner
#define WOLF_PATCH_VERSION 0    // bug fixes
#define WOLF_DEBUG_VERSION 0    // for debugging

typedef enum
{
    W_PASSED = 0,
    W_FAILED = 1,
    W_INVALIDARG,
    W_OUTOFMEMORY,
    W_INVALID,
	W_INCOMPLETE
} W_RESULT;

typedef void (*w_job)(void*);

/**
 * initialize wolf
 * @return W_RESULT as a result
*/
W_RESULT wolf_initialize(void);

/**
 * get default memory pool
 * @return apr memory pool
*/
apr_pool_t* w_get_default_memory_pool(void);

/**
 * allocate memory
 * @return memory in void pointer
*/
void* w_alloc(_In_ const size_t pSize);

/**
 * release all resources of wolf
*/
void wolf_terminate(void);

#ifdef __cplusplus
}
#endif
