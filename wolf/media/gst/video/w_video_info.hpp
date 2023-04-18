#pragma once

#include "wolf.hpp"

#include "media/gst/internal/w_common.hpp"
#include "media/gst/internal/w_wrapper.hpp"
#include "media/gst/video/w_video_format.hpp"
#include "media/gst/core/w_caps.hpp"

#include <gst/video/video-info.h>

#include <stdexcept>
#include <utility>

namespace wolf::media::gst {

/**
 * wrapper of GstAudioInfo.
 */
class w_video_info : public w_wrapper<w_video_info, GstVideoInfo, void, gst_video_info_free>
{
    friend class internal::w_raw_access;

public:
    /**
     * @brief make an w_video_info instance with given format and other info.
     * @return a constructed audio info on success.
     */
    [[nodiscard]] static auto make(w_video_format p_format,
                                   std::size_t p_width,
                                   std::size_t p_height)
        -> boost::leaf::result<w_video_info>;

    /**
     * @brief make an equivalent w_caps instance from this audio info.
     */
    w_caps to_caps()
    {
        auto ptr = gst_video_info_to_caps(raw());
        return internal::w_raw_access::from_raw<w_caps>(ptr);
    }

    /**
     * @brief set playback fps.
     * @param p_fps frame per second.
     */
    void set_fps(std::size_t p_fps)
    {
        raw()->fps_n = 1;
        raw()->fps_d = gsl::narrow_cast<gint>(p_fps);
    }

    /**
     * @brief set playback fps in relative fraction.
     */
    void set_fps(std::size_t p_numerator, std::size_t p_denomirator)
    {
        raw()->fps_n = gsl::narrow_cast<gint>(p_numerator);
        raw()->fps_d = gsl::narrow_cast<gint>(p_denomirator);
    }

private:
    w_video_info(internal::w_raw_tag, GstVideoInfo* video_info_raw) noexcept
        : w_wrapper(video_info_raw)
    {}
};

}  // namespace wolf::media::gst
