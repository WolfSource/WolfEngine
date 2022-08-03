/*
    Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <wolf.hpp>
#include "w_peer_conn_obs.hpp"
#include <regex>
#include <future>
#include <mutex>
#include <string>
#include <thread>

#include "DISABLE_ANALYSIS_BEGIN"

#include <api/scoped_refptr.h>
#include <api/media_stream_interface.h>

//#include <api/peer_connection_interface.h>
//#include <api/video_codecs/video_decoder_factory.h>

//#include <p2p/client/basic_port_allocator.h>
//#include <rtc_base/logging.h>
//#include <rtc_base/strings/json.h>
//#include <stream/http/w_http_server.hpp>

#include <rapidjson/document.h>

#include "DISABLE_ANALYSIS_END"

namespace wolf::stream::webRTC
{
    using w_audio_video_pair = std::pair<
        rtc::scoped_refptr<webrtc::VideoTrackSourceInterface>,
        rtc::scoped_refptr<webrtc::AudioSourceInterface>>;

    class w_peer_conn_manager
    {
    public:
        w_peer_conn_manager(
            const std::list<std::string> &p_ice_server_list,
            const rapidjson::Value &p_config,
            webrtc::AudioDeviceModule::AudioLayer p_audio_layer,
            const std::string &p_publish_filter,
            const std::string &p_webrtc_udp_port_range,
            bool p_use_null_codec = true,
            bool p_use_plan_b = true,
            int p_max_pc = 0);

        virtual ~w_peer_conn_manager();

        const rapidjson::Value add_ice_candidate(
            const std::string &p_peer_id,
            const rapidjson::Value &p_jmessage);

        rtc::scoped_refptr<webrtc::PeerConnectionInterface> get_peer_conn(const std::string &p_peer_id);
        const rapidjson::Value hang_up(const std::string& p_peer_id);
        
        const rapidjson::Document call(const std::string& p_peerid,
            const std::string& p_videourl,
            const std::string& p_audiourl,
            const std::string& p_options,
            const rapidjson::Value& p_message);

        std::unique_ptr<webrtc::SessionDescriptionInterface> get_answer(
            const std::string& p_peerid,
            webrtc::SessionDescriptionInterface* p_session_description,
            const std::string& p_videourl,
            const std::string& p_audiourl,
            const std::string& p_options);

        bool get_is_initialized() const;
        
       const rapidjson::Document get_ice_candidate_list(const std::string& p_peer_id);
       const rapidjson::Document get_video_device_list();
       const rapidjson::Document get_audio_device_list();
       const rapidjson::Document get_media_list();

        /**/
        //const std::map<std::string, http::w_http_function> get_http_api() const;
        

        // const Json::Value set_answer(const std::string& p_peerid,
        //     const Json::Value& p_message);

        //const rapidjson::Document get_ice_servers(const std::string &p_client_ip);

        // const Json::Value get_peer_conn_list();

        // const Json::Value get_stream_list();

        // const Json::Value create_offer(const std::string &p_peer_id,
        //                                const std::string &p_video_url,
        //                                const std::string &p_audio_url,
        //                                const std::string &p_options);

        // std::pair<std::map<std::string, std::string>, Json::Value> whip(const struct mg_request_info *p_req_info,
        //                                                                 const Json::Value &p_in);

    protected:
        void create_audio_module(webrtc::AudioDeviceModule::AudioLayer p_audio_layer);
        bool stream_still_used(const std::string& p_stream_label);
        
         bool add_streams(webrtc::PeerConnectionInterface* p_peer_conn,
             const std::string& p_video_url,
             const std::string& p_audio_url,
             const std::string& p_options);
         
        const std::string sanitize_label(const std::string& p_label);

        w_peer_conn_obs* create_peer_conn_obs(const std::string& p_peer_id);

        rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> create_video_source(
            const std::string& p_video_url,
            const std::map<std::string, std::string>& p_opts);

         rtc::scoped_refptr<webrtc::AudioSourceInterface> create_audio_source(
             const std::string& p_audio_url,
             const std::map<std::string, std::string>& p_opts);

        std::string get_oldest_peer_conn();

        std::unique_ptr<rtc::Thread> _signaling_thread;
        std::unique_ptr<rtc::Thread> _worker_thread;
        std::unique_ptr<rtc::Thread> _network_thread;
        rtc::scoped_refptr<webrtc::AudioDecoderFactory> _audio_decoder_factory;
        std::unique_ptr<webrtc::TaskQueueFactory> _task_queue_factory;
        rtc::scoped_refptr<webrtc::AudioDeviceModule> _audio_device_module;
        std::unique_ptr<webrtc::VideoDecoderFactory> _video_decoder_factory;
        rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> _peer_conn_factory;
        std::mutex _peer_map_mutex;
        std::map<std::string, w_peer_conn_obs *> _peer_conn_obs_map;
        std::map<std::string, w_audio_video_pair> _stream_map;
        std::mutex _stream_map_mutex;
        std::list<std::string> _ice_server_list;
        const rapidjson::Value &_config;
        std::map<std::string, std::string> _video_audio_map;
        const std::regex _publish_filter;
        // std::map<std::string, http::w_http_function> _funcs;
        std::string _webrtc_port_range;
        bool _use_null_codec;
        bool _use_plan_b;
        int _max_pc;
    };
} // namespace wolf::stream::webRTC