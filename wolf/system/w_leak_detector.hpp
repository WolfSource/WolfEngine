/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef WIN32
#define _CRTDBG_MAP_ALLOC
#endif

#include <wolf.hpp>

namespace wolf::system
{
    class w_leak_detector
    {
    public:
        // default constructor
        W_API w_leak_detector();
        // destructor
        W_API virtual ~w_leak_detector();

        // move constructor
        W_API w_leak_detector(w_leak_detector&& p_src) noexcept = delete;
        // move assignment operator.
        W_API w_leak_detector& operator=(w_leak_detector&& p_src) noexcept = delete;

    private:
        // copy constructor.
        w_leak_detector(const w_leak_detector&) = delete;
        // copy assignment operator.
        w_leak_detector& operator=(const w_leak_detector&) = delete;

#ifdef WIN32
        _CrtMemState _mem_state;
#endif
    }
#ifdef __clang__
    W_ALIGNMENT_64
#endif
    ;
} // namespace wolf::system