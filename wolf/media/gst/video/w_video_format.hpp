#pragma once

#include <gst/video/video-format.h>

namespace wolf::gst {

/**
 * enum same as GstVideoFormat.
 */
enum class w_video_format
{
    RGB = GST_VIDEO_FORMAT_RGB,
    RGBx = GST_VIDEO_FORMAT_RGBx,
    BGR = GST_VIDEO_FORMAT_BGR,
    BGRx = GST_VIDEO_FORMAT_BGRx
    // ...
};

}  // namespace wolf::gst
