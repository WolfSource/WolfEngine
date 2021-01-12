/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_python.h
	Description		 : python script manager
	Comment          :
*/

#pragma once

#ifdef WOLF_ENABLE_PYTHON3

#include <wolf.h>
#include "memory/w_hash.h"
#include "memory/w_string.h"

typedef enum w_python_object_type
{
	W_PYTHON_BOOLEAN = 0,
	W_PYTHON_NUMBER_INT,
	W_PYTHON_NUMBER_DOUBLE,
	W_PYTHON_STRING,
} w_python_object_type;

typedef struct w_python_object_t
{
	w_python_object_type type;
	void* data;
} w_python_object_t;
typedef w_python_object_t* w_python_object;

struct w_python
{
	W_SYSTEM_EXPORT
	static void init();

	W_SYSTEM_EXPORT
	static W_RESULT execute(
		_In_ w_mem_pool pMemPool,
		_In_z_ const char* pScriptText,
		_In_opt_ w_hash pKeyValueParameters,
		_Inout_opt_ w_string* pErrorMsg);

	W_SYSTEM_EXPORT
	static void fini();
};

#endif