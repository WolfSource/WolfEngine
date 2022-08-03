/*
    Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <wolf.hpp>

#include "DISABLE_ANALYSIS_BEGIN"

#include <api/media_stream_interface.h>
#include <api/scoped_refptr.h>

#include "DISABLE_ANALYSIS_END"

namespace wolf::stream::webRTC
{
    class w_audio_sink : public webrtc::AudioTrackSinkInterface
    {
    public:
        w_audio_sink(const rtc::scoped_refptr<webrtc::AudioTrackInterface> &p_track);
        virtual ~w_audio_sink();

        void OnData(const void *p_audio_data, int p_bits_per_sample,
                    int p_sample_rate, size_t p_number_of_channels,
                    size_t p_number_of_frames) noexcept override;

    protected:
        rtc::scoped_refptr<webrtc::AudioTrackInterface> _track;
    };
} // namespace wolf::stream::webRTC