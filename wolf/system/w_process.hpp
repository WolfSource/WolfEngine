/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <wolf.hpp>

namespace wolf::system
{
    class w_process
    {
    public:
        // default constructor
        W_API w_process() = default;
        // destructor
        W_API virtual ~w_process() = default;
        // move constructor
        W_API w_process(w_process&& p_src) noexcept = default;
        // move assignment operator.
        W_API w_process& operator=(w_process&& p_src) noexcept = default;
        // copy constructor.
        w_process(const w_process&) = delete;
        // copy assignment operator.
        w_process& operator=(const w_process&) = delete;

    private:

    }
#ifdef __clang__
    W_ALIGNMENT_64
#endif
    ;
} // namespace wolf::system