#include "w_video_sink.hpp"

using w_video_sink = wolf::stream::webRTC::w_video_sink;

w_video_sink::w_video_sink(
    rtc::scoped_refptr<webrtc::VideoTrackInterface>& p_track)
    : _track(p_track)
{
    this->_track->AddOrUpdateSink(this, rtc::VideoSinkWants());
}

w_video_sink::~w_video_sink()
{
    this->_track->RemoveSink(this);
}

void w_video_sink::OnFrame(const webrtc::VideoFrame& p_video_frame)
{
    //rtc::scoped_refptr<webrtc::I420BufferInterface> buffer(
    //    p_video_frame.video_frame_buffer()->ToI420());
}