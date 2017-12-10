#include "w_media_core_pch.h"
#include "w_media_core.h"
#include <io.h>
#include <wrl/client.h>
#include <iomanip>      // For std::hex
#include <codecvt>      // For Unicode conversions

using namespace wolf;
using namespace wolf::system;
using namespace wolf::framework;

static const MFTIME MF_ONE_SEC = 10000000;
static const LONG MF_ONE_MSEC = 1000;
static std::once_flag onceInit;
static std::once_flag onceRelease;

namespace wolf
{
    namespace framework
    {
        struct w_video_frame_down_sample_data
        {
            std::array<int, VIDEO_FRAME_DOWN_SAMPLE_SIZE> data;
        };
        struct w_audio_frame_data
        {
            uint8_t             data[AUDIO_FRAME_SIZE];
            uint32_t            usedSize;
            int64_t             pts;
        };
        struct w_codec
        {
            AVStream*			avStream = nullptr;
            AVCodecContext*		avCodecCtx = nullptr;
            AVCodec*			avCodec = nullptr;
            AVFrame*			avFrame = nullptr;
            double				pts = 0;
            double				clock = 0;
        };

        class w_media_core_pimp
        {
        public:
            w_media_core_pimp() :
                _name("w_media_core"),
                _is_media_open(false),
                _is_released(false),
                _av_format_ctx(nullptr),
                _av_packet(nullptr),
                _audio_convert(nullptr)
            {
                this->_name = "w_ffmpeg";

                std::memset(&this->_video_codec, 0, sizeof(w_codec));
                std::memset(&this->_audio_codec, 0, sizeof(w_codec));

#if defined(__1080p_50__)
                this->frameRate = 50;
#elif defined(__1080p_25__) || defined(__PAL__)
                this->frameRate = 25;
#elif defined(__NTSC__)
                this->frameRate = 29.97f;
#endif
            }

            ~w_media_core_pimp()
            {
                release();
            }

