/*
    Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <wolf.hpp>

#include "DISABLE_ANALYSIS_BEGIN"

#include <api/video/video_frame.h>
#include <api/video/video_sink_interface.h>
#include <api/video/video_source_interface.h>
#include <media/base/video_broadcaster.h>

#include "DISABLE_ANALYSIS_END"
namespace wolf::stream::webRTC
{
    class w_video_source : public rtc::VideoSourceInterface<webrtc::VideoFrame>
    {
    public:
        void AddOrUpdateSink(
            rtc::VideoSinkInterface<webrtc::VideoFrame> *p_sink,
            const rtc::VideoSinkWants &wants) override;

        void RemoveSink(rtc::VideoSinkInterface<webrtc::VideoFrame> *p_sink) override;

    protected:
        rtc::VideoBroadcaster broadcaster;
    };
} // namespace wolf::stream::webRTC
