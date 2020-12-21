/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_atomic.h
    Description      : atomic functions
    Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"

    //forward declaration
    typedef struct apr_table_t* w_table;
    typedef unsigned 
#ifdef _WIN64
        __int64
#elif defined _WIN32    
        __int32
#else

#endif
        W_ATOMIC_INT64;

#if defined (_WIN64) || defined (W_PLATFORM_ANDROID) || defined (W_PLATFORM_OSX) || defined (W_PLATFORM_IOS) || defined (W_PLATFORM_LINUX)

   /**
    * atomically increment an unsigned int64 by 1
    * @param pMem pointer to the mem object
    * @return old value pointed to by mem
    */
    W_SYSTEM_EXPORT
        W_ATOMIC_INT64 w_atomic_inc64(_Inout_ volatile W_ATOMIC_INT64* pMem);

   /**
    * atomically decrement an unsigned int64 by 1
    * @param pMem pointer to the mem object
    * @return zero if the value becomes zero on decrement, otherwise non-zero
    */
    W_SYSTEM_EXPORT
        int w_atomic_dec64(_Inout_ volatile W_ATOMIC_INT64* pMem);

   /**
    * atomically set an unsigned int64 in memory
    * @param pMem pointer to the object
    * @param pVal value that the object will assume
    */
    W_SYSTEM_EXPORT
        void w_atomic_set64(_Inout_ volatile W_ATOMIC_INT64* pMem, W_ATOMIC_INT64 pVal);

   /**
    * atomically read an unsigned int64 from memory
    * @param pMem the pointer
    */
    W_SYSTEM_EXPORT
        W_ATOMIC_INT64 w_atomic_read64(_Inout_ volatile W_ATOMIC_INT64* pMem);

#else

    W_SYSTEM_EXPORT
        W_ATOMIC_INT64  w_atomic_read32(_Inout_ volatile W_ATOMIC_INT64* pMem);

    W_SYSTEM_EXPORT
        W_ATOMIC_INT64 w_atomic_inc32(_Inout_ volatile W_ATOMIC_INT64* pVal);


    W_SYSTEM_EXPORT
        void w_atomic_set32(_Inout_ volatile W_ATOMIC_INT64* pMem, W_ATOMIC_INT64 pVal);

    W_SYSTEM_EXPORT
        int w_atomic_dec32(_Inout_ volatile W_ATOMIC_INT64* pVal);


#endif
   
#ifdef __cplusplus
}
#endif
