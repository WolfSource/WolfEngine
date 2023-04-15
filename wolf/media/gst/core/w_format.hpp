#pragma once

#include <gst/gst.h>

namespace wolf::gst {

/** wrapper of GstFormat */
enum class w_format
{
    Undefined = GST_FORMAT_UNDEFINED,
    Default = GST_FORMAT_DEFAULT,
    Bytes = GST_FORMAT_BYTES,
    Time = GST_FORMAT_TIME,
    Buffers = GST_FORMAT_BUFFERS,
    Percent = GST_FORMAT_PERCENT
};

}  // namespace wolf::gst