            HRESULT open_media(std::wstring pMediaPath, 
                int64_t pSeekToFrame, 
                bool pLog4User)
            {
                if (pMediaPath.empty()) return S_FALSE;

                release();

                this->_is_released = false;

                std::memset(&this->_video_codec, 0, sizeof(w_codec));
                std::memset(&this->_audio_codec, 0, sizeof(w_codec));

                //Initialize the packet
                this->_av_packet = new AVPacket();
                av_init_packet(this->_av_packet);

                this->_audio_convert = swr_alloc();
                this->_av_format_ctx = avformat_alloc_context();

                this->_is_media_open = false;
                {
                    this->_full_path = wolf::system::convert::to_utf8(pMediaPath).c_str();

                    //Output the message
                    std::wstring msg = L"Media from following path \"" + pMediaPath + L"\" is going to decode";
                    //OutputDebugString(msg.c_str());
                    if (pLog4User)
                    {
                        logger.user(msg);
                    }
                    msg.clear();

                    HRESULT hr = S_OK;
                    // Open video file
                    bool containVideo = true, containAudio = true;


                    //Open file
                    if (avformat_open_input(&_av_format_ctx, this->_full_path.c_str(), NULL, NULL) != 0)
                    {
                        hr = S_FALSE;
                        V(hr, L"openning file", this->_name);
                        release();
                        return hr;
                    }

                    // Retrieve stream information
                    if (avformat_find_stream_info(_av_format_ctx, NULL) < 0)
                    {
                        hr = S_FALSE;
                        V(hr, L"finding file stream information", this->_name);
                        release();
                        return hr;
                    }



#ifdef _DEBUG
                    // Dump information about file onto standard error
                    //	av_dump_format(avFormatCtx, 0, path, 0);
#endif

                    // Find the first video & audio stream
                    this->_video_stream_index = av_find_best_stream(_av_format_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
                    this->_audio_stream_index = av_find_best_stream(_av_format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);

                    auto hasAudioStream = true;
                    auto hasVideoStream = true;

                    if (this->_video_stream_index <= -1)
                    {
                        hr = S_FALSE;
                        hasVideoStream = false;
                        if (pLog4User)
                        {
                            V(hr, L"finding stream of video", this->_name);
                            logger.user(L"Media file does not have video frames");
                        }
                    }
                    if (this->_audio_stream_index <= -1)
                    {
                        hr = S_FALSE;
                        hasAudioStream = false;
                        if (pLog4User)
                        {
                            V(hr, L"finding stream of audio", this->_name);
                            logger.user(L"Media file does not have audio frames");
                        }
                    }

                    if (!hasVideoStream && !hasAudioStream)
                    {
                        logger.user("File " + this->_full_path + " did not load");
                        release();
                        return S_FALSE;
                    }

                    //Get a pointer to the stream of video & audio 
                    //Get a pointer to the codec context of video & audio 
                    if (hasVideoStream)
                    {
                        _video_codec.avStream = _av_format_ctx->streams[this->_video_stream_index];
                        _video_codec.avCodecCtx = _av_format_ctx->streams[this->_video_stream_index]->codec;
                    }

                    if (hasAudioStream)
                    {
                        this->_audio_codec.avStream = _av_format_ctx->streams[this->_audio_stream_index];
                        this->_audio_codec.avCodecCtx = _av_format_ctx->streams[this->_audio_stream_index]->codec;
                    }

                    //Calculate the time and the frame per seconds

                    //Bugs with ffmpeg on duration of mpg files, so I used media source resolver from Mf.lib of windows
                    /*Log(QString::number(videoCodec.avStream->duration));
                    Log(QString::number(ticksPerFrame));
                    Log(QString::number(avr.den));
                    auto durationSeconds = static_cast<double>(videoCodec.avStream->duration) * static_cast<double>(ticksPerFrame) / static_cast<double>(avr.den);
                    */


#pragma region Get the information of media from Windows

                    MF_OBJECT_TYPE objectType = MF_OBJECT_INVALID;
                    Microsoft::WRL::ComPtr<IUnknown> pSource = nullptr;
                    Microsoft::WRL::ComPtr<IMFSourceResolver> sourceResolver = NULL;
                    Microsoft::WRL::ComPtr<IMFMediaSource> source;

                    hr = MFCreateSourceResolver(&sourceResolver);
                    if (FAILED(hr))
                    {
                        logger.write(L"creating source resolver");
                    }
                    hr = sourceResolver->CreateObjectFromURL(
                        pMediaPath.c_str(),
                        MF_RESOLUTION_MEDIASOURCE,
                        NULL,
                        &objectType,
                        &pSource);
                    if (FAILED(hr) || objectType == MF_OBJECT_TYPE::MF_OBJECT_INVALID || pSource == nullptr)
                    {
                        logger.error(L"Invalid object type, creating object from source for source resolver");
                        release();
                        return S_FALSE;
                    }

                    hr = pSource.Get()->QueryInterface(IID_PPV_ARGS(&source));
                    if (FAILED(hr))
                    {
                        logger.write(L"Query interface for media source");
                    }

                    Microsoft::WRL::ComPtr<IMFPresentationDescriptor> pPD = nullptr;
                    hr = source->CreatePresentationDescriptor(&pPD);
                    if (FAILED(hr))
                    {
                        logger.write(L"creating presentation descriptor");
                    }
                    auto pDuration = new MFTIME(0);
                    pPD->GetUINT64(MF_PD_DURATION, (UINT64*)pDuration);
                    auto duration = (LONG)(*pDuration / (MF_ONE_SEC / MF_ONE_MSEC));
                    pDuration = nullptr;

                    //Release 
                    COMPTR_RELEASE(pPD);
                    COMPTR_RELEASE(pSource);
                    COMPTR_RELEASE(sourceResolver);

#pragma endregion

                    this->_duration_time = w_time_span::from_milliseconds(duration);
                    this->_remained_time = this->_duration_time;
                    this->_elapsed_time = w_time_span::zero();

                    if (hasVideoStream)
                    {
                        AVRational avr = _video_codec.avCodecCtx->time_base;
                        auto ticksPerFrame = _video_codec.avCodecCtx->ticks_per_frame;
                        this->_frame_rate = static_cast<float>(avr.den) / static_cast<float>(avr.num * _video_codec.avCodecCtx->ticks_per_frame);
                    }
                    else if (hasAudioStream)
                    {
                        AVRational avr = _audio_codec.avCodecCtx->time_base;
                        auto ticksPerFrame = _audio_codec.avCodecCtx->ticks_per_frame;
                        this->_frame_rate = static_cast<float>(avr.den) / static_cast<float>(avr.num * _audio_codec.avCodecCtx->ticks_per_frame);
                    }

                    //Set total frames
                    this->_duration_video_frames = static_cast<byte>(std::round(((double)this->_duration_time.get_milliseconds() * this->_frame_rate) / 1000.0));
                    this->_remained_video_frames = this->_duration_video_frames;
                    this->_elapsed_video_frames = 0;

                    this->_total_video_frames = static_cast<size_t>(((double)this->_duration_time.get_total_milliseconds() * this->_frame_rate) / 1000.0);
                    //Find the decoder for the video and audio and also allocate frames for video & audio

                    if (hasVideoStream)
                    {
                        _video_codec.avCodec = avcodec_find_decoder(_video_codec.avCodecCtx->codec_id);
                        if (_video_codec.avCodec == NULL)
                        {
                            hr = S_FALSE;
                            V(hr, L"unsupported codec for video", this->_name);
                            release();
                            return hr;
                        }

                        //Open codec
                        if (avcodec_open2(_video_codec.avCodecCtx, _video_codec.avCodec, NULL) < 0)
                        {
                            hr = S_FALSE;
                            V(hr, L"could not open video codec", this->_name);
                            release();
                            return hr;
                        }

                        _video_codec.avFrame = av_frame_alloc();//avcodec_alloc_frame deprecated function

                        auto videoBitRater = get_bit_rate();
                        auto videoChannels = get_video_channels();
                        auto videoSampleRate = get_video_sample_rate();

                        auto width = get_video_frame_width();
                        auto height = get_video_frame_height();

                        //Create destination avFrame
                        this->_av_Frame.reset(av_frame_alloc());
                        av_frame_unref(this->_av_Frame.get()); // avcodec_get_frame_defaults deprecated function

                                                               //This time we need BGRA for directX
                        avpicture_alloc(
                            (AVPicture*) this->_av_Frame.get(),
                            AV_PIX_FMT_BGRA,
                            width,
                            height);

                        this->_down_sample_avFrame.reset(av_frame_alloc());
                        av_frame_unref(this->_down_sample_avFrame.get()); // avcodec_get_frame_defaults deprecated function

                                                                          //This time we need BGRA for directX
                        avpicture_alloc(
                            (AVPicture*) this->_down_sample_avFrame.get(),
                            AV_PIX_FMT_BGRA,
                            width / 2,
                            height / 2);
                    }


                    if (hasAudioStream)
                    {
                        _audio_codec.avCodec = avcodec_find_decoder(_audio_codec.avCodecCtx->codec_id);
                        if (_audio_codec.avCodec == NULL)
                        {
                            hr = S_FALSE;
                            V(hr, L"unsupported codec for audio", this->_name);
                            release();
                            return hr;
                        }

                        if (avcodec_open2(_audio_codec.avCodecCtx, _audio_codec.avCodec, NULL) < 0)
                        {
                            hr = S_FALSE;
                            V(hr, L"could not open audio codec", this->_name);
                            release();
                            return hr;
                        }
                        _audio_codec.avFrame = av_frame_alloc();//avcodec_alloc_frame deprecated function

                        auto audioChannels = get_audio_channels();
                        auto audioSampleRate = get_audio_sample_rate();

                        AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
                        uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO;
                        int64_t audio_in_channel_layout = av_get_default_channel_layout(audioChannels);
                        this->_audio_out_channels_layout = av_get_channel_layout_nb_channels(out_channel_layout);

                        //Create swr for converting audio
                        this->_audio_convert = swr_alloc_set_opts(
                            this->_audio_convert,
                            out_channel_layout,
                            out_sample_fmt,
                            audioSampleRate,
                            audio_in_channel_layout,
                            _audio_codec.avCodecCtx->sample_fmt,
                            audioSampleRate,
                            0,
                            NULL);
                        swr_init(this->_audio_convert);
                        //	
                        //auto le = 1000 / ((double)audioCodec.avFrame->sample_rate * (double)audioCodec.avFrame->nb_samples);
                        //Logger.Write(std::to_string(audioCodec.avFrame->sample_rate));
                        //Logger.Write(std::to_string(audioCodec.avFrame->nb_samples));
                        //Logger.Write(std::to_string(le));


                        //Seek to the specific time in seconds
                        if (pSeekToFrame != 0)
                        {
                            seek_frame(pSeekToFrame);
                        }

                        //Output the infomation
                        if (pLog4User)
                        {
                            msg = L"The media decoded successfully with the following information : time: " + this->_duration_time.to_wstring() + L" , fps: " + std::to_wstring(this->_frame_rate);
                            logger.user(msg);
                        }

                        msg.clear();
                    }

                    pMediaPath.clear();
                    this->_is_media_open = true;
                }
                return S_OK;
            }

            int64_t time_to_frame(int64_t pMilliSecond)
            {
                auto _videoStream = this->_video_codec.avStream;
                auto _frameNumber = av_rescale(pMilliSecond, _videoStream->time_base.den, _videoStream->time_base.num) / 1000;
                return _frameNumber;
            }

            int seek_frame_milliSecond(int64_t pMilliSecond)
            {
                auto _elapsedTotalMS = get_elapsed_time().get_total_milliseconds();
                auto _totalTimeMS = get_duration_time().get_total_milliseconds();

                if (pMilliSecond == _elapsedTotalMS || pMilliSecond > _totalTimeMS) return 0;

                auto _frame = time_to_frame(pMilliSecond);
                return seek_frame(_frame);
            }

            int seek_frame(int64_t pFrame)
            {
                if (pFrame <0) return 0;

                int _hr = 1;
                bool _clockUpdated = false;
                //Seek the video
                if (this->_video_stream_index > -1)
                {
                    auto _videoStream = this->_video_codec.avStream;
                    if (avformat_seek_file(_av_format_ctx, this->_video_stream_index, 0, pFrame, pFrame, AVSEEK_FLAG_FRAME) >= 0)
                    {
                        avcodec_flush_buffers(this->_video_codec.avCodecCtx);
                    }
                    else
                    {
                        _hr = -1;
                    }

                    _update_time(this->_video_codec.clock);
                    _clockUpdated = true;
                }

                //Wait one cycle
                Sleep(0);

                //Seek the audio
                if (this->_video_stream_index > -1)
                {
                    auto _videoStream = this->_video_codec.avStream;
                    if (avformat_seek_file(_av_format_ctx, this->_video_stream_index, 0, pFrame, pFrame, AVSEEK_FLAG_FRAME) >= 0)
                    {
                        avcodec_flush_buffers(this->_video_codec.avCodecCtx);
                    }
                    else
                    {
                        if (_hr == -1)
                        {
                            _hr = -3;
                        }
                        else
                        {
                            _hr = -2;
                        }
                    }

                    if (!_clockUpdated)
                    {
                        _update_time(this->_audio_codec.clock);
                    }
                }

                return _hr;
            }
            
            HRESULT buffer_video_to_memory(wolf::system::w_memory& pVideoMemory, UINT pDownSampling)
            {
                HRESULT hr = S_OK;

                auto width = get_video_frame_width();
                auto height = get_video_frame_height();

                int gotFrame = 0;
                //Start reading
                while (!gotFrame && av_read_frame(_av_format_ctx, this->_av_packet) >= 0)
                {
                    // Is this a packet from the video stream
                    if (this->_av_packet->stream_index == this->_video_stream_index)
                    {
#pragma region read video buffer

                        auto result = avcodec_decode_video2(this->_video_codec.avCodecCtx, this->_video_codec.avFrame, &gotFrame, this->_av_packet);
                        if (result < 0)
                        {
                            hr = S_FALSE;
                            V(hr, L"decoding video frame", this->_name);
                            return hr;
                        }

                        if (this->_av_packet->dts == AV_NOPTS_VALUE && this->_video_codec.avFrame->opaque && *(uint64_t*) this->_video_codec.avFrame->opaque != AV_NOPTS_VALUE)
                        {
                            this->_video_codec.pts = *(uint64_t *) this->_video_codec.avFrame->opaque;
                        }
                        else if (this->_av_packet->dts != AV_NOPTS_VALUE)
                        {
                            this->_video_codec.pts = this->_av_packet->dts;
                        }
                        else
                        {
                            this->_video_codec.pts = 0;
                        }
                        this->_video_codec.pts *= av_q2d(this->_video_codec.avStream->time_base);

                        if (!gotFrame) continue;

                        _update_clock(this->_video_codec);
                        _update_time(this->_video_codec.clock);

#pragma endregion

#pragma region Re-Scale and store av frame in to the DirectX format

                        //Set the target convert properties
                        auto format = (AVPixelFormat)this->_video_codec.avFrame->format;

                        auto _des_width = width;
                        auto _des_height = height;

                        bool _down_sampling = false;
                        if (pDownSampling != 0 && pDownSampling != 1)
                        {
                            _down_sampling = true;
                            _des_width /= pDownSampling;
                            _des_height /= pDownSampling;
                        }

                        struct SwsContext* swsContext = sws_getContext(
                            width,
                            height,
                            format,
                            _des_width,
                            _des_height,
                            AV_PIX_FMT_BGRA,
                            SWS_BICUBIC,
                            NULL,
                            NULL,
                            NULL);

                        const size_t _slice_pitch = _des_width * _des_height * 4;

                        auto _hr = S_FALSE;
                        if (_down_sampling)
                        {
                            //Re-Scale it
                            sws_scale(
                                swsContext,
                                this->_video_codec.avFrame->data,
                                this->_video_codec.avFrame->linesize,
                                0,
                                height,
                                this->_down_sample_avFrame->data,
                                this->_down_sample_avFrame->linesize);

                            //copy data to the destination
                            auto _frame = (uint8_t*)(this->_down_sample_avFrame->data[0]);
                            if (_frame)
                            {
                                _hr = pVideoMemory.allocate(sizeof(w_video_frame_down_sample_data), std::alignment_of<w_video_frame_down_sample_data>::value);
                                if (_hr)
                                {
                                    auto _address = pVideoMemory.get_address() - 1;
                                    auto _memory = pVideoMemory.read(_address);

                                    auto _videoFrameData = static_cast<w_video_frame_down_sample_data*>(_memory);
                                    if (_videoFrameData)
                                    {
                                        //first clear memory
                                        std::fill(_videoFrameData->data.begin(), _videoFrameData->data.end(), 0);
                                        tbb::parallel_for<size_t>(0, _slice_pitch, 4, [&](size_t i)
                                        {
                                            _videoFrameData->data.at(i / 4) = (int)((_frame[i + 3] & 0xFF) << 24) | ((_frame[i] & 0xFF) << 16) | ((_frame[i + 1] & 0xFF) << 8) | (_frame[i + 2] & 0xFF);//argb
                                        });
                                    }
                                    else
                                    {
                                        logger.error(L"Could not cast allocated memory for video");
                                        hr = S_FALSE;
                                    }

                                }
                                else
                                {
                                    logger.error(L"Could not allocate memory for video");
                                    hr = S_FALSE;
                                }

                                _frame = nullptr;
                            }
                        }
                        else
                        {
                            //Re-Scale it
                            sws_scale(
                                swsContext,
                                this->_video_codec.avFrame->data,
                                this->_video_codec.avFrame->linesize,
                                0,
                                height,
                                this->_av_Frame->data,
                                this->_av_Frame->linesize);

                            //copy data to the destination
                            auto _frame = (uint8_t*)(this->_av_Frame->data[0]);
                            if (_frame)
                            {
                                auto _size = width * height;
                                _hr = pVideoMemory.allocate(4 * _size, std::alignment_of<int>::value);
                                if (_hr)
                                {
                                    auto _address = pVideoMemory.get_address() - 1;
                                    auto _memory = pVideoMemory.read(_address);

                                    auto _data = static_cast<int*>(_memory);
                                    if (_data)
                                    {
                                        //first clear memory
                                        std::fill(&_data[0], &_data[_size - 1], 0);
                                        tbb::parallel_for<size_t>(0, _slice_pitch, 4, [&](size_t i)
                                        {
                                            _data[i / 4] = (int)((_frame[i + 3] & 0xFF) << 24) | ((_frame[i] & 0xFF) << 16) | ((_frame[i + 1] & 0xFF) << 8) | (_frame[i + 2] & 0xFF);//argb
                                        });
                                    }
                                    else
                                    {
                                        logger.error(L"Could not cast allocated memory for video");
                                        hr = S_FALSE;
                                    }

                                }
                                else
                                {
                                    logger.error(L"Could not allocate memory for video");
                                    hr = S_FALSE;
                                }

                                _frame = nullptr;
                            }
                        }

                        sws_freeContext(swsContext);

#pragma endregion

                    }

                    //free the packet
                    av_free_packet(this->_av_packet);
                }

                return hr;
            }

            HRESULT buffer_audio_to_memory(wolf::system::w_memory& pAudioMemory, double& pAudioFrameVolumeDB)
            {
                HRESULT hr = S_OK;

                auto width = get_video_frame_width();
                auto height = get_video_frame_height();

                pAudioFrameVolumeDB = 0;
                int gotFrame = 0;

                //Start storing all audio untill we get first video frame
                while (!gotFrame && av_read_frame(_av_format_ctx, this->_av_packet) >= 0)
                {
                    if (this->_av_packet->stream_index == this->_audio_stream_index)
                    {
#pragma region read audio buffer

                        auto result = avcodec_decode_audio4(this->_audio_codec.avCodecCtx, this->_audio_codec.avFrame, &gotFrame, this->_av_packet);
                        if (result < 0)
                        {
                            auto hr = S_FALSE;
                            V(hr, L"decoding audio frame", this->_name);
                            return hr;
                        }

                        if (this->_av_packet->dts == AV_NOPTS_VALUE && 
                            this->_audio_codec.avFrame->opaque && 
                            *(uint64_t*) this->_audio_codec.avFrame->opaque != AV_NOPTS_VALUE)
                        {
                            this->_audio_codec.pts = *(uint64_t *) this->_audio_codec.avFrame->opaque;
                        }
                        else if (this->_av_packet->dts != AV_NOPTS_VALUE)
                        {
                            this->_audio_codec.pts = this->_av_packet->dts;
                        }
                        else
                        {
                            this->_audio_codec.pts = 0;
                        }
                        this->_audio_codec.pts *= av_q2d(this->_audio_codec.avStream->time_base);

                        if (!gotFrame) continue;

                        _update_clock(this->_audio_codec);
                        _update_time(this->_audio_codec.clock);

                        auto _hr = pAudioMemory.allocate(sizeof(w_audio_frame_data), std::alignment_of<w_audio_frame_data>::value);
                        if (_hr)
                        {
                            auto _address = pAudioMemory.get_address() - 1;
                            auto _memory = pAudioMemory.read(_address);

                            auto _frame = new (_memory) w_audio_frame_data();
                            if (_frame)
                            {
                                auto _dataPtr = &_frame->data[0];
                                _frame->usedSize = static_cast<UINT>(swr_convert(
                                    this->_audio_convert,
                                    &_dataPtr,
                                    AUDIO_FRAME_SIZE,
                                    (const uint8_t **)this->_audio_codec.avFrame->data,
                                    this->_audio_codec.avFrame->nb_samples));

                                _frame->pts = this->_av_packet->pts;

                                double RootMeanSquare = 0;
                                for (size_t i = 0; i < _frame->usedSize; ++i)
                                {
                                    RootMeanSquare += std::pow(_dataPtr[i], 2);
                                }
                                RootMeanSquare = std::sqrt(RootMeanSquare / _frame->usedSize);
                                //Log("RootMeanSquare : " + QString::number(RootMeanSquare));

                                pAudioFrameVolumeDB = 20.000 * std::log10(RootMeanSquare / 32768);

                            }
                            else
                            {
                                logger.error(L"Could not cast allocated memory for audio");
                                hr = S_FALSE;
                            }
                        }
                        else
                        {
                            logger.error(L"Could not allocate memory for audio");
                            hr = S_FALSE;
                        }

#pragma endregion
                    }

                    //free the packet
                    av_free_packet(this->_av_packet);
                }


                return hr;
            }

            HRESULT buffer_to_memory(wolf::system::w_memory& pVideoMemory, wolf::system::w_memory& pAudioMemory)
            {
                HRESULT hr = S_OK;

                auto width = get_video_frame_width();
                auto height = get_video_frame_height();

                int gotVideoFrame = 0;
                int gotAudioFrame = 0;

                //Start storing all audio untill we get first video frame
                while (!gotVideoFrame && av_read_frame(_av_format_ctx, this->_av_packet) >= 0)
                {
                    // Is this a packet from the video stream
                    if (this->_av_packet->stream_index == this->_video_stream_index)
                    {
#pragma region read video buffer

                        auto result = avcodec_decode_video2(this->_video_codec.avCodecCtx, this->_video_codec.avFrame, &gotVideoFrame, this->_av_packet);
                        if (result < 0)
                        {
                            hr = S_FALSE;
                            V(hr, L"decoding video frame", this->_name);
                            return hr;
                        }

                        if (this->_av_packet->dts == AV_NOPTS_VALUE && this->_video_codec.avFrame->opaque && *(uint64_t*) this->_video_codec.avFrame->opaque != AV_NOPTS_VALUE)
                        {
                            this->_video_codec.pts = *(uint64_t *) this->_video_codec.avFrame->opaque;
                        }
                        else if (this->_av_packet->dts != AV_NOPTS_VALUE)
                        {
                            this->_video_codec.pts = _av_packet->dts;
                        }
                        else
                        {
                            this->_video_codec.pts = 0;
                        }
                        this->_video_codec.pts *= av_q2d(this->_video_codec.avStream->time_base);

                        if (!gotVideoFrame) continue;

                        _update_clock(this->_video_codec);
                        _update_time(this->_video_codec.clock);

#pragma region Re-Scale and store av frame in to the DirectX format

                        //Set the target convert properties
                        auto format = (AVPixelFormat)this->_video_codec.avFrame->format;

                        struct SwsContext* swsContext = sws_getContext(
                            width,
                            height,
                            format,
                            width,
                            height,
                            AV_PIX_FMT_BGRA,
                            SWS_BICUBIC,
                            NULL,
                            NULL,
                            NULL);

                        //Re-Scale it
                        sws_scale(
                            swsContext,
                            this->_video_codec.avFrame->data,
                            this->_video_codec.avFrame->linesize,
                            0,
                            height,
                            this->_av_Frame->data,
                            this->_av_Frame->linesize);

                        const size_t _rowPitch = width * 4; // 4 bytes of RGBA
                        const size_t _rowCount = height;
                        const size_t _slicePitch = _rowPitch * _rowCount;

                        //copy data to the destination
                        auto _frame = (uint8_t*)(this->_av_Frame->data[0]);
                        if (_frame)
                        {
                            auto _size = width * height;
                            auto _hr = pVideoMemory.allocate(sizeof(int) * _size,
                                std::alignment_of<int>::value);
                            if (_hr)
                            {
                                auto _address = pVideoMemory.get_address() - 1;
                                auto _memory = pVideoMemory.read(_address);

                                auto _videoFrameData = static_cast<int*>(_memory);
                                if (_videoFrameData)
                                {
                                    //first clear memory
                                    std::memset(&_videoFrameData, 0, _size);
                                    tbb::parallel_for<size_t>(0, _slicePitch, 4, [&](size_t i)
                                    {
                                        _videoFrameData[i / 4] = (int)((_frame[i + 3] & 0xFF) << 24) | ((_frame[i] & 0xFF) << 16) | ((_frame[i + 1] & 0xFF) << 8) | (_frame[i + 2] & 0xFF);//argb
                                    });
                                }
                                else
                                {
                                    logger.error(L"Could not cast allocated memory for video");
                                    hr = S_FALSE;
                                }

                            }
                            else
                            {
                                logger.error(L"Could not allocate memory for video");
                                hr = S_FALSE;
                            }

                            _frame = nullptr;
                        }

                        sws_freeContext(swsContext);

#pragma endregion

#pragma endregion
                    }
                    else if (this->_av_packet->stream_index == this->_audio_stream_index)
                    {
#pragma region read audio buffer

                        auto result = avcodec_decode_audio4(_audio_codec.avCodecCtx, _audio_codec.avFrame, &gotAudioFrame, _av_packet);
                        if (result < 0)
                        {
                            auto hr = S_FALSE;
                            V(hr, L"decoding audio frame", this->_name);
                            return hr;
                        }

                        if (_av_packet->dts == AV_NOPTS_VALUE && 
                            this->_audio_codec.avFrame->opaque && 
                            *(uint64_t*) this->_audio_codec.avFrame->opaque != AV_NOPTS_VALUE)
                        {
                            this->_audio_codec.pts = *(uint64_t *) this->_audio_codec.avFrame->opaque;
                        }
                        else if (_av_packet->dts != AV_NOPTS_VALUE)
                        {
                            this->_audio_codec.pts = _av_packet->dts;
                        }
                        else
                        {
                            this->_audio_codec.pts = 0;
                        }
                        this->_audio_codec.pts *= av_q2d(this->_audio_codec.avStream->time_base);

                        if (!gotAudioFrame) continue;

                        _update_clock(this->_audio_codec);
                        _update_time(this->_audio_codec.clock);

                        auto _hr = pAudioMemory.allocate(sizeof(w_audio_frame_data), std::alignment_of<w_audio_frame_data>::value);
                        if (_hr)
                        {
                            auto _address = pAudioMemory.get_address() - 1;
                            auto _memory = pAudioMemory.read(_address);

                            auto _frame = new (_memory) w_audio_frame_data();
                            if (_frame)
                            {
                                auto _dataPtr = &_frame->data[0];
                                _frame->usedSize = swr_convert(
                                    this->_audio_convert,
                                    &_dataPtr,
                                    AUDIO_FRAME_SIZE,
                                    (const uint8_t **)_audio_codec.avFrame->data,
                                    _audio_codec.avFrame->nb_samples);
                            }
                            else
                            {
                                logger.error("Could not cast allocated memory for audio");
                                hr = S_FALSE;
                            }
                        }
                        else
                        {
                            logger.error("Could not allocate memory for audio");
                            hr = S_FALSE;
                        }

#pragma endregion
                    }

                    //free the packet
                    av_free_packet(this->_av_packet);
                }

                return hr;
            }

            ULONG release()
            {
                if (this->_is_released) return 1;

                this->_full_path.clear();

                this->_audio_frame_volume_db = 0;

                //if (this->videoCodec.avCodec)
                //{
                //av_free(this->videoCodec.avCodec);
                //}
                if (this->_video_codec.avCodecCtx)
                {
                    avcodec_close(_video_codec.avCodecCtx);
                }
                if (this->_video_codec.avFrame)
                {
                    av_free(this->_video_codec.avFrame);
                }


                //if (this->audioCodec.avCodec)
                //{
                //av_free(this->audioCodec.avCodec);
                //}
                if (this->_audio_codec.avCodecCtx)
                {
                    avcodec_close(_audio_codec.avCodecCtx);
                }
                if (this->_audio_codec.avFrame)
                {
                    av_free(this->_audio_codec.avFrame);
                }

                //avcodec_free(&frame);

                //Release directX frame
                if (this->_av_Frame)
                {
                    auto getdstFrame = this->_av_Frame.get();
                    av_frame_free(&getdstFrame);
                    this->_av_Frame.release();
                }

                if (this->_down_sample_avFrame)
                {
                    auto getdstFrame = this->_down_sample_avFrame.get();
                    av_frame_free(&getdstFrame);
                    this->_down_sample_avFrame.release();
                }

                //Free the packet
                if (this->_av_packet)
                {
                    av_free_packet(this->_av_packet);
                }

                //Free the video format context
                if (this->_av_format_ctx)
                {
                    avformat_close_input(&this->_av_format_ctx);
                }

                //free swr
                if (this->_audio_convert)
                {
                    swr_free(&this->_audio_convert);
                }

                this->_is_released = true;
                return 0;
            }

#pragma region Getters

            bool is_open() const
            {
                return this->_is_media_open;
            }

            const char* get_media_path() const
            {
                return this->_full_path.c_str();
            }

            w_time_span get_duration_time() const
            {
                return this->_duration_time;
            }

            w_time_span get_elapsed_time() const
            {
                return this->_elapsed_time;
            }

            w_time_span get_remained_time() const
            {
                return this->_remained_time;
            }

            size_t get_total_video_frames() const
            {
                return this->_total_video_frames;
            }

            uint8_t get_duration_video_frames() const
            {
                return this->_duration_video_frames;
            }

            uint8_t get_elapsed_video_frames() const
            {
                return this->_elapsed_video_frames;
            }

            uint8_t get_remained_video_frames() const
            {
                return this->_remained_video_frames;
            }

            float get_video_frame_rate() const
            {
                return this->_frame_rate;
            }

            float get_audio_frame_rate() const
            {
                if (this->_audio_codec.avCodecCtx != nullptr)
                {
                    return this->_audio_codec.avCodecCtx->ticks_per_frame;
                }
                return 0;
            }

            int get_bit_rate() const
            {
                if (this->_av_format_ctx)
                {
                    return this->_av_format_ctx->bit_rate;
                }
                return 0;
            }

            int get_video_channels() const
            {
                if (this->_video_codec.avCodecCtx)
                {
                    return this->_video_codec.avCodecCtx->channels;
                }
                return 0;
            }

            int get_video_sample_rate() const
            {
                if (this->_video_codec.avCodecCtx)
                {
                    return this->_video_codec.avCodecCtx->sample_rate;
                }
                return 0;
            }

            int get_video_frame_width() const
            {
                if (this->_video_codec.avCodecCtx)
                {
                    return this->_video_codec.avCodecCtx->width;
                }
                return 0;
            }

            int get_video_frame_height() const
            {
                if (this->_video_codec.avCodecCtx)
                {
                    return this->_video_codec.avCodecCtx->height;
                }
                return 0;
            }

            int get_audio_channels() const
            {
                if (this->_audio_codec.avCodecCtx)
                {
                    return this->_audio_codec.avCodecCtx->channels;
                }
                return 0;
            }

            int get_audio_sample_rate() const
            {
                if (this->_audio_codec.avCodecCtx)
                {
                    return this->_audio_codec.avCodecCtx->sample_rate;
                }
                return 0;
            }

            double get_audio_frame_volume_db() const
            {
                return this->_audio_frame_volume_db;
            }

            int64_t get_packet_pts() const
            {
                return this->_av_packet->pts;
            }

#pragma endregion


        private:
            //Copy audio frame to the buffer
            HRESULT _copy_audio_frame_to(uint8_t* pBuffer, int& pBufferSize)
            {
                if (this->_audio_stream_index <= -1) return S_OK;

                int frameFinished = 0;
                auto result = avcodec_decode_audio4(this->_audio_codec.avCodecCtx, this->_audio_codec.avFrame, &frameFinished, _av_packet);
                if (result < 0)
                {
                    auto hr = S_FALSE;
                    V(hr, L"decoding audio frame", this->_name);
                    return hr;
                }

                if (_av_packet->dts == AV_NOPTS_VALUE && this->_audio_codec.avFrame->opaque && *(uint64_t*) this->_audio_codec.avFrame->opaque != AV_NOPTS_VALUE)
                {
                    this->_audio_codec.pts = *(uint64_t *) this->_audio_codec.avFrame->opaque;
                }
                else if (_av_packet->dts != AV_NOPTS_VALUE)
                {
                    this->_audio_codec.pts = _av_packet->dts;
                }
                else
                {
                    this->_audio_codec.pts = 0;
                }
                this->_audio_codec.pts *= av_q2d(this->_audio_codec.avStream->time_base);

                if (!frameFinished) return S_FALSE;

                _update_clock(this->_audio_codec);
                _update_time(this->_audio_codec.clock);

                pBufferSize = swr_convert(
                    this->_audio_convert,
                    &pBuffer,
                    AUDIO_FRAME_SIZE,
                    (const uint8_t **)this->_audio_codec.avFrame->data,
                    this->_audio_codec.avFrame->nb_samples);

                return S_OK;
            }

            void _update_clock(w_codec& pCodec)
            {
                double frame_delay;
                if (pCodec.pts != 0)
                {
                    /* if we have pts, set video clock to it */
                    pCodec.clock = pCodec.pts;
                }
                else
                {
                    /* if we aren't given a pts, set it to the clock */
                    pCodec.pts = pCodec.clock;
                }
                /* update the video clock */
                frame_delay = av_q2d(pCodec.avStream->time_base);
                /* if we are repeating a frame, adjust clock accordingly */
                frame_delay += pCodec.avFrame->repeat_pict * (frame_delay * 0.5);
                pCodec.clock += frame_delay;
            }

            void _update_time(double pClock)
            {
                this->_elapsed_time = w_time_span::from_milliseconds(pClock * 1000.000);
                this->_remained_time = this->_duration_time - this->_elapsed_time;

                //If we just finished
                this->_elapsed_video_frames = static_cast<byte>(((float)this->_elapsed_time.get_milliseconds() * this->_frame_rate) / 1000.0);
                if (this->_elapsed_time == this->_duration_time)
                {
                    this->_remained_video_frames = 0;
                }
                else
                {
                    this->_remained_video_frames = static_cast<byte>(this->_frame_rate - (float)this->_elapsed_video_frames);
                }
            }

            std::string                                 _name;
            bool										_is_released;

            std::unique_ptr<AVFrame>					_av_Frame;
            std::unique_ptr<AVFrame>					_down_sample_avFrame;

            bool										_is_media_open;
            AVFormatContext*							_av_format_ctx;
            w_codec										_video_codec;
            w_codec										_audio_codec;
            AVPacket*									_av_packet;
            struct SwrContext*							_audio_convert;
            int											_audio_out_channels_layout;
            int											_video_stream_index;
            int											_audio_stream_index;
            std::string									_full_path;

            wolf::system::w_time_span					_duration_time;
            wolf::system::w_time_span					_elapsed_time;
            wolf::system::w_time_span					_remained_time;

            size_t										_total_video_frames;
            byte										_duration_video_frames;
            byte										_elapsed_video_frames;
            byte										_remained_video_frames;

            float										_frame_rate;
            double										_audio_frame_volume_db;
        };
    }
}

w_media_core::w_media_core() :_pimp(new w_media_core_pimp())
{
    _super::set_class_name("w_media_core");
}

w_media_core::~w_media_core()
{
	release();
}

void w_media_core::register_all()
{
	std::call_once(onceInit, []()
	{
		// Register all formats and codec(s)
		av_register_all();
        avcodec_register_all();
        avformat_network_init();
		MFStartup(MF_VERSION);
	});
}

HRESULT w_media_core::open_media(std::wstring pMediaPath, int64_t pSeekToFrame, bool pEnableLogging)
{
    return this->_pimp ? this->_pimp->open_media(pMediaPath, pSeekToFrame, pEnableLogging) : S_FALSE;
}

int64_t w_media_core::time_to_frame(int64_t pMilliSecond)
{
    return this->_pimp ? this->_pimp->time_to_frame(pMilliSecond) : -1;
}

int w_media_core::seek_frame_milliSecond(int64_t pMilliSecond)
{
    return this->_pimp ? this->_pimp->seek_frame_milliSecond(pMilliSecond) : -3;
}

int w_media_core::seek_frame(int64_t pFrame)
{
    return this->_pimp ? this->_pimp->seek_frame(pFrame) : -3;
}

HRESULT w_media_core::buffer_video_to_memory(wolf::system::w_memory& pVideoMemory, UINT pDownSampling)
{
    return this->_pimp ? this->_pimp->buffer_video_to_memory(pVideoMemory, pDownSampling) : S_FALSE;
}

HRESULT w_media_core::buffer_audio_to_memory(w_memory& pAudioMemory, double& pAudioFrameVolumeDB)
{
    return this->_pimp ? this->_pimp->buffer_audio_to_memory(pAudioMemory, pAudioFrameVolumeDB) : S_FALSE;
}

HRESULT w_media_core::buffer_to_memory(w_memory& pVideoMemory, w_memory& pAudioMemory)
{
    return this->_pimp ? this->_pimp->buffer_to_memory(pVideoMemory, pAudioMemory) : S_FALSE;
}

ULONG w_media_core::release()
{
    if (this->get_is_released()) return 1;
    SAFE_RELEASE(this->_pimp);
    return _super::release();
}

void w_media_core::release_all()
{
	std::call_once(onceRelease, []()
	{
		MFShutdown();
	});
}

#pragma region Getters

bool w_media_core::is_open() const
{
    return this->_pimp ? this->_pimp->is_open() : false;
}

const char* w_media_core::get_media_path() const
{
    return this->_pimp ? this->_pimp->get_media_path() : nullptr;
}

w_time_span w_media_core::get_duration_time() const
{
    return this->_pimp ? this->_pimp->get_duration_time() : w_time_span();
}

w_time_span w_media_core::get_elapsed_time() const
{
    return this->_pimp ? this->_pimp->get_elapsed_time() : w_time_span();
}

w_time_span w_media_core::get_remained_time() const
{
    return this->_pimp ? this->_pimp->get_remained_time() : w_time_span();
}

size_t w_media_core::get_total_video_frames() const
{
    return this->_pimp ? this->_pimp->get_total_video_frames() : 0;
}

uint8_t w_media_core::get_duration_video_frames() const
{
    return this->_pimp ? this->_pimp->get_duration_video_frames() : 0;
}

uint8_t w_media_core::get_elapsed_video_frames() const
{
    return this->_pimp ? this->_pimp->get_elapsed_video_frames() : 0;
}

uint8_t w_media_core::get_remained_video_frames() const
{
    return this->_pimp ? this->_pimp->get_remained_video_frames() : 0;
}

float w_media_core::get_video_frame_rate() const
{
    return this->_pimp ? this->_pimp->get_video_frame_rate() : 0.0f;
}

float w_media_core::get_audio_frame_rate() const
{
    return this->_pimp ? this->_pimp->get_audio_frame_rate() : 0.0f;
}

int w_media_core::get_bit_rate() const
{
    return this->_pimp ? this->_pimp->get_bit_rate() : 0;
}

int w_media_core::get_video_channels() const
{
    return this->_pimp ? this->_pimp->get_video_channels() : 0;
}

int w_media_core::get_video_sample_rate() const
{
    return this->_pimp ? this->_pimp->get_video_sample_rate() : 0;
}

int w_media_core::get_video_frame_width() const
{
    return this->_pimp ? this->_pimp->get_video_frame_width() : 0;
}

int w_media_core::get_video_frame_height() const
{
    return this->_pimp ? this->_pimp->get_video_frame_height() : 0;
}

int w_media_core::get_audio_channels() const
{
    return this->_pimp ? this->_pimp->get_audio_channels() : 0;
}

int w_media_core::get_audio_sample_rate() const
{
    return this->_pimp ? this->_pimp->get_audio_sample_rate() : 0;
}

double w_media_core::get_audio_frame_volume_db() const
{
    return this->_pimp ? this->_pimp->get_audio_frame_volume_db() : 0.0;
}

int64_t w_media_core::get_packet_pts() const
{
    return this->_pimp ? this->_pimp->get_packet_pts() : 0.0;
}

#pragma endregion
