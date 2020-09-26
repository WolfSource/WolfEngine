/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_string.h
    Description      : a string
    Comment          : 
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"

    typedef struct w_string_t
    {
        char* data;
        size_t      str_len;
        size_t      reserved_size;
    } w_string_t;
    typedef struct w_string_t* w_string;

    /**
     * create a string
     * @param pMemPool memory pool
     * @param pStringView pointer to string view
     * @param pData const char*
     * @return string view
    */
    W_SYSTEM_EXPORT
        W_RESULT w_string_init(
            _Inout_ w_mem_pool pMemPool,
            _Inout_ w_string* pStringView,
            _In_ const char* pData);

    /**
     * create a duplicate from string view
     * @param pMemPool memory pool
     * @param pDst pointer to destination string view
     * @param pSrc pointer to source string view
     * @return duplicated string view
    */
    W_SYSTEM_EXPORT
        W_RESULT w_string_dup(
            _Inout_ w_mem_pool pMemPool,
            _Inout_ w_string* pDst,
            _Inout_ w_string pSrc);

    /**
     * check string view is empty or not
     * @param pDst pointer to destination string view
     * @param pSrc pointer to source string view
     * @return duplicated string view
    */
    W_SYSTEM_EXPORT
        W_RESULT w_string_is_empty(_In_ w_string pStr);

   /**
    * to lower a string
    * @param pString input string
    * @param pStringLen the len of string
    */
    W_SYSTEM_EXPORT
        void w_string_to_lower(_Inout_z_ w_string pString);

    /**
     * clear string 
     * @param pSrc pointer to the string view
     * @return duplicated string view
    */
    W_SYSTEM_EXPORT
        W_RESULT w_string_clear(_Inout_ w_string* pStr);

#ifdef __cplusplus
}
#endif
