#include "w_peer_conn_manager.hpp"
#include "w_peer_conn_manager_helpers.hpp"
#include "w_peer_conn_obs.hpp"

#include <stream/webrtc/media/w_v4l2_alsa_map.hpp>
#include <stream/webrtc/interceptor/w_set_session_desc_obs.hpp>
#include <stream/webrtc/interceptor/w_create_session_desc_obs.hpp>
#include <stream/webrtc/capturer/w_capturer_factory.hpp>

#include "DISABLE_ANALYSIS_BEGIN"

#include <api/task_queue/default_task_queue_factory.h>
#include <api/audio_codecs/builtin_audio_decoder_factory.h>
#include <modules/audio_device/include/fake_audio_device.h>

#include "DISABLE_ANALYSIS_END"

//#include "modules/audio_device/include/fake_audio_device.h"
//#include "w_ice_server.hpp"
//#include "w_video_decoder_factory_null_codec.hpp"
//#include "w_video_encoder_factory_null_codec.hpp"
//#include <api/video_codecs/video_decoder_factory.h>
//#include <api/video_codecs/video_encoder_factory.h>
//#include <fstream>
//#include <functional>
//#include <iostream>
//#include <memory>
//#include <utility>
//

// #include "NullEncoder.h"
// #include "NullDecoder.h"

using w_peer_conn_manager = wolf::stream::webRTC::w_peer_conn_manager;
using w_peer_conn_obs = wolf::stream::webRTC::w_peer_conn_obs;

// using w_http_function = wolf::stream::http::w_http_function;
// using w_set_session_des_obs = wolf::stream::webRTC::w_set_session_desc_obs;
// using w_create_session_desc_obs = wolf::stream::webRTC::w_create_session_desc_obs;

// Names used for a SessionDescription JSON object.
constexpr char s_session_desc_type_name[] = "type";
constexpr char s_session_desc_sdp_name[] = "sdp";

w_peer_conn_manager::w_peer_conn_manager(
    const std::list<std::string> &p_ice_server_list,
    const rapidjson::Value &p_config,
    webrtc::AudioDeviceModule::AudioLayer p_audio_layer,
    const std::string &p_publish_filter,
    const std::string &p_webrtc_udp_port_range,
    bool p_use_null_codec,
    bool p_use_plan_b,
    int p_max_pc) : _signaling_thread(rtc::Thread::Create()),
                    _worker_thread(rtc::Thread::Create()),
                    _audio_decoder_factory(webrtc::CreateBuiltinAudioDecoderFactory()),
                    _task_queue_factory(webrtc::CreateDefaultTaskQueueFactory()),
                    _video_decoder_factory(w_peer_conn_manager_helpers::create_decoder_factory(p_use_null_codec)),
                    _ice_server_list(p_ice_server_list),
                    _config(p_config),
                    _publish_filter(p_publish_filter),
                    _webrtc_port_range(p_webrtc_udp_port_range),
                    _use_null_codec(p_use_null_codec),
                    _use_plan_b(p_use_plan_b),
                    _max_pc(p_max_pc)
{
    this->_worker_thread->SetName("worker", nullptr);
    this->_worker_thread->Start();

    this->_worker_thread->Invoke<void>(RTC_FROM_HERE, [this, p_audio_layer]
                                       { create_audio_module(p_audio_layer); });

    this->_signaling_thread->SetName("signaling", nullptr);
    this->_signaling_thread->Start();
    this->_peer_conn_factory = webrtc::CreateModularPeerConnectionFactory(
        w_peer_conn_manager_helpers::create_peer_conn_factory_deps(
            this->_signaling_thread.get(),
            this->_worker_thread.get(),
            this->_audio_device_module,
            this->_audio_decoder_factory,
            p_use_null_codec));

    // build video audio map
    this->_video_audio_map = get_v4l2_alsa_map();

    // register api in http server
    // m_func["/api/getMediaList"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     return std::make_pair(std::map<std::string, std::string>(), this->getMediaList());
    // };

    // m_func["/api/getVideoDeviceList"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     return std::make_pair(std::map<std::string, std::string>(), this->getVideoDeviceList());
    // };

    // m_func["/api/getAudioDeviceList"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     return std::make_pair(std::map<std::string, std::string>(), this->getAudioDeviceList());
    // };

    // m_func["/api/getIceServers"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     return std::make_pair(std::map<std::string, std::string>(), this->getIceServers(req_info->remote_addr));
    // };

    // m_func["/api/call"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     std::string peerid;
    //     std::string url;
    //     std::string audiourl;
    //     std::string options;
    //     if (req_info->query_string)
    //     {
    //         CivetServer::getParam(req_info->query_string, "peerid", peerid);
    //         CivetServer::getParam(req_info->query_string, "url", url);
    //         CivetServer::getParam(req_info->query_string, "audiourl", audiourl);
    //         CivetServer::getParam(req_info->query_string, "options", options);
    //     }
    //     return std::make_pair(std::map<std::string, std::string>(), this->call(peerid, url, audiourl, options, in));
    // };

    // m_func["/api/whip"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     return this->whip(req_info, in);
    // };

    // m_func["/api/hangup"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     std::string peerid;
    //     if (req_info->query_string)
    //     {
    //         CivetServer::getParam(req_info->query_string, "peerid", peerid);
    //     }
    //     return std::make_pair(std::map<std::string, std::string>(), this->hangUp(peerid));
    // };

    // m_func["/api/createOffer"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     std::string peerid;
    //     std::string url;
    //     std::string audiourl;
    //     std::string options;
    //     if (req_info->query_string)
    //     {
    //         CivetServer::getParam(req_info->query_string, "peerid", peerid);
    //         CivetServer::getParam(req_info->query_string, "url", url);
    //         CivetServer::getParam(req_info->query_string, "audiourl", audiourl);
    //         CivetServer::getParam(req_info->query_string, "options", options);
    //     }
    //     return std::make_pair(std::map<std::string, std::string>(), this->createOffer(peerid, url, audiourl, options));
    // };
    // m_func["/api/setAnswer"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     std::string peerid;
    //     if (req_info->query_string)
    //     {
    //         CivetServer::getParam(req_info->query_string, "peerid", peerid);
    //     }
    //     return std::make_pair(std::map<std::string, std::string>(), this->setAnswer(peerid, in));
    // };

    // m_func["/api/getIceCandidate"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     std::string peerid;
    //     if (req_info->query_string)
    //     {
    //         CivetServer::getParam(req_info->query_string, "peerid", peerid);
    //     }
    //     return std::make_pair(std::map<std::string, std::string>(), this->getIceCandidateList(peerid));
    // };

    // m_func["/api/addIceCandidate"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     std::string peerid;
    //     if (req_info->query_string)
    //     {
    //         CivetServer::getParam(req_info->query_string, "peerid", peerid);
    //     }
    //     return std::make_pair(std::map<std::string, std::string>(), this->addIceCandidate(peerid, in));
    // };

    // m_func["/api/getPeerConnectionList"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     return std::make_pair(std::map<std::string, std::string>(), this->getPeerConnectionList());
    // };

    // m_func["/api/getStreamList"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     return std::make_pair(std::map<std::string, std::string>(), this->getStreamList());
    // };

    // m_func["/api/version"] = [](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     Json::Value answer(VERSION);
    //     return std::make_pair(std::map<std::string, std::string>(), answer);
    // };
    // m_func["/api/log"] = [](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     std::string loglevel;
    //     if (req_info->query_string)
    //     {
    //         CivetServer::getParam(req_info->query_string, "level", loglevel);
    //         if (!loglevel.empty())
    //         {
    //             rtc::LogMessage::LogToDebug((rtc::LoggingSeverity)atoi(loglevel.c_str()));
    //         }
    //     }
    //     Json::Value answer(rtc::LogMessage::GetLogToDebug());
    //     return std::make_pair(std::map<std::string, std::string>(), answer);
    // };
    // m_func["/api/help"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     Json::Value answer;
    //     for (auto it : m_func)
    //     {
    //         answer.append(it.first);
    //     }
    //     return std::make_pair(std::map<std::string, std::string>(), answer);
    // };
}

