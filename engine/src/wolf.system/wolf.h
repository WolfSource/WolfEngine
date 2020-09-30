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

#define W_SAFE_DELETE(x)            { if (x)  { delete x; x = NULL;                  } }
#define W_SAFE_DELETE_ARRAY(ar)     { if (ar) { delete[] ar; ar = NULL;              } }
#define W_SAFE_RELEASE(x)           { if (x)  { x->release(); delete x; x = NULL;    } }
#define W_ARRAY_SIZE(ar)	        (size_t)((sizeof(ar) / sizeof(ar[0])))

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

#ifndef W_TIMEOUT
#define W_TIMEOUT 5//Same as NNG_ETIMEDOUT
#endif

#ifndef W_BAD_ARG
#define W_BAD_ARG 70013//Same as APR_BADARG
#endif

    typedef
#ifdef W_PLATFORM_WIN
        __int64
#elif defined W_PLATFORM_OSX
        __darwin_off_t
#elif defined W_PLATFORM_ANDROID
        __kernel_off_t
#else

#endif
        w_offset;

    typedef struct apr_file_t* w_file;
    typedef struct apr_finfo_t* w_file_info;
    typedef struct w_arg_t
    {
        w_mem_pool pool;
        void* data;
    } w_arg_t;
    typedef struct w_arg_t* w_arg;

    /**
     * initialize wolf
     * @return W_RESULT as a result
    */
    W_SYSTEM_EXPORT
        W_RESULT wolf_init(void);

    /**
     * concatenate two or more char*
     * @param pMemPool The memory pool
     * @note make sure to append NULL
     * @return result chars
    */
    W_SYSTEM_EXPORT
        char* w_strcat(_In_ w_mem_pool pMemPool, ...);

#if defined(W_PLATFORM_WIN) || defined(W_PLATFORM_OSX) || defined(W_PLATFORM_LINUX)

    /**
     * concatenate two or more wchar_t*
     * @param pMemPool The memory pool
     * @note make sure to append NULL
     * @return concated chars
    */
    W_SYSTEM_EXPORT
        wchar_t* w_wstrcat(_In_ w_mem_pool pMemPool, ...);

#endif
    
    /**
     * release all resources of wolf
    */
    W_SYSTEM_EXPORT
        void wolf_fini(void);

#ifdef __cplusplus
}
#endif
