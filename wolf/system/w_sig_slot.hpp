/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_SYSTEM_SIG_SLOT

#pragma once

#include <wolf.hpp>
#include <boost/signals2.hpp>

namespace wolf::system
{
    template <typename T>
    class w_sig_slot : public boost::signals2::signal<T>
    {
    };
} // namespace wolf::system

#endif