w_peer_conn_manager::~w_peer_conn_manager()
{
    this->_worker_thread->Invoke<void>(
        RTC_FROM_HERE, [this]
        { this->_audio_device_module->Release(); });
}

void w_peer_conn_manager::create_audio_module(webrtc::AudioDeviceModule::AudioLayer p_audio_layer)
{
#ifdef HAVE_SOUND
    this->_audio_device_module = webrtc::AudioDeviceModule::Create(
        p_audio_layer,
        this->_task_queue_factory.get());
    if (this->_audio_device_module->Init() != 0)
    {
        this->_audio_device_module = new webrtc::FakeAudioDeviceModule();
    }
#else
    this->_audio_device_module = new webrtc::FakeAudioDeviceModule();
#endif
}

const rapidjson::Value w_peer_conn_manager::add_ice_candidate(
    const std::string &p_peer_id,
    const rapidjson::Value &p_jmessage)
{
    int _sdp_line_index = 0;
    std::string _sdp_mid, _sdp;

    auto _end_json_msg = p_jmessage.MemberEnd();

    auto _sdp_line_iter = p_jmessage.FindMember(CANDIDATE_SDP_MLINE_INDEX_NAME);
    auto _sdp_mid_iter = p_jmessage.FindMember(CANDIDATE_SDP_MID_NAME);
    auto _sdp_iter = p_jmessage.FindMember(CANDIDATE_SDP_NAME);

    if (_sdp_line_iter == _end_json_msg ||
        _sdp_mid_iter == _end_json_msg ||
        _sdp_iter == _end_json_msg)
    {
        rapidjson::Value answer;
        answer.SetBool(false);
        return answer;
    }

    bool _result = true;

    _sdp_line_index = _sdp_line_iter->value.GetInt();
    _sdp_mid = _sdp_mid_iter->value.GetString();
    _sdp = _sdp_iter->value.GetString();

    std::unique_ptr<webrtc::IceCandidateInterface> _candidate(
        webrtc::CreateIceCandidate(_sdp_mid, _sdp_line_index, _sdp, nullptr));
    if (_candidate.get() == nullptr)
    {
        _result = false;
    }
    else
    {
        std::lock_guard<std::mutex> _peer_lock(this->_peer_map_mutex);
        rtc::scoped_refptr<webrtc::PeerConnectionInterface> _peer_conn = get_peer_conn(p_peer_id);
        if (_peer_conn)
        {
            _result = _peer_conn->AddIceCandidate(_candidate.get());
        }
    }

    rapidjson::Value answer;
    answer.SetBool(_result);
    return answer;
}

