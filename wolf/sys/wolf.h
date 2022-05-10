/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/WolfEngine/issues
	Website			 : https://WolfEngine.github.io
	Name			 : wolf.h
	Description		 : the base headers for WOLF
	Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _WIN32

#define WOLF_API __declspec(dllexport)

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <Windows.h>

#else

#define WOLF_API

#endif // _WIN32

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#ifndef MAX_TRACE_LEN
#define MAX_TRACE_LEN 256
#endif

#ifdef __cplusplus
}
#endif