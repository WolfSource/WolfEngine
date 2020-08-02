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
    
    #ifndef NOMINMAX 
        #define NOMINMAX 
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

#include <stdbool.h>
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <limits.h>
#include "os/w_platform.h"

//#include <errno.h>

#define W_ASSERT(c, fmt)            { if (!c) { printf(fmt); assert(c);} }
#define W_ASSERT_P(c, fmt, ...)     { if (!c) { printf((fmt), __VA_ARGS__); assert(c);} }
#define W_SAFE_DELETE(x)            { if (x)  { delete x; x = NULL;                  } }
#define W_SAFE_DELETE_ARRAY(ar)     { if (ar) { delete[] ar; ar = NULL;              } }
#define W_SAFE_RELEASE(x)           { if (x)  { x->release(); delete x; x = NULL;    } }

#if defined(W_PLATFORM_WIN) && !defined(__WOLF_SYSTEM_STATIC_LIB)
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

#ifndef W_PATH_MAX
#define W_PATH_MAX 256
#endif

#ifndef W_MAX_BUFFER_SIZE
#define W_MAX_BUFFER_SIZE 4096
#endif

#define WOLF_MAJOR_VERSION 2    // Making incompatible API changes
#define WOLF_MINOR_VERSION 0    // Adding functionality in a backwards - compatible manner
#define WOLF_PATCH_VERSION 11   // bug fixes
#define WOLF_DEBUG_VERSION 38   // for debugging

#ifndef W_SUCCESS
#define W_SUCCESS 0
#endif

#ifndef W_FAILURE
#define W_FAILURE APR_EGENERAL
#endif

//typedef void (*w_job)(void*);
typedef int W_RESULT;
//typedef apr_off_t w_offset;

typedef
#ifdef W_PLATFORM_WIN
__int64
#elif defined W_PLATFORM_OSX
__darwin_off_t
#else

#endif
w_offset;

typedef struct apr_pool_t* w_mem_pool;
typedef struct apr_file_t* w_file;
typedef struct apr_finfo_t* w_file_info;
typedef struct apr_array_header_t* w_array;
typedef struct apr_hash_t* w_hash;

/**
 * initialize wolf
 * @return W_RESULT as a result
*/
W_SYSTEM_EXPORT
W_RESULT wolf_initialize(void);

/**
 * get default memory pool
 * @return memory pool
*/
W_SYSTEM_EXPORT
w_mem_pool w_get_default_memory_pool(void);

/**
 * create memory pool
 * @return memory pool
*/
W_SYSTEM_EXPORT
w_mem_pool w_create_memory_pool(void);

/**
 * allocate memory from default memory pool
 * @param pMemSize the size of memory
 * @param pTraceInfo trace infomation
 * @return memory in void pointer
*/
W_SYSTEM_EXPORT
void* w_malloc(_In_ const size_t pMemSize, _In_z_ const char* pTraceInfo);

/**
 * free memory from default memory pool
 * @param pMem the memory which is need to be free
*/
W_SYSTEM_EXPORT
void w_free(_In_ const void* pMem);

/**
 * initialize a string
 * @param pSource the constant string
 * @return allocated string from default memory pool
*/
W_SYSTEM_EXPORT
char* w_string(_In_ const char* pSource);

/**
 * initialize a string
 * @param pNumberOfArgs the number of argumans
 * @return concated string
*/
W_SYSTEM_EXPORT
char* w_string_concat(_In_ const int pNumberOfArgs, ...);

/**
 * release all resources of wolf
*/
W_SYSTEM_EXPORT
void wolf_terminate(void);

#ifdef __cplusplus
}
#endif