rtc::scoped_refptr<webrtc::PeerConnectionInterface> w_peer_conn_manager::get_peer_conn(const std::string &p_peer_id)
{
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_conn;
    auto _iter = this->_peer_conn_obs_map.find(p_peer_id);
    if (_iter != this->_peer_conn_obs_map.end())
    {
        peer_conn = _iter->second->get_peer_conn();
    }
    return peer_conn;
}

const rapidjson::Value w_peer_conn_manager::hang_up(const std::string& p_peer_id)
{
    bool result = false;

    w_peer_conn_obs* _pc_observer = nullptr;
    {
        std::lock_guard<std::mutex> _peer_lock(this->_peer_map_mutex);
        auto it = this->_peer_conn_obs_map.find(p_peer_id);
        if (it != this->_peer_conn_obs_map.end())
        {
            _pc_observer = it->second;
            this->_peer_conn_obs_map.erase(it);
        }

        if (_pc_observer)
        {
            auto _peer_conn = _pc_observer->get_peer_conn();
            auto _local_streams = _peer_conn->GetSenders();
            for (auto stream : _local_streams)
            {
                auto stream_vector = stream->stream_ids();
                if (stream_vector.size() > 0)
                {
                    auto _stream_label = stream_vector[0];
                    bool _still_used = stream_still_used(_stream_label);
                    if (!_still_used)
                    {
                        std::lock_guard<std::mutex> mlock(this->_stream_map_mutex);
                        auto it = this->_stream_map.find(_stream_label);
                        if (it != this->_stream_map.end())
                        {
                            this->_stream_map.erase(it);
                        }
                    }

                    _peer_conn->RemoveTrack(stream);
                }
            }

            delete _pc_observer;
            result = true;
        }
    }

    rapidjson::Value answer;
    if (result)
    {
        answer = result;
    }
    return answer;
}

bool w_peer_conn_manager::stream_still_used(const std::string& p_stream_label)
{
    bool _still_used = false;
    for (auto it : this->_peer_conn_obs_map)
    {
        auto peer_conn = it.second->get_peer_conn();
        auto local_streams = peer_conn->GetSenders();

        for (auto stream : local_streams)
        {
            auto stream_vector = stream->stream_ids();
            if (stream_vector.size() > 0)
            {
                if (stream_vector[0] == p_stream_label)
                {
                    _still_used = true;
                    break;
                }
            }
        }
    }
    return _still_used;
}

bool w_peer_conn_manager::add_streams(
    webrtc::PeerConnectionInterface* p_peer_conn,
    const std::string& p_video_url,
    const std::string& p_audio_url,
    const std::string& p_options)
{
    bool ret = false;

    // compute options
    std::string _opt_string = p_options;
    if (!p_video_url.empty() && this->_config.HasMember(p_video_url.c_str()))
    {
        auto* _value = &this->_config[p_video_url.c_str()]["options"];
        std::string urlopts = _value->GetString();
        if (p_options.empty())
        {
            _opt_string = urlopts;
        }
        else if (p_options.find_first_of("&") == 0)
        {
            _opt_string = urlopts + p_options;
        }
        else
        {
            _opt_string = p_options;
        }
    }

    // convert options string into map
    std::istringstream _is(_opt_string);
    std::map<std::string, std::string> _opts;
    std::string key, value;
    while (std::getline(std::getline(_is, key, '='), value, '&'))
    {
        _opts[key] = value;
    }

    std::string video = p_video_url;
    if (this->_config.HasMember(video.c_str()))
    {
        video = this->_config[video.c_str()]["video"].GetString();
    }

    // compute audiourl if not set
    std::string audio(p_audio_url);
    if (audio.empty())
    {
        audio = p_video_url;
    }

    // set bandwidth
    if (_opts.find("bitrate") != _opts.end())
    {
        int _bitrate = std::stoi(_opts.at("bitrate"));

        webrtc::BitrateSettings _bitrate_param = {};
        _bitrate_param.min_bitrate_bps = absl::optional<int>(_bitrate / 2);
        _bitrate_param.start_bitrate_bps = absl::optional<int>(_bitrate);
        _bitrate_param.max_bitrate_bps = absl::optional<int>(_bitrate * 2);
        p_peer_conn->SetBitrate(_bitrate_param);
    }

    // keep capturer options (to improve!!!)
    std::string _opt_capturer;
    if ((video.find("rtsp://") == 0) || (audio.find("rtsp://") == 0))
    {
        if (_opts.find("rtptransport") != _opts.end())
        {
            _opt_capturer += _opts["rtptransport"];
        }
        if (_opts.find("timeout") != _opts.end())
        {
            _opt_capturer += _opts["timeout"];
        }
    }

    // compute stream label removing space because SDP use label
    std::string _stream_label = sanitize_label(
        p_video_url + "|" + p_audio_url + "|" + _opt_capturer);

    bool _existing_stream = false;
    {
        std::lock_guard<std::mutex> mlock(this->_stream_map_mutex);
        _existing_stream = (this->_stream_map.find(_stream_label) != this->_stream_map.end());
    }

    if (!_existing_stream)
    {
        // need to create the stream
        rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> _video_source(create_video_source(video, _opts));
        rtc::scoped_refptr<webrtc::AudioSourceInterface> _audio_source(create_audio_source(audio, _opts));
        
        std::lock_guard<std::mutex> mlock(this->_stream_map_mutex);
        this->_stream_map[_stream_label] = std::make_pair(_video_source, _audio_source);
    }

    // create a new webrtc stream
    {
        std::lock_guard<std::mutex> _lock(this->_stream_map_mutex);
        auto it = this->_stream_map.find(_stream_label);
        if (it != this->_stream_map.end())
        {
            auto pair = it->second;
            rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> _video_source(pair.first);
            if (!_video_source)
            {
                // RTC_LOG(LS_ERROR) << "Cannot create capturer video:" << p_video_url;
            }
            else
            {
                rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track =
                    _peer_conn_factory->CreateVideoTrack(_stream_label + "_video", _video_source.get());

                auto _add_track_is_ok = p_peer_conn->AddTrack(video_track, { _stream_label }).ok();
                if (video_track && !_add_track_is_ok)
                {
                }
                else
                {
                    ret = true;
                }
            }

            rtc::scoped_refptr<webrtc::AudioSourceInterface> _audio_source(pair.second);
            if (!_audio_source)
            {
                // RTC_LOG(LS_ERROR) << "Cannot create capturer audio:" << audio;
            }
            else
            {
                auto audio_track = this->_peer_conn_factory->CreateAudioTrack(
                    _stream_label + "_audio", _audio_source.get());
                auto _track_added = p_peer_conn->AddTrack(audio_track, { _stream_label }).ok();
                if (audio_track && _track_added)
                {
                    ret = true;
                }
            }
        }
        else
        {
        }
    }

    return ret;
}

