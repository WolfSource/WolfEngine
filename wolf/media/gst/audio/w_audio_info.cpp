#include "media/gst/audio/w_audio_info.hpp"

namespace wolf::media::gst {

auto w_audio_info::make(w_audio_format p_format, size_t p_channels, size_t p_samples)
    -> boost::leaf::result<wolf::gst::w_audio_info>
{
    auto audioinfo_raw = gst_audio_info_new();
    if (!audioinfo_raw) {
        return W_FAILURE(std::errc::operation_canceled, "couldn't create audio info.");
    }

    auto format_raw = static_cast<GstAudioFormat>(p_format);
    gst_audio_info_set_format(
                audioinfo_raw,
                format_raw,
                p_samples,
                p_channels,
                nullptr
                );

    return w_audio_info(internal::w_raw_tag{}, audioinfo_raw);
}

}  // namespace wolf::media::gst
