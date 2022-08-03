#include "w_capturer_factory.hpp"
#include "w_track_source.hpp"
#include "w_vcm_capturer.hpp"
#include "wolf.hpp"
#include <modules/video_capture/video_capture_factory.h>

using w_capturer_factory = wolf::stream::webRTC::w_capturer_factory;
using w_vcm_capturer = wolf::stream::webRTC::w_vcm_capturer;

const std::list<std::string> w_capturer_factory::get_video_capture_device_list(
    const std::regex &p_publish_filter,
    bool p_use_null_codec)
{
    std::list<std::string> _video_device_list;

    if (std::regex_match("videocap://", p_publish_filter) && !p_use_null_codec)
    {
        std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> _info(webrtc::VideoCaptureFactory::CreateDeviceInfo());
        if (_info != nullptr)
        {
            auto _num_video_devices = _info->NumberOfDevices();
            for (uint32_t i = 0; i < _num_video_devices; ++i)
            {
                std::array<char, W_MAX_PATH> _name = {0};
                std::array<char, W_MAX_PATH> _id = {0};

                if (_info->GetDeviceName(
                        i,
                        _name.data(),
                        W_MAX_PATH,
                        _id.data(),
                        W_MAX_PATH) != -1)
                {
                    std::string _dev_name;
                    auto _it = std::find(_video_device_list.begin(),
                                         _video_device_list.end(),
                                         _name.data());
                    if (_it == _video_device_list.end())
                    {
                        _dev_name = std::string(_name.data());
                    }
                    else
                    {
                        _dev_name = "videocap://";
                        _dev_name += std::to_string(i);
                    }
                    _video_device_list.push_back(_dev_name);
                }
            }
        }
    }
    if (std::regex_match("v4l2://", p_publish_filter) && p_use_null_codec)
    {
#ifdef HAVE_V4L2
        DIR *dir = opendir("/dev");
        if (dir != nullptr)
        {
            struct dirent *entry = NULL;
            while ((entry = readdir(dir)) != NULL)
            {
                if (strncmp(entry->d_name, "video", 5) == 0)
                {
                    std::string device("/dev/");
                    device.append(entry->d_name);
                    V4L2DeviceParameters param(device.c_str(), V4L2_PIX_FMT_H264, 0, 0, 0);
                    V4l2Capture *capture = V4l2Capture::create(param);
                    if (capture != NULL)
                    {
                        delete capture;
                        std::string v4l2url("v4l2://");
                        v4l2url.append(device);
                        videoDeviceList.push_back(v4l2url);
                    }
                }
            }
            closedir(dir);
        }
#endif
    }

    return _video_device_list;
}

const std::list<std::string> w_capturer_factory::get_video_source_list(
    const std::regex &p_publish_filter,
    bool p_use_null_codec)
{
    std::list<std::string> _video_list;

#ifdef USE_X11
    if (std::regex_match("window://", publishFilter) && !useNullCodec)
    {
        std::unique_ptr<webrtc::DesktopCapturer> capturer = webrtc::DesktopCapturer::CreateWindowCapturer(webrtc::DesktopCaptureOptions::CreateDefault());
        if (capturer)
        {
            webrtc::DesktopCapturer::SourceList sourceList;
            if (capturer->GetSourceList(&sourceList))
            {
                for (auto source : sourceList)
                {
                    std::ostringstream os;
                    os << "window://" << source.title;
                    videoList.push_back(os.str());
                }
            }
        }
    }
    if (std::regex_match("screen://", publishFilter) && !useNullCodec)
    {
        std::unique_ptr<webrtc::DesktopCapturer> capturer = webrtc::DesktopCapturer::CreateScreenCapturer(webrtc::DesktopCaptureOptions::CreateDefault());
        if (capturer)
        {
            webrtc::DesktopCapturer::SourceList sourceList;
            if (capturer->GetSourceList(&sourceList))
            {
                for (auto source : sourceList)
                {
                    std::ostringstream os;
                    os << "screen://" << source.id;
                    videoList.push_back(os.str());
                }
            }
        }
    }
#endif
    return _video_list;
}

rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> w_capturer_factory::create_video_source(
    const std::string &p_video_url,
    const std::map<std::string, std::string> &p_opts,
    const std::regex &p_publish_filter,
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> p_peer_connection_factory)
{
    rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> _video_source;

    if (((p_video_url.find("rtsp://") == 0) || (p_video_url.find("rtsps://") == 0)) &&
        (std::regex_match("rtsp://", p_publish_filter)))
    {
#ifdef HAVE_LIVE555
        _video_source = w_track_source<RTSPVideoCapturer>::create(p_video_url, opts, p_video_decoder_factory);
#endif
    }
    else if ((p_video_url.find("file://") == 0) && (std::regex_match("file://", p_publish_filter)))
    {
#ifdef HAVE_LIVE555
        _video_source = w_track_source<FileVideoCapturer>::create(p_video_url, opts, p_video_decoder_factory);
#endif
    }
    else if ((p_video_url.find("rtp://") == 0) && (std::regex_match("rtp://", p_publish_filter)))
    {
#ifdef HAVE_LIVE555
        _video_source = w_track_source<RTPVideoCapturer>::create(SDPClient::getSdpFromRtpUrl(p_video_url), opts, p_video_decoder_factory);
#endif
    }
    else if ((p_video_url.find("screen://") == 0) && (std::regex_match("screen://", p_publish_filter)))
    {
#ifdef USE_X11
        _video_source = w_track_source<ScreenCapturer>::create(p_video_url, opts, p_video_decoder_factory);
#endif
    }
    else if ((p_video_url.find("window://") == 0) && (std::regex_match("window://", p_publish_filter)))
    {
#ifdef USE_X11
        _video_source = w_track_source<WindowCapturer>::create(p_video_url, opts, p_video_decoder_factory);
#endif
    }
    else if ((p_video_url.find("rtmp://") == 0) && (std::regex_match("rtmp://", p_publish_filter)))
    {
#ifdef HAVE_RTMP
        _video_source = w_track_source<RtmpVideoSource>::create(p_video_url, opts, p_video_decoder_factory);
#endif
    }
    else if ((p_video_url.find("v4l2://") == 0) && (std::regex_match("v4l2://", p_publish_filter)))
    {
#ifdef HAVE_V4L2
        _video_source = w_track_source<V4l2Capturer>::create(p_video_url, opts, p_video_decoder_factory);
#endif
    }
    else if (std::regex_match("videocap://", p_publish_filter))
    {
        _video_source = wolf::stream::webRTC::w_track_source<w_vcm_capturer>::create(
            p_video_url,
            p_opts);
    }

    return _video_source;
}

rtc::scoped_refptr<webrtc::AudioSourceInterface> w_capturer_factory::create_audio_source(
    const std::string &p_audio_url,
    const std::map<std::string, std::string> &p_opts,
    const std::regex &p_publish_filter,
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> p_peer_connection_factory,
    rtc::scoped_refptr<webrtc::AudioDecoderFactory> p_audio_decoderf_factory,
    rtc::scoped_refptr<webrtc::AudioDeviceModule> p_audio_device_module)
{
    rtc::scoped_refptr<webrtc::AudioSourceInterface> _audio_source;

    if ((p_audio_url.find("rtsp://") == 0) && (std::regex_match("rtsp://", p_publish_filter)))
    {
#ifdef HAVE_LIVE555
        p_audio_device_module->Terminate();
        audioSource = RTSPAudioSource::Create(audioDecoderfactory, audiourl, opts);
#endif
    }
    else if ((p_audio_url.find("file://") == 0) && (std::regex_match("file://", p_publish_filter)))
    {
#ifdef HAVE_LIVE555
        p_audio_device_module->Terminate();
        audioSource = FileAudioSource::Create(audioDecoderfactory, audiourl, opts);
#endif
    }
    else if (std::regex_match("audiocap://", p_publish_filter))
    {
        p_audio_device_module->Init();

        const auto _num_audio_devices = p_audio_device_module->RecordingDevices();
        auto _idx_audio_device = -1;

        std::array<char, W_MAX_PATH> _name = {0};
        std::array<char, W_MAX_PATH> _id = {0};

        if (p_audio_url.find("audiocap://") == 0)
        {
            int _device_number = atoi(p_audio_url.substr(strlen("audiocap://")).c_str());

            if (p_audio_device_module->RecordingDeviceName(_device_number, _name.data(), _id.data()) != -1)
            {
                _idx_audio_device = _device_number;
            }
        }
        else
        {
            for (auto i = 0; i < _num_audio_devices; ++i)
            {
                if (p_audio_device_module->RecordingDeviceName(i, _name.data(), _id.data()) != -1)
                {
                    if (strcmp(p_audio_url.data(), _name.data()) == 0)
                    {
                        _idx_audio_device = i;
                        break;
                    }
                }
            }
        }
        if ((_idx_audio_device >= 0) && (_idx_audio_device < _num_audio_devices))
        {
            p_audio_device_module->SetRecordingDevice(_idx_audio_device);
            cricket::AudioOptions opt;
            _audio_source = p_peer_connection_factory->CreateAudioSource(opt);
        }
    }
    return _audio_source;
}

const std::list<std::string> w_capturer_factory::get_audio_capture_device_list(
    const std::regex &p_publish_filter,
    rtc::scoped_refptr<webrtc::AudioDeviceModule> p_audio_device_module)
{
    std::list<std::string> _audio_list;
    if (std::regex_match("audiocap://", p_publish_filter))
    {
        const auto _num_audio_devices = p_audio_device_module->RecordingDevices();
        for (auto i = 0; i < _num_audio_devices; ++i)
        {
            std::array<char, W_MAX_PATH> _name = {0};
            std::array<char, W_MAX_PATH> _id = {0};
            if (p_audio_device_module->RecordingDeviceName(i, _name.data(), _id.data()) != -1)
            {
                // RTC_LOG(LS_INFO) << "audio device name:" << name << " id:" << id;
                std::string _dev_name;
                auto it = std::find(_audio_list.begin(), _audio_list.end(), _name.data());
                if (it == _audio_list.end())
                {
                    _dev_name = std::string(_name.data());
                }
                else
                {
                    _dev_name = "audiocap://";
                    _dev_name += std::to_string(i);
                }
                _audio_list.push_back(_dev_name);
            }
        }
    }
    return _audio_list;
}