const std::string w_peer_conn_manager::sanitize_label(const std::string& p_label)
{
    std::string _out(p_label);

    // conceal labels that contain rtsp URL to prevent sensitive data leaks.
    if (p_label.find("rtsp:") != std::string::npos)
    {
        std::hash<std::string> hash_fn;
        size_t hash = hash_fn(_out);
        return std::to_string(hash);
    }

    _out.erase(std::remove_if(_out.begin(), _out.end(),
        w_peer_conn_manager_helpers::ignore_in_label), _out.end());
    return _out;
}

w_peer_conn_obs* w_peer_conn_manager::create_peer_conn_obs(const std::string& p_peer_id)
{
    std::string oldestpeerid = get_oldest_peer_conn();
    if (!oldestpeerid.empty())
    {
        hang_up(oldestpeerid);
    }

    webrtc::PeerConnectionInterface::RTCConfiguration config;
    if (this->_use_plan_b)
    {
        config.sdp_semantics = webrtc::SdpSemantics::kPlanB;
    }
    else
    {
        config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
    }
    for (auto ice_server : this->_ice_server_list)
    {
        auto _srv = w_peer_conn_manager_helpers::get_ice_server_from_url(ice_server);

        webrtc::PeerConnectionInterface::IceServer _server = {};
        _server.uri = _srv.url;
        _server.username = _srv.user;
        _server.password = _srv.pass;

        config.servers.push_back(_server);
    }

    // Use example From https://soru.site/questions/51578447/api-c-webrtcyi-kullanarak-peerconnection-ve-ucretsiz-baglant-noktasn-serbest-nasl
    int _min_port = 0;
    int _max_port = 65535;
    std::istringstream _is(this->_webrtc_port_range);
    std::string port;
    if (std::getline(_is, port, ':'))
    {
        _min_port = std::stoi(port);
        if (std::getline(_is, port, ':'))
        {
            _max_port = std::stoi(port);
        }
    }

    //config.port_allocator_config.min_port = _min_port;
    //config.port_allocator_config.max_port = _max_port;

    auto* _obs = new (std::nothrow) w_peer_conn_obs(this, p_peer_id, config);
    if (!_obs)
    {
        //LOG
    }
    return _obs;
}

rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> w_peer_conn_manager::create_video_source(
    const std::string& p_video_url,
    const std::map<std::string, std::string>& p_opts)
{
    std::string _video = p_video_url;
    auto _video_cstr = _video.c_str();
    if (_video_cstr && this->_config.HasMember(_video_cstr))
    {
        _video = this->_config[_video_cstr]["video"].GetString();
    }

    return w_capturer_factory::create_video_source(
        _video,
        p_opts,
        this->_publish_filter,
        this->_peer_conn_factory);
}

