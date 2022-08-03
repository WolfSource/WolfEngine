/*
    Project: Wolf Engine. Copyright � 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/
#pragma once

#include <sstream>
#include <string>
#include <gsl/gsl>

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

namespace wolf
{
    /**
     * returns wolf version
     * @return string format with the following style
     * "<major>.<minor>.<patch>.<debug>"
     */
    auto w_version() -> std::string;

} // namespace wolf
