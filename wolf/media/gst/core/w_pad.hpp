#pragma once

#include "media/gst/internal/w_common.hpp"
#include "media/gst/internal/w_wrapper.hpp"

#include <gst/gst.h>

#include <utility>

namespace wolf::gst {

/** gstreamer pad availablity */
enum class w_availability
{
    Always,
    Request,
    Sometimes
};

/**
 * @brief wrapper of GstPad.
 *
 * each gstreamer element has source and/or sink pads,
 * which they connect by to each other with.
 */
class w_pad : public w_wrapper<w_pad, GstPad, void, gst_object_unref>
{
    friend class internal::w_raw_access;

public:
    // so far nothing...

private:
    explicit w_pad(internal::w_raw_tag, GstPad* p_pad) noexcept
        : w_wrapper(p_pad)
    {}
};

}  // namespace wolf::gst
