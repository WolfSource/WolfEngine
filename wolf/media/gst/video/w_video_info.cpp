#include "media/gst/video/w_video_info.hpp"

namespace wolf::media::gst {

auto w_video_info::make(w_video_format p_format, size_t p_width, size_t p_height)
    -> boost::leaf::result<w_video_info>
{
    auto video_info_raw = gst_video_info_new();
    if (!video_info_raw) {
        return W_FAILURE(std::errc::operation_canceled, "couldn't create video info.");
    }

    bool res = gst_video_info_set_format(
                video_info_raw,
                static_cast<GstVideoFormat>(p_format),
                gsl::narrow_cast<guint>(p_width),
                gsl::narrow_cast<guint>(p_height)
                );
    if (!res) {
        return W_FAILURE(std::errc::operation_canceled, "couldn't set video info.");
    }

    return w_video_info(internal::w_raw_tag{}, video_info_raw);
}

}  // namespace wolf::media::gst
