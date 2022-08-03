#include "w_audio_sink.hpp"

using w_audio_sink = wolf::stream::webRTC::w_audio_sink;

w_audio_sink::w_audio_sink(const rtc::scoped_refptr<webrtc::AudioTrackInterface>& p_track) 
    : _track(p_track)
{
    this->_track->AddSink(this);
}

w_audio_sink::~w_audio_sink()
{
    this->_track->RemoveSink(this);
}

void w_audio_sink::OnData(
    const void* p_audio_data,
    int p_bits_per_sample,
    int p_sample_rate,
    size_t p_number_of_channels,
    size_t p_number_of_frames) noexcept
{

}
