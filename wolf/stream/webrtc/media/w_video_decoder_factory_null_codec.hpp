/*
    Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <wolf.hpp>
#include <vector>
#include <memory>

#include "DISABLE_ANALYSIS_BEGIN"

#include <api/video_codecs/video_decoder.h>
#include <api/video_codecs/video_decoder_factory.h>
#include <api/video_codecs/sdp_video_format.h>

#include "DISABLE_ANALYSIS_END"


namespace wolf::stream::webRTC
{
    class w_video_decoder_factory_null_codec : public webrtc::VideoDecoderFactory
    {
    public:
        std::unique_ptr<webrtc::VideoDecoder> CreateVideoDecoder(
            const webrtc::SdpVideoFormat& p_format) override;

        std::vector<webrtc::SdpVideoFormat> GetSupportedFormats() const override;
    };
} // namespace wolf::stream::webRTC