rtc::scoped_refptr<webrtc::AudioSourceInterface> w_peer_conn_manager::create_audio_source(
    const std::string& p_audio_url,
    const std::map<std::string, std::string>& p_opts)
{
    std::string _audio = p_audio_url;
    auto _audio_cstr = _audio.c_str();
    if (_audio_cstr && this->_config.HasMember(_audio_cstr))
    {
        _audio = this->_config[_audio_cstr]["audio"].GetString();
    }

    auto _it = this->_video_audio_map.find(_audio);
    if (_it != this->_video_audio_map.end())
    {
        _audio = _it->second;
    }

    return this->_worker_thread->Invoke<rtc::scoped_refptr<webrtc::AudioSourceInterface>>(
        RTC_FROM_HERE,
        [this, _audio, p_opts]
        {
            return w_capturer_factory::create_audio_source(
                _audio,
                p_opts,
                this->_publish_filter,
                this->_peer_conn_factory,
                this->_audio_decoder_factory,
                this->_audio_device_module);
        });
}

const rapidjson::Document w_peer_conn_manager::call(
    const std::string& p_peer_id,
    const std::string& p_video_url,
    const std::string& p_audio_url,
    const std::string& p_options,
    const rapidjson::Value& p_jmessage)
{
    bool _result = true;
    rapidjson::Document _answer = {};

    auto _end_json_msg = p_jmessage.MemberEnd();
    auto _type_val = p_jmessage.FindMember(s_session_desc_type_name);
    auto _sdp_val = p_jmessage.FindMember(s_session_desc_sdp_name);

    if (_type_val == _end_json_msg || _sdp_val == _end_json_msg)
    {
        _result = false;
    }
    else
    {
        auto type = _type_val->value.GetString();
        auto sdp = _sdp_val->value.GetString();

        webrtc::SessionDescriptionInterface* _session_description(
            webrtc::CreateSessionDescription(type, sdp, nullptr));
        if (!_session_description)
        {
            _result = false;
        }
        else
        {
            auto desc = get_answer(
                p_peer_id,
                _session_description,
                p_video_url,
                p_audio_url,
                p_options);
            if (desc.get())
            {
                std::string sdp;
                desc->ToString(&sdp);

                _answer[s_session_desc_type_name].SetString(desc->type().c_str(), _answer.GetAllocator());
                _answer[s_session_desc_sdp_name].SetString(sdp.c_str(), _answer.GetAllocator());
            }
            else
            {
                _result = false;
            }
        }
    }
    _answer.SetBool(_result);
    return _answer;
}

std::unique_ptr<webrtc::SessionDescriptionInterface> w_peer_conn_manager::get_answer(
    const std::string& p_peer_id,
    webrtc::SessionDescriptionInterface* p_session_desc,
    const std::string& p_video_url,
    const std::string& p_audio_url,
    const std::string& p_options)
{
    std::unique_ptr<webrtc::SessionDescriptionInterface> answer;

    auto* _peer_conn_obs = create_peer_conn_obs(p_peer_id);
    if (!_peer_conn_obs)
    {
        // RTC_LOG(LS_ERROR) << "Failed to initialize PeerConnectionObserver";
    }
    else if (!_peer_conn_obs->get_peer_conn().get())
    {
        // RTC_LOG(LS_ERROR) << "Failed to initialize PeerConnection";
        delete _peer_conn_obs;
    }
    else
    {
        auto _peer_conn = _peer_conn_obs->get_peer_conn();

        // register peer id
        {
            std::lock_guard<std::mutex> _lock(this->_peer_map_mutex);
            this->_peer_conn_obs_map.insert(std::pair<std::string, w_peer_conn_obs*>(
                p_peer_id, 
                _peer_conn_obs));
        }

        // set remote offer
        std::promise<const webrtc::SessionDescriptionInterface*> _remote_promise;
        rtc::scoped_refptr<w_set_session_desc_obs> _remote_session_obs(
            w_set_session_desc_obs::create(_peer_conn, _remote_promise));

        _peer_conn->SetRemoteDescription(_remote_session_obs.get(), p_session_desc);
        // waiting for remote description
        auto _remote_future = _remote_promise.get_future();
        if (_remote_future.wait_for(std::chrono::milliseconds(5000)) == std::future_status::ready)
        {

        }
        else
        {
            _remote_session_obs->cancel();
        }

        // add local stream
        if (!add_streams(_peer_conn.get(), p_video_url, p_audio_url, p_options))
        {
            // RTC_LOG(LS_WARNING) << "Can't add stream";
        }

        // create answer
        webrtc::PeerConnectionInterface::RTCOfferAnswerOptions _rtc_options;
        std::promise<const webrtc::SessionDescriptionInterface*> _local_promise;

        rtc::scoped_refptr<w_create_session_desc_obs> _local_session_obs(
            w_create_session_desc_obs::create(_peer_conn, _local_promise));
        _peer_conn->CreateAnswer(_local_session_obs.get(), _rtc_options);

        // waiting for answer
        auto _local_future = _local_promise.get_future();
        if (_local_future.wait_for(std::chrono::milliseconds(5000)) == std::future_status::ready)
        {
            // answer with the created answer
            const auto* _desc = _local_future.get();
            if (_desc)
            {
                answer = _desc->Clone();
            }
            else
            {
                //LOG 
            }
        }
        else
        {
            _local_session_obs->cancel();
        }
    }
    return answer;
}

