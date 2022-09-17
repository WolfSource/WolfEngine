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