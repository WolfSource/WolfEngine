#include "w_av_config.hpp"

using w_av_config = wolf::media::ffmpeg::w_av_config;

w_av_config::w_av_config(_In_ AVPixelFormat p_format, _In_ int p_width, _In_ int p_height,
                         _In_ int p_alignment) noexcept
    : format(p_format), width(p_width), height(p_height), alignment(p_alignment) {}

w_av_config::w_av_config(_In_ int p_nb_channels, _In_ AVSampleFormat p_sample_fmts,
                         _In_ int p_sample_rate) noexcept
    : sample_rate(p_sample_rate), sample_fmts(p_sample_fmts), nb_channels(p_nb_channels) {}

int w_av_config::get_required_video_buffer_size() const noexcept {
  return av_image_get_buffer_size(this->format, this->width, this->height, this->alignment);
}
