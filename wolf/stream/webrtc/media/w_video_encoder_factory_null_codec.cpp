#include "w_video_encoder_factory_null_codec.hpp"

using w_video_encoder_factory_null_codec = wolf::stream::webRTC::w_video_encoder_factory_null_codec;

std::unique_ptr<webrtc::VideoEncoder> w_video_encoder_factory_null_codec::CreateVideoEncoder(
    const webrtc::SdpVideoFormat& p_format)
{
    return nullptr;
}

std::vector<webrtc::SdpVideoFormat> w_video_encoder_factory_null_codec::GetSupportedFormats() const
{
    return std::vector<webrtc::SdpVideoFormat>();
}