std::string w_peer_conn_manager::get_oldest_peer_conn()
{
    std::string _oldest_peer_id;

    uint64_t _oldest_pc = std::numeric_limits<uint64_t>::max();
    std::lock_guard<std::mutex> _peer_lock(this->_peer_map_mutex);

    if ((this->_max_pc > 0) &&
        (this->_peer_conn_obs_map.size() >= this->_max_pc))
    {
        for (auto _it : this->_peer_conn_obs_map)
        {
            auto _creation_time = _it.second->get_creation_time();
            if (_creation_time < _oldest_pc)
            {
                _oldest_pc = _creation_time;
                _oldest_peer_id = _it.second->get_peer_id();
            }
        }
    }
    return _oldest_peer_id;
}

bool w_peer_conn_manager::get_is_initialized() const
{
    return this->_peer_conn_factory.get() != nullptr;
}

//const std::map<std::string, w_http_function> w_peer_conn_manager::get_http_api() const
//{
//    return this->_funcs;
//}

const rapidjson::Document w_peer_conn_manager::get_ice_candidate_list(const std::string& p_peer_id)
{
    rapidjson::Document _value = {};
    std::lock_guard<std::mutex> _peer_lock(this->_peer_map_mutex);
    auto _it = this->_peer_conn_obs_map.find(p_peer_id);
    if (_it != this->_peer_conn_obs_map.end())
    {
        auto* _obs = _it->second;
        if (_obs)
        {
            _value = std::move(_obs->get_ice_candidate_list());
        }
        else
        {
        }
    }
    return _value;
}

const rapidjson::Document w_peer_conn_manager::get_video_device_list()
{
    rapidjson::Document _value(rapidjson::kArrayType);

    const auto _video_capture_devices = webRTC::w_capturer_factory::get_video_capture_device_list(
        this->_publish_filter,
        this->_use_null_codec);
    for (auto video_device : _video_capture_devices)
    {
       // _value.PushBack(video_device, _value.GetAllocator());
    }

    return _value;
}

const rapidjson::Document w_peer_conn_manager::get_audio_device_list()
{
    rapidjson::Document _value(rapidjson::kArrayType);

    const auto _audio_capture_devices = webRTC::w_capturer_factory::get_audio_capture_device_list(
        this->_publish_filter,
        this->_audio_device_module);
    for (auto audio_device : _audio_capture_devices)
    {
        //_value.PushBack(audio_device, _value.GetAllocator());
    }

    return _value;
}

const rapidjson::Document w_peer_conn_manager::get_media_list()
{
    rapidjson::Document _value(rapidjson::kArrayType);

    const std::list<std::string> _video_capture_devices = webRTC::w_capturer_factory::get_video_capture_device_list(
        this->_publish_filter,
        this->_use_null_codec);

    for (auto video_device : _video_capture_devices)
    {
        rapidjson::Value _media = {};

        _media.SetObject();
        _media["video"].SetString(video_device.c_str(), video_device.length());

        auto it = this->_video_audio_map.find(video_device);
        if (it != this->_video_audio_map.end())
        {
            _media["audio"].SetString(it->second.c_str(), it->second.length());
        }
        _value.PushBack(_media, _value.GetAllocator());
    }

    const auto video_list = webRTC::w_capturer_factory::get_video_source_list(
        this->_publish_filter,
        this->_use_null_codec);
    for (auto video_source : video_list)
    {
        rapidjson::Value _media = {};
        _media["video"].SetString(video_source.c_str(), video_source.length());
        _value.PushBack(_media, _value.GetAllocator());
    }

    //for (auto _media = this->_config.Begin(); _media != this->_config.End(); _media++)
    //{
        //std::string name = _media->GetString();
        //if (_media->HasMember("video"))
        //{
        //    (*_media)["video"] = name;
        //}
        //if (_media->HasMember("audio"))
        //{
        //    _media["audio"] = name;
        //}
        //_value.PushBack(*_media, _value.GetAllocator());
    //}
    return _value;
}

// std::pair<std::map<std::string, std::string>, Json::Value> PeerConnectionManager::whip(const struct mg_request_info *req_info, const Json::Value &in)
// {
//     std::string peerid;
//     std::string videourl;
//     std::string audiourl;
//     std::string options;
//     if (req_info->query_string)
//     {
//         CivetServer::getParam(req_info->query_string, "peerid", peerid);
//         CivetServer::getParam(req_info->query_string, "url", videourl);
//         CivetServer::getParam(req_info->query_string, "audiourl", audiourl);
//         CivetServer::getParam(req_info->query_string, "options", options);
//     }

