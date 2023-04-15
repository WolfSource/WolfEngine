#pragma once

#include "media/gst/internal/w_wrapper.hpp"

#include <gst/gst.h>

namespace wolf::gst {

/**
 * @brief wrapper of GstClock.
 *
 * @note this class is not user constructible. it's nonowning by default.
 */
class w_clock : public w_wrapper<w_clock, GstClock, void, +[](GstClock*){ /* do nothing */ }>
{
    friend class internal::w_raw_access;

public:
    w_clock() = delete;

    /**
     * @brief get time in nanoseconds.
     * @return time in nanoseconds.
     */
    std::size_t get_time() noexcept
    {
        return gst_clock_get_time(raw());
    }

private:
    explicit w_clock(internal::w_raw_tag, GstClock* p_clock_raw) noexcept
        : w_wrapper(p_clock_raw)
    {}
};

}  // namespace wolf::gst
