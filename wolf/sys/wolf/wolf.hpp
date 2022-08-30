/*
    Project: Wolf Engine. Copyright � 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <memory>
#include <gsl/gsl>

#include <DISABLE_ANALYSIS_BEGIN>

//#include <mimalloc-new-delete.h>
#include <mimalloc-override.h>

#include <DISABLE_ANALYSIS_END>

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