//     std::map<std::string, std::string> headers;
//     std::string answersdp;
//     if (strcmp(req_info->request_method, "DELETE") == 0)
//     {
//         this->hangUp(peerid);
//     }
//     else if (strcmp(req_info->request_method, "PATCH") == 0)
//     {
//     }
//     else
//     {
//         std::string offersdp(in.asString());
//         RTC_LOG(LS_ERROR) << "offer:" << offersdp;
//         webrtc::SessionDescriptionInterface *session_description(webrtc::CreateSessionDescription(webrtc::SessionDescriptionInterface::kOffer, offersdp, NULL));
//         if (!session_description)
//         {
//             RTC_LOG(LS_WARNING) << "Can't parse received session description message.";
//         }
//         else
//         {
//             std::unique_ptr<webrtc::SessionDescriptionInterface> desc = this->getAnswer(peerid, session_description, videourl, audiourl, options);
//             if (desc.get())
//             {
//                 desc->ToString(&answersdp);
//             }
//             else
//             {
//                 RTC_LOG(LS_ERROR) << "Failed to create answer - no SDP";
//             }
//         }
//         RTC_LOG(LS_ERROR) << "anwser:" << answersdp;
//         headers["location"] = req_info->request_uri;
//     }
//     return std::make_pair(headers, answersdp);
// }

 //const rapidjson::Document w_peer_conn_manager::get_ice_servers(const std::string &clientIp)
 //{
 //    rapidjson::Value _urls(rapidjson::kArrayType);

 //    for (auto ice_server : this->_ice_server_list)
 //    {
 //        Json::Value server;
 //        Json::Value urlList(Json::arrayValue);
 //        IceServer srv = getIceServerFromUrl(iceServer, clientIp);
 //        RTC_LOG(LS_INFO) << "ICE URL:" << srv.url;
 //        urlList.append(srv.url);
 //        server["urls"] = urlList;
 //        if (srv.user.length() > 0)
 //            server["username"] = srv.user;
 //        if (srv.pass.length() > 0)
 //            server["credential"] = srv.pass;
 //        urls.append(server);
 //    }

 //    Json::Value iceServers;
 //    iceServers["iceServers"] = urls;

 //    return iceServers;
 //}

// /* ---------------------------------------------------------------------------
// ** create an offer for a call
// ** -------------------------------------------------------------------------*/
// const Json::Value PeerConnectionManager::createOffer(const std::string &peerid, const std::string &videourl, const std::string &audiourl, const std::string &options)
// {
//     RTC_LOG(LS_INFO) << __FUNCTION__ << " video:" << videourl << " audio:" << audiourl << " options:" << options;
//     Json::Value offer;

//     PeerConnectionObserver *peerConnectionObserver = this->CreatePeerConnection(peerid);
//     if (!peerConnectionObserver)
//     {
//         RTC_LOG(LS_ERROR) << "Failed to initialize PeerConnection";
//     }
//     else
//     {
//         rtc::scoped_refptr<webrtc::PeerConnectionInterface> peerConnection = peerConnectionObserver->getPeerConnection();

//         if (!this->AddStreams(peerConnection.get(), videourl, audiourl, options))
//         {
//             RTC_LOG(LS_WARNING) << "Can't add stream";
//         }

//         // register peerid
//         {
//             std::lock_guard<std::mutex> peerlock(m_peerMapMutex);
//             m_peer_connectionobs_map.insert(std::pair<std::string, PeerConnectionObserver *>(peerid, peerConnectionObserver));
//         }

//         // ask to create offer
//         webrtc::PeerConnectionInterface::RTCOfferAnswerOptions rtcoptions;
//         rtcoptions.offer_to_receive_video = 0;
//         rtcoptions.offer_to_receive_audio = 0;
//         std::promise<const webrtc::SessionDescriptionInterface *> localpromise;
//         rtc::scoped_refptr<CreateSessionDescriptionObserver> localSessionObserver(CreateSessionDescriptionObserver::Create(peerConnection, localpromise));
//         peerConnection->CreateOffer(localSessionObserver.get(), rtcoptions);

//         // waiting for offer
//         std::future<const webrtc::SessionDescriptionInterface *> future = localpromise.get_future();
//         if (future.wait_for(std::chrono::milliseconds(5000)) == std::future_status::ready)
//         {
//             // answer with the created offer
//             const webrtc::SessionDescriptionInterface *desc = future.get();
//             if (desc)
//             {
//                 std::string sdp;
//                 desc->ToString(&sdp);

//                 offer[s_session_desc_type_name] = desc->type();
//                 offer[s_session_desc_sdp_name] = sdp;
//             }
//             else
//             {
//                 RTC_LOG(LS_ERROR) << "Failed to create offer - no session";
//             }
//         }
//         else
//         {
//             localSessionObserver->cancel();
//             RTC_LOG(LS_ERROR) << "Failed to create offer - timeout";
//         }
//     }
//     return offer;
// }

// /* ---------------------------------------------------------------------------
// ** set answer to a call initiated by createOffer
// ** -------------------------------------------------------------------------*/
// const Json::Value PeerConnectionManager::setAnswer(const std::string &peerid, const Json::Value &jmessage)
// {
//     RTC_LOG(LS_INFO) << jmessage;
//     Json::Value answer;

//     std::string type;
//     std::string sdp;
//     if (!rtc::GetStringFromJsonObject(jmessage, s_session_desc_type_name, &type) || !rtc::GetStringFromJsonObject(jmessage, s_session_desc_sdp_name, &sdp))
//     {
//         RTC_LOG(LS_WARNING) << "Can't parse received message.";
//         answer["error"] = "Can't parse received message.";
//     }
//     else
//     {
//         webrtc::SessionDescriptionInterface *session_description(webrtc::CreateSessionDescription(type, sdp, NULL));
//         if (!session_description)
//         {
//             RTC_LOG(LS_WARNING) << "Can't parse received session description message.";
//             answer["error"] = "Can't parse received session description message.";
//         }
//         else
//         {
//             RTC_LOG(LS_ERROR) << "From peerid:" << peerid << " received session description :" << session_description->type();

