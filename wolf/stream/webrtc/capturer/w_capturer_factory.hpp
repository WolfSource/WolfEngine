/*
    Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <wolf.hpp>
#include <regex>
#include "w_vcm_capturer.hpp"

//#ifdef HAVE_V4L2
//#include "V4l2Capturer.h"
//#endif
//
//#ifdef HAVE_LIVE555
//#include "w_rtsp_video_capturer.h"
//#include "w_rtp_video_capturer.h"
//#include "w_file_video_capturer.h"
//#include "w_rtsp_audio_capturer.h"
//#include "w_file_audio_capturer.h"
//#endif
//
//#ifdef USE_X11
//#include "w_screen_capturer.h"
//#include "w_window_capturer.h"
//#endif
//
//#ifdef HAVE_RTMP
//#include "w_rtmp_video_source.h"
//#endif

#include "DISABLE_ANALYSIS_BEGIN"

#include <pc/video_track_source.h>
#include <api/peer_connection_interface.h>
#include <modules/audio_device/include/audio_device.h>

#include "DISABLE_ANALYSIS_END"

namespace wolf::stream::webRTC
{
    class w_capturer_factory
    {
    public:
        static const std::list<std::string> get_video_capture_device_list(
            const std::regex &p_publish_filter,
            bool p_use_null_codec);

        static const std::list<std::string> get_video_source_list(
            const std::regex &p_publish_filter,
            bool p_use_null_codec);

        static rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> create_video_source(
            const std::string &p_video_url,
            const std::map<std::string, std::string> &p_opts,
            const std::regex &p_publish_filter,
            rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> p_peer_connection_factory);

        static rtc::scoped_refptr<webrtc::AudioSourceInterface> create_audio_source(
            const std::string &p_audio_url,
            const std::map<std::string, std::string> &p_opts,
            const std::regex &p_publish_filter,
            rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> p_peer_connection_factory,
            rtc::scoped_refptr<webrtc::AudioDecoderFactory> p_audio_decoder_factory,
            rtc::scoped_refptr<webrtc::AudioDeviceModule> p_audio_device_module);

        static const std::list<std::string> get_audio_capture_device_list(
            const std::regex &p_publish_filter,
            rtc::scoped_refptr<webrtc::AudioDeviceModule> p_audio_device_module);
    };
} // namespace wolf::stream::webRTC