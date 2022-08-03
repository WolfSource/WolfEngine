#include "w_peer_conn_manager_helpers.hpp"

#include "DISABLE_ANALYSIS_BEGIN"

#include <api/audio_codecs/builtin_audio_encoder_factory.h>
#include <api/create_peerconnection_factory.h>
#include <api/rtc_event_log/rtc_event_log_factory.h>
#include <api/task_queue/default_task_queue_factory.h>
#include <api/video_codecs/builtin_video_decoder_factory.h>
#include <api/video_codecs/builtin_video_encoder_factory.h>
#include <media/engine/webrtc_media_engine.h>
#include <stream/webrtc/media/w_video_decoder_factory_null_codec.hpp>
#include <stream/webrtc/media/w_video_encoder_factory_null_codec.hpp>

#include "DISABLE_ANALYSIS_END"

#ifdef WIN64

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#include <WinSock2.h>

#else

#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>

#endif

using w_ice_server = wolf::stream::webRTC::w_ice_server;
using w_peer_conn_manager_helpers = wolf::stream::webRTC::w_peer_conn_manager_helpers;
using w_video_encoder_factory_null_codec = wolf::stream::webRTC::w_video_encoder_factory_null_codec;
using w_video_decoder_factory_null_codec = wolf::stream::webRTC::w_video_decoder_factory_null_codec;

std::string w_peer_conn_manager_helpers::get_server_ip_from_client_ip(int p_client_ip)
{
#if defined(WIN64) || defined(__APPLE__)
    W_UNUSED(p_client_ip);
    return "127.0.0.1";
#else
    std::string server_address;
    char host[NI_MAXHOST];
    ifaddrs *ifaddr = nullptr;
    if (getifaddrs(&ifaddr) == 0)
    {
        for (ifaddrs *ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next)
        {
            if ((ifa->ifa_netmask != nullptr) &&
                (ifa->ifa_netmask->sa_family == AF_INET) &&
                (ifa->ifa_addr != nullptr) &&
                (ifa->ifa_addr->sa_family == AF_INET))
            {
                sockaddr_in *addr = (sockaddr_in *)ifa->ifa_addr;
                sockaddr_in *mask = (sockaddr_in *)ifa->ifa_netmask;
                if ((addr->sin_addr.s_addr & mask->sin_addr.s_addr) == (p_client_ip & mask->sin_addr.s_addr))
                {
                    if (getnameinfo(ifa->ifa_addr,
                                    sizeof(sockaddr_in),
                                    host,
                                    sizeof(host),
                                    nullptr,
                                    0,
                                    NI_NUMERICHOST) == 0)
                    {
                        server_address = host;
                        break;
                    }
                }
            }
        }
    }
    freeifaddrs(ifaddr);
    return server_address;
#endif
}

w_ice_server w_peer_conn_manager_helpers::get_ice_server_from_url(
    const std::string &p_url,
    const std::string &p_client_ip)
{
    w_ice_server _srv;
    _srv.url = p_url;

    auto _pos = p_url.find_first_of(':');
    if (_pos != std::string::npos)
    {
        std::string _protocol = p_url.substr(0, _pos);
        std::string _uri = p_url.substr(_pos + 1);
        std::string _credentials;

        _pos = _uri.rfind('@');
        if (_pos != std::string::npos)
        {
            _credentials = _uri.substr(0, _pos);
            _uri = _uri.substr(_pos + 1);
        }

        if ((_uri.find("0.0.0.0:") == 0) && (p_client_ip.empty() == false))
        {
            // answer with ip that is on same network as client
            std::string _client_url = get_server_ip_from_client_ip(
                inet_addr(p_client_ip.c_str()));
            _client_url += _uri.substr(_uri.find_first_of(':'));
            _uri = _client_url;
        }
        _srv.url = _protocol + ":" + _uri;

        if (!_credentials.empty())
        {
            _pos = _credentials.find(':');
            if (_pos == std::string::npos)
            {
                _srv.user = _credentials;
            }
            else
            {
                _srv.user = _credentials.substr(0, _pos);
                _srv.pass = _credentials.substr(_pos + 1);
            }
        }
    }

    return _srv;
}

webrtc::PeerConnectionFactoryDependencies w_peer_conn_manager_helpers::create_peer_conn_factory_deps(
    rtc::Thread *p_signaling_thread,
    rtc::Thread *p_worker_thread,
    rtc::scoped_refptr<webrtc::AudioDeviceModule> p_audio_device_module,
    rtc::scoped_refptr<webrtc::AudioDecoderFactory> p_audio_decoder_factory,
    bool p_use_null_codec)
{
    webrtc::PeerConnectionFactoryDependencies _peer_factory_deps;
    _peer_factory_deps.network_thread = nullptr;
    _peer_factory_deps.worker_thread = p_worker_thread;
    _peer_factory_deps.signaling_thread = p_signaling_thread;
    _peer_factory_deps.call_factory = webrtc::CreateCallFactory();
    _peer_factory_deps.task_queue_factory = webrtc::CreateDefaultTaskQueueFactory();
    _peer_factory_deps.event_log_factory = absl::make_unique<webrtc::RtcEventLogFactory>(_peer_factory_deps.task_queue_factory.get());

    cricket::MediaEngineDependencies _media_dependencies;
    _media_dependencies.task_queue_factory = _peer_factory_deps.task_queue_factory.get();
    _media_dependencies.adm = std::move(p_audio_device_module);
    _media_dependencies.audio_encoder_factory = webrtc::CreateBuiltinAudioEncoderFactory();
    _media_dependencies.audio_decoder_factory = std::move(p_audio_decoder_factory);
    _media_dependencies.audio_processing = webrtc::AudioProcessingBuilder().Create();

    _media_dependencies.video_encoder_factory = create_encoder_factory(p_use_null_codec);
    _media_dependencies.video_decoder_factory = create_decoder_factory(p_use_null_codec);

    _peer_factory_deps.media_engine = cricket::CreateMediaEngine(std::move(_media_dependencies));

    return _peer_factory_deps;
}

std::unique_ptr<webrtc::VideoEncoderFactory> w_peer_conn_manager_helpers::create_encoder_factory(bool p_use_null_codec)
{
    return p_use_null_codec ? std::make_unique<w_video_encoder_factory_null_codec>() : webrtc::CreateBuiltinVideoEncoderFactory();
}

std::unique_ptr<webrtc::VideoDecoderFactory> w_peer_conn_manager_helpers::create_decoder_factory(bool p_use_null_codec)
{
    return p_use_null_codec ? std::make_unique<w_video_decoder_factory_null_codec>() : webrtc::CreateBuiltinVideoDecoderFactory();
}