//             std::lock_guard<std::mutex> peerlock(m_peerMapMutex);
//             rtc::scoped_refptr<webrtc::PeerConnectionInterface> peerConnection = this->getPeerConnection(peerid);
//             if (peerConnection)
//             {
//                 std::promise<const webrtc::SessionDescriptionInterface *> remotepromise;
//                 rtc::scoped_refptr<SetSessionDescriptionObserver> remoteSessionObserver(SetSessionDescriptionObserver::Create(peerConnection, remotepromise));
//                 peerConnection->SetRemoteDescription(remoteSessionObserver.get(), session_description);
//                 // waiting for remote description
//                 std::future<const webrtc::SessionDescriptionInterface *> remotefuture = remotepromise.get_future();
//                 if (remotefuture.wait_for(std::chrono::milliseconds(5000)) == std::future_status::ready)
//                 {
//                     RTC_LOG(LS_INFO) << "remote_description is ready";
//                     const webrtc::SessionDescriptionInterface *desc = remotefuture.get();
//                     if (desc)
//                     {
//                         std::string sdp;
//                         desc->ToString(&sdp);

//                         answer[s_session_desc_type_name] = desc->type();
//                         answer[s_session_desc_sdp_name] = sdp;
//                     }
//                     else
//                     {
//                         answer["error"] = "Can't get remote description.";
//                     }
//                 }
//                 else
//                 {
//                     remoteSessionObserver->cancel();
//                     RTC_LOG(LS_WARNING) << "Can't get remote description.";
//                     answer["error"] = "Can't get remote description.";
//                 }
//             }
//         }
//     }
//     return answer;
// }

// /* ---------------------------------------------------------------------------
// **  auto-answer to a call
// ** -------------------------------------------------------------------------*/

// /* ---------------------------------------------------------------------------
// **  get PeerConnection list
// ** -------------------------------------------------------------------------*/
// const Json::Value PeerConnectionManager::getPeerConnectionList()
// {
//     Json::Value value(Json::arrayValue);

//     std::lock_guard<std::mutex> peerlock(m_peerMapMutex);
//     for (auto it : m_peer_connectionobs_map)
//     {
//         Json::Value content;

//         // get local SDP
//         rtc::scoped_refptr<webrtc::PeerConnectionInterface> peerConnection = it.second->getPeerConnection();
//         if ((peerConnection) && (peerConnection->local_description()))
//         {
//             content["pc_state"] = (int)(peerConnection->peer_connection_state());
//             content["signaling_state"] = (int)(peerConnection->signaling_state());
//             content["ice_state"] = (int)(peerConnection->ice_connection_state());

//             std::string sdp;
//             peerConnection->local_description()->ToString(&sdp);
//             content["sdp"] = sdp;

//             Json::Value streams;
//             std::vector<rtc::scoped_refptr<webrtc::RtpSenderInterface>> localstreams = peerConnection->GetSenders();
//             for (auto localStream : localstreams)
//             {
//                 if (localStream != NULL)
//                 {
//                     rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> mediaTrack = localStream->track();
//                     if (mediaTrack)
//                     {
//                         Json::Value track;
//                         track["kind"] = mediaTrack->kind();
//                         if (track["kind"] == "video")
//                         {
//                             webrtc::VideoTrackInterface *videoTrack = (webrtc::VideoTrackInterface *)mediaTrack.get();
//                             webrtc::VideoTrackSourceInterface::Stats stats;
//                             if (videoTrack->GetSource())
//                             {
//                                 track["state"] = videoTrack->GetSource()->state();
//                                 if (videoTrack->GetSource()->GetStats(&stats))
//                                 {
//                                     track["width"] = stats.input_width;
//                                     track["height"] = stats.input_height;
//                                 }
//                             }
//                         }
//                         else if (track["kind"] == "audio")
//                         {
//                             webrtc::AudioTrackInterface *audioTrack = (webrtc::AudioTrackInterface *)mediaTrack.get();
//                             if (audioTrack->GetSource())
//                             {
//                                 track["state"] = audioTrack->GetSource()->state();
//                             }
//                             int level = 0;
//                             if (audioTrack->GetSignalLevel(&level))
//                             {
//                                 track["level"] = level;
//                             }
//                         }

//                         Json::Value tracks;
//                         tracks[mediaTrack->id()] = track;
//                         std::string streamLabel = localStream->stream_ids()[0];
//                         streams[streamLabel] = tracks;
//                     }
//                 }
//             }
//             content["streams"] = streams;
//         }

//         Json::Value pc;
//         pc[it.first] = content;
//         value.append(pc);
//     }
//     return value;
// }

// /* ---------------------------------------------------------------------------
// **  get StreamList list
// ** -------------------------------------------------------------------------*/
// const Json::Value PeerConnectionManager::getStreamList()
// {
//     std::lock_guard<std::mutex> mlock(m_streamMapMutex);
//     Json::Value value(Json::arrayValue);
//     for (auto it : m_stream_map)
//     {
//         value.append(it.first);
//     }
//     return value;
// }
