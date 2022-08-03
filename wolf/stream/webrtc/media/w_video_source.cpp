#include "w_video_source.hpp"

using w_video_source = wolf::stream::webRTC::w_video_source;

void w_video_source::AddOrUpdateSink(
    rtc::VideoSinkInterface<webrtc::VideoFrame>* p_sink,
    const rtc::VideoSinkWants& wants)
{
    this->broadcaster.AddOrUpdateSink(p_sink, wants);
}

void w_video_source::RemoveSink(rtc::VideoSinkInterface<webrtc::VideoFrame>* p_sink)
{
    this->broadcaster.RemoveSink(p_sink);
}

