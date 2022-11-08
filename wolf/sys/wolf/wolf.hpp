/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef WIN32

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

#endif

#include <memory>
#include <cstddef>

//gsl always terminate (std::terminate) on contract violation
#include <gsl/gsl>
#include <mimalloc-override.h>

using defer = std::shared_ptr<void>;

template<typename T>
bool w_is_null(T p)
{
    return p == nullptr;
}

template<typename T, typename... Args>
bool w_is_null(T p, Args... args)
{
    return p == nullptr || w_is_null(args...);
}

template<typename T> 
requires std::same_as<T, char>
void s_strcpy(
	_Inout_ T* p_out,
	_In_ const T* p_in)
{
#if defined(_MSC_VER) || defined(__STDC_LIB_EXT1__)
	strncpy_s(p_out, W_MAX_PATH, p_in, strlen(p_in));
#else
	auto _size = strlen(p_in);
	if (_size > W_MAX_PATH)
	{
		_size = W_MAX_PATH;
	}
	strncpy_s(p_out, p_in, _size);
#endif
}