#include "w_video_decoder_factory_null_codec.hpp"

using w_video_decoder_factory_null_codec = wolf::stream::webRTC::w_video_decoder_factory_null_codec;

std::unique_ptr<webrtc::VideoDecoder> w_video_decoder_factory_null_codec::CreateVideoDecoder(
    const webrtc::SdpVideoFormat& p_format)
{
    return nullptr;
}

std::vector<webrtc::SdpVideoFormat> w_video_decoder_factory_null_codec::GetSupportedFormats() const
{
    return std::vector<webrtc::SdpVideoFormat>();
}