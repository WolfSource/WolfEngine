/*
    Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <wolf.hpp>
#include <chrono>
#include <stream/webrtc/media/w_audio_sink.hpp>
#include <stream/webrtc/data/w_data_channel_obs.hpp>
#include <stream/webrtc/media/w_video_sink.hpp>
#include "w_peer_conn_stats_collector_callback.hpp"

#include "DISABLE_ANALYSIS_BEGIN"

#include <api/peer_connection_interface.h>
#include <rapidjson/document.h>

#include "DISABLE_ANALYSIS_END"

namespace wolf::stream::webRTC
{
    constexpr auto CANDIDATE_SDP_MID_NAME = "sdpMid";
    constexpr auto CANDIDATE_SDP_MLINE_INDEX_NAME = "sdpMLineIndex";
    constexpr auto CANDIDATE_SDP_NAME = "candidate";

    class w_peer_conn_manager;
    class w_peer_conn_obs : public webrtc::PeerConnectionObserver
    {
    public:
        w_peer_conn_obs(w_peer_conn_manager* p_peer_conn_manager,
            const std::string& p_peerid,
            const webrtc::PeerConnectionInterface::RTCConfiguration& p_config);
        virtual ~w_peer_conn_obs();

        rapidjson::Document& get_ice_candidate_list();
        rapidjson::Document& get_stats();

        rtc::scoped_refptr<webrtc::PeerConnectionInterface> get_peer_conn();

        void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> p_stream) override;
        void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> p_stream) override;
        void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> p_channel) override;
        void OnRenegotiationNeeded() override;
        void OnIceCandidate(const webrtc::IceCandidateInterface* p_candidate) override;
        void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState p_state) override;
        void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState p_state) override;
        void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState p_state) override;

        uint64_t get_creation_time() const;
        std::string get_peer_id() const;

    private:
        w_peer_conn_manager* _peer_conn_manager;
        const std::string _peer_id;
        rtc::scoped_refptr<webrtc::PeerConnectionInterface> _pc;
        w_data_channel_obs* _local_channel;
        w_data_channel_obs* _remote_channel;
        rapidjson::Document _ice_candidate_list;
        rtc::scoped_refptr<w_peer_conn_stats_collector_callback> _stats_callback;
        std::unique_ptr<w_video_sink> _video_sink;
        std::unique_ptr<w_audio_sink> _audio_sink;
        bool _deleting;
        uint64_t _creation_time;
    };
}