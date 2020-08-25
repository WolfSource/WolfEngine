/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : wolf.h
	Description		 : the basic headers for WOLF
	Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <limits.h>
#include "memory/w_mem_pool.h"

    //#include <errno.h>

#define W_ASSERT(c, fmt)            { if (!c) { printf(fmt); assert(c);} }
#define W_ASSERT_P(c, fmt, ...)     { if (!c) { printf((fmt), __VA_ARGS__); assert(c);} }
#define W_SAFE_DELETE(x)            { if (x)  { delete x; x = NULL;                  } }
#define W_SAFE_DELETE_ARRAY(ar)     { if (ar) { delete[] ar; ar = NULL;              } }
#define W_SAFE_RELEASE(x)           { if (x)  { x->release(); delete x; x = NULL;    } }
#define W_ARRAY_SIZE(ar)	        (size_t)((sizeof(ar) / sizeof(ar[0])))

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

#ifndef W_MAX_BUFFER_SIZE
#define W_MAX_BUFFER_SIZE 4096
#endif

#define WOLF_MAJOR_VERSION 2    // Making incompatible API changes
#define WOLF_MINOR_VERSION 0    // Adding functionality in a backwards - compatible manner
#define WOLF_PATCH_VERSION 27   // bug fixes
#define WOLF_DEBUG_VERSION 58   // for debugging

#ifndef W_SUCCESS
#define W_SUCCESS 0
#endif

#ifndef W_FAILURE
#define W_FAILURE 1
#endif

    typedef
#ifdef W_PLATFORM_WIN
        __int64
#elif defined W_PLATFORM_OSX
        __darwin_off_t
#else

#endif
        w_offset;

    typedef struct apr_file_t* w_file;
    typedef struct apr_finfo_t* w_file_info;

    /**
     * initialize wolf
     * @return W_RESULT as a result
    */
    W_SYSTEM_EXPORT
        W_RESULT wolf_initialize(void);

    /**
     * concatenate two or more char*
     * @param pMemPool , The pool to use. set NULL to use default memory pool.
     * @return result chars
    */
    W_SYSTEM_EXPORT
        char* w_strcat(_In_ w_mem_pool pMemPool, ...);

    /**
     * concatenate two or more wchar_t*
     * @param pMemPool , The pool to use. set NULL to use default memory pool.
     * @return concated chars
    */
    W_SYSTEM_EXPORT
        wchar_t* w_wstrcat(_In_ w_mem_pool pMemPool, ...);

    /**
     * release all resources of wolf
    */
    W_SYSTEM_EXPORT
        void wolf_terminate(void);

#ifdef __cplusplus
}
#endif
