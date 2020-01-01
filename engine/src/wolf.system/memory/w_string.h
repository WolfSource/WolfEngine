/*
    Project            : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source             : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website            : https://WolfEngine.App
    Name               : w_string.h
    Description        : w_string helper for handling characters
    Comment            :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"

	/**
	 * create a string
	 * @return a pointer to string
	 */
	char* w_string_create(_In_ const size_t pSize);

	/**
	 * initialize string with another string
	 * @return a pointer to string
	 */
	char* w_string_init_with_string(_In_ const char* pSource);

	/**
	 * concat two strings (pLString += pRString)
	 * @param pNumberOfArgs number of Strings string
	 * @param ... strings
	 */
	char* w_string_concat(_In_ const int pNumberOfArgs, ...);

#ifdef __cplusplus
}
#endif


