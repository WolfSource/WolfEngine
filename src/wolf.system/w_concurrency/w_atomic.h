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
        W_ATOMIC_INT;

   /**
    * atomically increment an unsigned int64 by 1
    * @param pMem pointer to the mem object
    */
    W_SYSTEM_EXPORT
        void w_atomic_inc(_Inout_ volatile W_ATOMIC_INT* pMem);

   /**
    * atomically decrement an unsigned int64 by 1
    * @param pMem pointer to the mem object
    */
    W_SYSTEM_EXPORT
        void w_atomic_dec(_Inout_ volatile W_ATOMIC_INT* pMem);

   /**
    * atomically set an unsigned int64 in memory
    * @param pMem pointer to the object
    * @param pVal value that the object will assume
    */
    W_SYSTEM_EXPORT
        void w_atomic_set(_Inout_ volatile W_ATOMIC_INT* pMem, W_ATOMIC_INT pVal);

   /**
    * atomically read an unsigned int64 from memory
    * @param pMem the pointer
    */
    W_SYSTEM_EXPORT
        W_ATOMIC_INT w_atomic_read(_Inout_ volatile W_ATOMIC_INT* pMem);

   
#ifdef __cplusplus
}
#endif
