#pragma once

#include "wolf.hpp"

#include "media/gst/internal/w_common.hpp"
#include "media/gst/internal/w_wrapper.hpp"
#include "media/gst/core/w_caps.hpp"
#include "media/gst/audio/w_audio_format.hpp"

#include <gst/audio/audio-info.h>

#include <stdexcept>
#include <utility>

namespace wolf::gst {

/**
 * wrapper of GstAudioInfo.
 */
class w_audio_info : public w_wrapper<w_audio_info, GstAudioInfo, void, gst_audio_info_free>
{
    friend class internal::w_raw_access;

public:
    /**
     * @brief make a w_audio_info instance with given format and other info.
     * @return a constructed audio info on success.
     */
    [[nodiscard]] static auto make(w_audio_format p_format,
                                   std::size_t p_channels = 2,
                                   std::size_t p_samples = 44100)
        -> boost::leaf::result<w_audio_info>;

    /**
     * @brief make a w_caps with info values.
     */
    [[nodiscard]] w_caps to_caps()
    {
        auto caps_raw = gst_audio_info_to_caps(raw());
        return internal::w_raw_access::from_raw<w_caps>(caps_raw);
    }

    /**
     * @brief set sample rate.
     * @param p_rate sample rate.
     */
    void set_rate(std::size_t p_rate)
    {
        raw()->rate = p_rate;
    }

    /**
     * @brief set channels.
     */
    void set_channels(std::size_t p_channels)
    {
        raw()->channels = p_channels;
    }

private:
    w_audio_info(internal::w_raw_tag, GstAudioInfo* p_rawptr) noexcept
        : w_wrapper(p_rawptr)
    {}
};

}  // namespace wolf::gst
