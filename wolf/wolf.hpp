/*
    Project: Wolf Engine. Copyright � 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/
#pragma once

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <sstream>
#include <string>
#include <memory>
#include <gsl/gsl>

#ifdef WOLF_SYSTEM_MIMALLOC
#include <mimalloc-override.h>
#endif

#ifndef W_SAFE_DELETE
#define W_SAFE_DELETE(x) \
    {                    \
        if (x)           \
        {                \
            delete x;    \
            x = nullptr; \
        }                \
    }
#endif

#ifndef W_SAFE_DELETE_ARRAY
#define W_SAFE_DELETE_ARRAY(a) \
    {                          \
        if (a)                 \
        {                      \
            delete[] a;        \
            a = nullptr;       \
        }                      \
    }
#endif

#ifndef W_UNUSED
#define W_UNUSED(x) ((void)(x)) /* to avoid warnings */
#endif

#ifndef W_MAX_PATH
#define W_MAX_PATH 260
#endif

enum class W_RESULT
{
    SUCCESS = 0,
    FAILURE,
    TIMEOUT,
    BAD_ARG
};

namespace wolf
{
    /**
     * returns wolf version
     * @return string format with the following style
     * "<major>.<minor>.<patch>.<debug>"
     */
    auto w_version() -> std::string;

} // namespace wolf
