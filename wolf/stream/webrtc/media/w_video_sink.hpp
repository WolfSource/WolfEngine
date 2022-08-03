/*
    Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <wolf.hpp>

#include "DISABLE_ANALYSIS_BEGIN"

#include <api/media_stream_interface.h>
#include <api/scoped_refptr.h>
#include <api/video/video_frame.h>

#include "DISABLE_ANALYSIS_END"

namespace wolf::stream::webRTC
{
    class w_video_sink : public rtc::VideoSinkInterface<webrtc::VideoFrame>
    {
    public:
        w_video_sink(rtc::scoped_refptr<webrtc::VideoTrackInterface>& p_track);
        virtual ~w_video_sink();

        virtual void OnFrame(const webrtc::VideoFrame& p_video_frame) override;

    protected:
        rtc::scoped_refptr<webrtc::VideoTrackInterface> _track;
    };
} // namespace wolf::stream::webRTC