#include "w_media_core_pch.h"
#include "w_media_core.h"
#include <cmath>
#include <iomanip>      // For std::hex
#include <codecvt>      // For Unicode conversions
#include <w_thread.h>
#include <w_std.h>
#include <w_game_time.h>

#ifdef __WIN32

#include <io.h>
#include <wrl/client.h>
static const MFTIME MF_ONE_SEC = 10000000;
static std::once_flag once_release_flag;

#else

static const __int64_t MF_ONE_SEC = 10000000;

#endif

static const long MF_ONE_MSEC = 1000;
static std::once_flag once_initialize_flag;

using namespace wolf;
using namespace wolf::system;

namespace wolf
{
    namespace framework
    {
        //struct w_video_frame_down_sample_data
        //{
        //    std::array<int, VIDEO_FRAME_DOWN_SAMPLE_SIZE> data;
        //};
        //struct w_audio_frame_data
        //{
        //    uint8_t             data[AUDIO_FRAME_SIZE];
        //    uint32_t            usedSize;
        //    int64_t             pts;
        //};
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
                _av_format_ctx(nullptr),
                _av_packet(nullptr),
                _audio_convert(nullptr),
				_stream_out(nullptr),
                _stream_out_ctx(nullptr),
				_stream_out_frame(nullptr),
                _frame_max_delay_in_ms(1000)
            {
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

            W_RESULT open_media(_In_z_ std::wstring pMediaPath, _In_ int64_t pSeekToFrame)
            {
				const auto _trace_info = this->_name + "::open_media";

                if (pMediaPath.empty()) return W_FAILED;

                release_media();
                
                std::memset(&this->_video_codec, 0, sizeof(w_codec));
                std::memset(&this->_audio_codec, 0, sizeof(w_codec));

                //Initialize the packet
                this->_av_packet = new AVPacket();
                av_init_packet(this->_av_packet);

                this->_audio_convert = swr_alloc();
                this->_av_format_ctx = avformat_alloc_context();

                this->_is_media_open = false;
                {
                    this->_media_full_path = wolf::system::convert::to_utf8(pMediaPath).c_str();

                    //Output the message
                    logger.write(L"Media from following path {} is going to decode. trace info", pMediaPath);
                    
					W_RESULT hr = W_PASSED;
                    // Open video file
                    bool containVideo = true, containAudio = true;


                    //Open file
                    if (avformat_open_input(&_av_format_ctx, this->_media_full_path.c_str(), NULL, NULL) != 0)
                    {
                        hr = W_FAILED;
                        V(hr, "openning file. trace info: {}", _trace_info);
                        release_media();
                        return hr;
                    }

                    // Retrieve stream information
					if (avformat_find_stream_info(_av_format_ctx, NULL) < 0)
					{
						hr = W_FAILED;
						V(hr, "finding file stream information. trace info: {}", _trace_info);
						release_media();
						return hr;
					}

#ifdef _DEBUG
                    // Dump information about file onto standard error
                    //	av_dump_format(avFormatCtx, 0, path, 0);
#endif

                    // Find the first video & audio stream
                    this->_video_stream_index = av_find_best_stream(_av_format_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
                    this->_audio_stream_index = av_find_best_stream(_av_format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);

                    auto _has_audio_stream = true;
                    auto _has_video_stream = true;

                    if (this->_video_stream_index <= -1)
                    {
                        hr = W_FAILED;
						_has_video_stream = false;
                        V(hr, "finding stream of video, media file does not have any video frame. trace info: {}", _trace_info);
                    }
                    if (this->_audio_stream_index <= -1)
                    {
                        hr = W_FAILED;
						_has_audio_stream = false;
                        V(hr, "finding stream of audio, media file does not have any audio frame. trace info: {}", _trace_info);
                    }

                    if (!_has_video_stream && !_has_audio_stream)
                    {
						hr = W_FAILED;
						V(hr, "File {} did not load. trace info: {}", this->_media_full_path, _trace_info);
                        release_media();
                        return W_FAILED;
                    }

                    //Get a pointer to the stream of video & audio
                    if (_has_video_stream)
                    {
                        _video_codec.avStream = _av_format_ctx->streams[this->_video_stream_index];
                        _video_codec.avCodecCtx = _av_format_ctx->streams[this->_video_stream_index]->codec;
                    }

                    if (_has_audio_stream)
                    {
                        this->_audio_codec.avStream = _av_format_ctx->streams[this->_audio_stream_index];
                        this->_audio_codec.avCodecCtx = _av_format_ctx->streams[this->_audio_stream_index]->codec;
                    }

#ifdef __WIN32
#pragma region Get the information of media from Windows SDK

                    MF_OBJECT_TYPE objectType = MF_OBJECT_INVALID;
                    Microsoft::WRL::ComPtr<IUnknown> pSource = nullptr;
                    Microsoft::WRL::ComPtr<IMFSourceResolver> sourceResolver = NULL;
                    Microsoft::WRL::ComPtr<IMFMediaSource> source;

                    hr = (W_RESULT)MFCreateSourceResolver(&sourceResolver);
                    if (FAILED(hr))
                    {
                        logger.write(L"creating source resolver");
                    }
                    hr = (W_RESULT)sourceResolver->CreateObjectFromURL(
                        pMediaPath.c_str(),
                        MF_RESOLUTION_MEDIASOURCE,
                        NULL,
                        &objectType,
                        &pSource);
                    if (FAILED(hr) || objectType == MF_OBJECT_TYPE::MF_OBJECT_INVALID || pSource == nullptr)
                    {
                        logger.error(L"Invalid object type, creating object from source for source resolver.");
                        release_media();
                        return W_FAILED;
                    }

                    hr = (W_RESULT)pSource.Get()->QueryInterface(IID_PPV_ARGS(&source));
                    if (FAILED(hr))
                    {
                        logger.write(L"Query interface for media source.");
                    }

                    Microsoft::WRL::ComPtr<IMFPresentationDescriptor> pPD = nullptr;
                    hr = (W_RESULT)source->CreatePresentationDescriptor(&pPD);
                    if (FAILED(hr))
                    {
                        logger.write(L"creating presentation descriptor.");
                    }
                    auto _duration_mftime = new MFTIME(0);
                    pPD->GetUINT64(MF_PD_DURATION, (UINT64*)_duration_mftime);
                    auto _duration = (LONG)(*_duration_mftime / (MF_ONE_SEC / MF_ONE_MSEC));
					_duration_mftime = nullptr;

                    //Release 
                    COMPTR_RELEASE(pPD);
                    COMPTR_RELEASE(pSource);
                    COMPTR_RELEASE(sourceResolver);

                    this->_duration_time = w_time_span::from_milliseconds(_duration);
#else
                    auto _duration_seconds = static_cast<double>(this->_video_codec.avStream->duration) / AV_TIME_BASE;
                    this->_duration_time = w_time_span::from_seconds(_duration_seconds);
#endif
#pragma endregion


                    this->_remained_time = this->_duration_time;
                    this->_elapsed_time = w_time_span::zero();

                    if (_has_video_stream)
                    {
                        //AVRational avr = _video_codec.avCodecCtx->time_base;
                        //auto ticksPerFrame = _video_codec.avCodecCtx->ticks_per_frame;
						this->_frame_rate = av_q2d(this->_video_codec.avCodecCtx->framerate);// static_cast<float>(avr.den) / static_cast<float>(avr.num * _video_codec.avCodecCtx->ticks_per_frame);
                    }
                    else if (_has_audio_stream)
                    {
                        //AVRational avr = _audio_codec.avCodecCtx->time_base;
                        //auto ticksPerFrame = _audio_codec.avCodecCtx->ticks_per_frame;
						//this->_frame_rate = static_cast<float>(avr.den) / static_cast<float>(avr.num * _audio_codec.avCodecCtx->ticks_per_frame);
						this->_frame_rate = av_q2d(this->_audio_codec.avCodecCtx->framerate);
                    }

                    //Set total frames
                    this->_duration_video_frames = static_cast<uint8_t>(std::round(((double)this->_duration_time.get_milliseconds() * this->_frame_rate) / 1000.0));
                    this->_remained_video_frames = this->_duration_video_frames;
                    this->_elapsed_video_frames = 0;

                    this->_total_video_frames = static_cast<size_t>(((double)this->_duration_time.get_total_milliseconds() * this->_frame_rate) / 1000.0);
                    //Find the decoder for the video and audio and also allocate frames for video & audio

                    if (_has_video_stream)
                    {
                        _video_codec.avCodec = avcodec_find_decoder(_video_codec.avCodecCtx->codec_id);
                        if (_video_codec.avCodec == NULL)
                        {
                            hr = W_FAILED;
                            V(hr, "unsupported codec for video. trace info: {}.", _trace_info);
                            release_media();
                            return hr;
                        }

                        //Open codec
                        if (avcodec_open2(_video_codec.avCodecCtx, _video_codec.avCodec, NULL) < 0)
                        {
                            hr = W_FAILED;
                            V(hr, "could not open video codec. trace info: {}", _trace_info);
                            release_media();
                            return hr;
                        }

                        _video_codec.avFrame = av_frame_alloc();//avcodec_alloc_frame deprecated function

                        auto videoBitRater = get_bit_rate();
                        auto videoChannels = get_video_channels();
                        auto videoSampleRate = get_video_sample_rate();

                        auto width = get_video_frame_width();
                        auto height = get_video_frame_height();

                        //Create destination avFrame
                        this->_av_frame.reset(av_frame_alloc());
                        av_frame_unref(this->_av_frame.get()); // avcodec_get_frame_defaults deprecated function

                                                               //This time we need BGRA for directX
                        avpicture_alloc(
                            (AVPicture*) this->_av_frame.get(),
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


					if (_has_audio_stream)
					{
						_audio_codec.avCodec = avcodec_find_decoder(_audio_codec.avCodecCtx->codec_id);
						if (_audio_codec.avCodec == NULL)
						{
							hr = W_FAILED;
							V(hr, "unsupported codec for audio. trace info: {}", _trace_info);
							release_media();
							return hr;
						}

						if (avcodec_open2(_audio_codec.avCodecCtx, _audio_codec.avCodec, NULL) < 0)
						{
							hr = W_FAILED;
							V(hr, "could not open audio codec. trace info: {}", _trace_info);
							release_media();
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
						logger.write(L"The media decoded successfully with the following information : time: {}, fps {}",
							this->_duration_time.to_wstring(), this->_frame_rate);
					}

                    pMediaPath.clear();
                    this->_is_media_open = true;
                }
                return W_PASSED;
            }

			W_RESULT open_stream_server(
				_In_z_ const char* pURL,
				_In_z_ const char* pProtocol,
				_In_z_ const char* pFormatName,
				_In_ const AVCodecID& pCodecID,
				_In_ const int64_t& pFrameRate,
				_In_ const AVPixelFormat& pPixelFormat,
				_In_ const uint32_t& pWidth,
				_In_ const uint32_t& pHeight,
				_In_ w_signal<void(const w_media_core::w_stream_connection_info&)>& pOnConnectionEstablished,
				_In_ w_signal<void(const w_media_core::w_stream_frame_info&)>& pOnFillingVideoFrameBuffer,
				_In_ w_signal<void(const char*)>& pOnConnectionClosed)
			{
				const std::string _trace_info = this->_name + "::open_stream_server";

				//create output context
				avformat_alloc_output_context2(&this->_stream_out_ctx, NULL, pFormatName, pURL);
				if (!this->_stream_out_ctx)
				{
					V(W_FAILED, w_log_type::W_ERROR,
						"allocating output context for output streaming {}. trace info: {}", pURL, _trace_info);
					return W_FAILED;
				}
				if (!this->_stream_out_ctx->oformat)
				{
					V(W_FAILED, w_log_type::W_ERROR,
						"creating output streaming {} format: {}. trace info: {}", pURL, pFormatName, _trace_info);
					release_output_stream_server();
					return W_FAILED;
				}

				//find the encoder
				auto _stream_video_codec = avcodec_find_encoder(pCodecID);
				if (!_stream_video_codec)
				{
					V(W_FAILED, w_log_type::W_ERROR,
						"finding encoder for codec id: {} for output stream {}. trace info: {}",
						avcodec_get_name(pCodecID), pURL, _trace_info);
					release_output_stream_server();
					return W_FAILED;
				}
				else
				{
					this->_stream_out = avformat_new_stream(this->_stream_out_ctx, _stream_video_codec);
					if (!this->_stream_out)
					{
						V(W_FAILED, w_log_type::W_ERROR,
							"allocating output stream {}. trace info: {}", pURL, _trace_info);
						release_output_stream_server();
						return W_FAILED;
					}
					else
					{
						this->_stream_out->id = this->_stream_out_ctx->nb_streams - 1;
						this->_stream_out->time_base.den = this->_stream_out->pts.den = 90000;
						this->_stream_out->time_base.num = this->_stream_out->pts.num = 1;

						auto _bit_rate = 5000 * 1000; //5000 kbps * 1000

						this->_stream_out->codec->codec_id = pCodecID;
						this->_stream_out->codec->bit_rate = _bit_rate;
						this->_stream_out->codec->bit_rate_tolerance = _bit_rate;
						this->_stream_out->codec->rc_max_rate = _bit_rate;
						this->_stream_out->codec->width = static_cast<int>(pWidth);
						this->_stream_out->codec->height = static_cast<int>(pHeight);
						this->_stream_out->codec->time_base.den = pFrameRate;
						this->_stream_out->codec->time_base.num = 1;
						this->_stream_out->codec->gop_size = 12; //emit one intra frame every twelve frames at most
						this->_stream_out->codec->pix_fmt = pPixelFormat;
					}
				}

				AVDictionary* _av_dic = NULL; // "create" an empty dictionary
				av_dict_set(&_av_dic, "rtsp_transport", pProtocol, 0); // add an entry
				av_dict_set(&_av_dic, "timeout", "-1", 0); // add an entry

				AVPicture  _stream_dst_picture = {};
				//now open video stream 
				if (avcodec_open2(this->_stream_out->codec, _stream_video_codec, &_av_dic))
				{
					V(W_FAILED, w_log_type::W_ERROR,
						"opening video codec for output stream {}. trace info: {}", pURL, _trace_info);
					release_output_stream_server();
					return W_FAILED;
				}
				else
				{
					//allocate and init a re-usable frame */
					this->_stream_out_frame = av_frame_alloc();
					if (!this->_stream_out_frame)
					{
						V(W_FAILED, w_log_type::W_ERROR,
							"allocating video stream frame for output stream {} . trace info: {}", pURL, _trace_info);
						release_output_stream_server();
						return W_FAILED;
					}
					else
					{
						auto _width = this->_stream_out->codec->width;
						auto _height = this->_stream_out->codec->height;

						this->_stream_out_frame->format = this->_stream_out->codec->pix_fmt;
						this->_stream_out_frame->width = _width;
						this->_stream_out_frame->height = _height;

						//Allocate the encoded raw picture.
						if (avpicture_alloc(&_stream_dst_picture, this->_stream_out->codec->pix_fmt, _width, _height) < 0)
						{
							V(W_FAILED, w_log_type::W_ERROR,
								"allocating video stream picture for output stream {} . trace info: {}", pURL, _trace_info);
							release_output_stream_server();
							return W_FAILED;
						}
						else
						{
							//copy data and linesize picture pointers to frame
							*((AVPicture*)this->_stream_out_frame) = _stream_dst_picture;
						}
					}
				}

				if (avformat_write_header(this->_stream_out_ctx, NULL) < 0)
				{
					V(W_FAILED, w_log_type::W_ERROR,
						"connecting to output stream server {}. trace info: {}", pURL, _trace_info);
					release_output_stream_server();
					return W_FAILED;
				}

				auto _w = static_cast<uint32_t>(this->_stream_out->codec->width);
				auto _h = static_cast<uint32_t>(this->_stream_out->codec->height);

				auto _sws_ctx = sws_getContext(
					_w, _h,
					AV_PIX_FMT_RGB24, _w, _h,
					pPixelFormat, 0, 0, 0, 0);
				if (!_sws_ctx)
				{
					V(W_FAILED, w_log_type::W_ERROR,
						"getting sws context for output stream server {}. trace info: {}", pURL, _trace_info);
					release_output_stream_server();
					return W_FAILED;
				}

				//on connection established
				w_media_core::w_stream_connection_info _con_info;
				_con_info.url = pURL;
				_con_info.context = this->_stream_out_ctx;
				_con_info.stream = this->_stream_out;

				w_media_core::w_stream_frame_info _frame_info = {};
				_frame_info.stride = 3;
				_frame_info.width = _w;
				_frame_info.height = _h;
				_frame_info.pixels = (uint8_t*)malloc(_frame_info.width * _frame_info.height * _frame_info.stride * sizeof(uint8_t));

				int _line_size[1] = { (int)_frame_info.stride * (int)_w };

				int _got_packet = 0;
				AVPacket _packet = { 0 };
				//init packet
				av_init_packet(&_packet);

				pOnConnectionEstablished.emit(_con_info);
				{
					auto _start_send_time = std::chrono::system_clock::now();
					while (this->_stream_out)
					{
						auto _start_frame_time = std::chrono::system_clock::now();

#pragma region Preparing Packets
						_frame_info.index++;

						pOnFillingVideoFrameBuffer.emit(_frame_info);

						//convert pixels to output stream format
						sws_scale(
							_sws_ctx,
							&_frame_info.pixels,
							_line_size,
							0,
							_h,
							_stream_dst_picture.data,
							_stream_dst_picture.linesize);

						//encode the image
						this->_stream_out_frame->pts = _frame_info.index;
						if (avcodec_encode_video2(
							this->_stream_out->codec,
							&_packet,
							this->_stream_out_frame,
							&_got_packet) < 0)
						{
							V(W_FAILED, w_log_type::W_ERROR,
								"encoding video frame for output stream {}. trace info: {}", pURL, _trace_info);
						}
						else
						{
							if (_got_packet)
							{
								_packet.stream_index = this->_stream_out->index;
								_packet.pts = av_rescale_q_rnd(
									_packet.pts,
									this->_stream_out->codec->time_base,
									this->_stream_out->time_base,
									AVRounding(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
								if (av_write_frame(this->_stream_out_ctx, &_packet) < 0)
								{
									V(W_FAILED, w_log_type::W_ERROR,
										"writing video frame for output stream {}. trace info: {}", pURL, _trace_info);
								}
							}
						}
#pragma endregion

						auto _now = std::chrono::system_clock::now();
						_frame_info.stream_duration = std::chrono::duration_cast<std::chrono::milliseconds>(_now - _start_send_time).count();
						_frame_info.frame_duration = std::chrono::duration_cast<std::chrono::milliseconds>(_now - _start_frame_time).count();
						if (_frame_info.frame_duration > _frame_max_delay_in_ms)
						{
							V(W_FAILED, w_log_type::W_ERROR,
								"streaming delay is greater than max frame delay for output stream server {}. trace info: {}", pURL, _trace_info);
							break;
						}
						else
						{
							std::this_thread::sleep_for(std::chrono::milliseconds((long)(1000.0 / (double)pFrameRate - (double)_frame_info.frame_duration)));
						}

						//prepare packet
						av_free_packet(&_packet);
						av_init_packet(&_packet);
					}
				}

				//free pixels
				if (_frame_info.pixels)
				{
					free(_frame_info.pixels);
					_frame_info.pixels = nullptr;
				}
				//free dst picture
				av_free(_stream_dst_picture.data[0]);
				//release stream server resources
				release_output_stream_server();
				//on connection lost
				pOnConnectionClosed.emit(pURL);

				return W_PASSED;
			}

			W_RESULT open_stream_client(
				_In_z_ const char* pURL,
				_In_z_ const char* pProtocol,
				_In_z_ const char* pFormatName,
				_In_ const AVCodecID& pCodecID,
				_In_ const AVPixelFormat& pPixelFormat,
				_In_ const uint32_t& pWidth,
				_In_ const uint32_t& pHeight,
				_In_ const long long& pTimeOut,
				_In_ w_signal<void(const w_media_core::w_stream_connection_info&)>& pOnConnectionEstablished,
				_In_ w_signal<void(const w_media_core::w_stream_frame_info&)>& pOnGettingStreamVideoFrame,
				_In_ w_signal<void(const char*)>& pOnConnectionLost,
				_In_ w_signal<void(const char*)>& pOnConnectionClosed)
			{
				const std::string _trace_info = this->_name + "::open_stream_client";

				std::string _fromat_str(pFormatName);
				std::transform(_fromat_str.begin(), _fromat_str.end(), _fromat_str.begin(), ::tolower);

				AVDictionary* _av_dic = NULL;//"create" an empty dictionary
				if (_fromat_str == "rtsp")
				{
					av_dict_set(&_av_dic, "rtsp_flags", "listen", 0); // add an entry
					av_dict_set(&_av_dic, "rtsp_transport", pProtocol, 0); // add an entry
				}

				AVFormatContext* _stream_in_format_ctx = avformat_alloc_context();
				//create an input context
				if (avformat_open_input(&_stream_in_format_ctx, pURL, NULL, &_av_dic) < 0)
				{
					V(W_FAILED, w_log_type::W_ERROR,
						"openning context for input stream {}. trace info: {}", pURL, _trace_info);
					return W_FAILED;
				}

				if (!_stream_in_format_ctx)
				{
					V(W_FAILED, w_log_type::W_ERROR,
						"allocating context for input stream {}. trace info: {}", pURL, _trace_info);
					return W_FAILED;
				}

				if (avformat_find_stream_info(_stream_in_format_ctx, NULL) < 0)
				{
					V(W_FAILED, w_log_type::W_ERROR,
						"could not find input stream {} format: {}. trace info: {}", pURL, pFormatName, _trace_info);
					avformat_free_context(_stream_in_format_ctx);
					return W_FAILED;
				}

				int _video_stream_index = -1;
				//search video stream
				for (size_t i = 0; i < _stream_in_format_ctx->nb_streams; ++i)
				{
					if (_stream_in_format_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
					{
						_video_stream_index = i;
						break;
					}
				}

				//open input stream frame 
				auto _stream_in_frame_ctx = avformat_alloc_context();
				if (!_stream_in_frame_ctx)
				{
					V(W_FAILED, w_log_type::W_ERROR,
						"allocating context for input stream: {}. trace info: {}", pURL, _trace_info);
					avformat_free_context(_stream_in_format_ctx);
					return W_FAILED;
				}

				//start reading packets from stream
				av_read_play(_stream_in_format_ctx);

				AVCodec* _codec = NULL;
				_codec = avcodec_find_decoder(pCodecID);
				if (!_codec)
				{
					V(W_FAILED, w_log_type::W_ERROR,
						"could not find decoder for codec {}. input stream: {}. trace info: {}", pCodecID, pURL, _trace_info);
					avformat_free_context(_stream_in_format_ctx);
					avformat_free_context(_stream_in_frame_ctx);
					return W_FAILED;
				}

				// Add this to allocate the context by codec
				auto _codec_ctx = avcodec_alloc_context3(_codec);
				if (!_codec_ctx)
				{
					V(W_FAILED, w_log_type::W_ERROR,
						"allocating context for codec {}. input stream: {}. trace info: {}", pCodecID, pURL, _trace_info);
					avformat_free_context(_stream_in_format_ctx);
					avformat_free_context(_stream_in_frame_ctx);
					return W_FAILED;
				}

				if (avcodec_get_context_defaults3(_codec_ctx, _codec) < 0)
				{
					V(W_FAILED, w_log_type::W_ERROR,
						"getting context for codec {}. input stream: {}. trace info: {}", pCodecID, pURL, _trace_info);
					avcodec_free_context(&_codec_ctx);
					avformat_free_context(_stream_in_format_ctx);
					avformat_free_context(_stream_in_frame_ctx);
					return W_FAILED;
				}

				if (avcodec_copy_context(_codec_ctx,
					_stream_in_format_ctx->streams[_video_stream_index]->codec) < 0)
				{
					V(W_FAILED, w_log_type::W_ERROR,
						"copying context for codec {}. input stream: {}. trace info: {}", pCodecID, pURL, _trace_info);
					avcodec_free_context(&_codec_ctx);
					avformat_free_context(_stream_in_format_ctx);
					avformat_free_context(_stream_in_frame_ctx);
					return W_FAILED;
				}

				if (avcodec_open2(_codec_ctx, _codec, NULL) < 0)
				{
					V(W_FAILED, w_log_type::W_ERROR,
						"opening codec {}. input stream: {}. trace info: {}", pCodecID, pURL, _trace_info);
					avcodec_free_context(&_codec_ctx);
					avformat_free_context(_stream_in_format_ctx);
					avformat_free_context(_stream_in_frame_ctx);
					return W_FAILED;
				}

				auto _stream_in = avformat_new_stream(
					_stream_in_frame_ctx,
					_stream_in_format_ctx->streams[_video_stream_index]->codec->codec);
				if (!_stream_in)
				{
					V(W_FAILED, w_log_type::W_ERROR,
						"creating stream for {}. trace info: {}", pURL, _trace_info);
					avcodec_free_context(&_codec_ctx);
					avformat_free_context(_stream_in_format_ctx);
					avformat_free_context(_stream_in_frame_ctx);
					return W_FAILED;
				}

				if (avcodec_copy_context(
					_stream_in->codec,
					_stream_in_format_ctx->streams[_video_stream_index]->codec) < 0)
				{
					V(W_FAILED, w_log_type::W_ERROR,
						"codec copy context for stream for {}. trace info: {}", pURL, _trace_info);
					avcodec_close(_stream_in->codec);
					avcodec_free_context(&_codec_ctx);
					avformat_free_context(_stream_in_format_ctx);
					avformat_free_context(_stream_in_frame_ctx);
					return W_FAILED;
				}

				_stream_in->sample_aspect_ratio = _stream_in_format_ctx->streams[_video_stream_index]->codec->sample_aspect_ratio;

				SwsContext* _img_convert_ctx = sws_getContext(
					_codec_ctx->width,
					_codec_ctx->height,
					_codec_ctx->pix_fmt,
					_codec_ctx->width,
					_codec_ctx->height,
					AV_PIX_FMT_RGB24,
					SWS_BICUBIC,
					NULL,
					NULL,
					NULL);
				if (!_img_convert_ctx)
				{
					V(W_FAILED, w_log_type::W_ERROR,
						"getting sws context. input stream: {}. trace info: {}", pURL, _trace_info);
					avcodec_close(_stream_in->codec);
					avcodec_free_context(&_codec_ctx);
					avformat_free_context(_stream_in_format_ctx);
					avformat_free_context(_stream_in_frame_ctx);
					return W_FAILED;
				}

				//on connection established
				w_media_core::w_stream_connection_info _con_info;
				_con_info.url = pURL;
				_con_info.context = _stream_in_format_ctx;
				_con_info.stream = _stream_in;

				w_media_core::w_stream_frame_info _frame_info = {};
				_frame_info.stride = 3;//RGB
				_frame_info.width = static_cast<uint32_t>(_codec_ctx->width);
				_frame_info.height = static_cast<uint32_t>(_codec_ctx->height);

				int _size_stream_format = avpicture_get_size(
					pPixelFormat,
					_codec_ctx->width,
					_codec_ctx->height);

				uint8_t* _picture_buffer = (uint8_t*)(av_malloc(_size_stream_format));
				AVFrame* _picture = av_frame_alloc();
				AVFrame* _picture_rgb = av_frame_alloc();
				int _size_rgb = avpicture_get_size(
					AV_PIX_FMT_RGB24,
					_codec_ctx->width,
					_codec_ctx->height);

				auto _picture_rgb_buffer = (uint8_t*)(av_malloc(_size_rgb));
				avpicture_fill(
					(AVPicture*)_picture,
					_picture_buffer,
					pPixelFormat,
					_codec_ctx->width,
					_codec_ctx->height);

				avpicture_fill(
					(AVPicture*)_picture_rgb,
					_picture_rgb_buffer,
					AV_PIX_FMT_RGB24,
					_codec_ctx->width,
					_codec_ctx->height);

				int _got_packet = 0, _check = 0;;
				AVPacket _packet = { 0 };
				av_init_packet(&_packet);

				long long _time_out = pTimeOut;
				w_game_time _is_stream_live;
				_is_stream_live.set_target_elapsed_seconds(1);
				_is_stream_live.set_fixed_time_step(true);

				bool _connected = false;
				bool _exit = false;
				while (!_exit)
				{
					if (av_read_frame(_stream_in_format_ctx, &_packet) >= 0)
					{
						if (!_connected)
						{
							_connected = true;
							_is_stream_live.reset();
							_time_out = pTimeOut;

							pOnConnectionEstablished.emit(_con_info);
						}
						if (_packet.stream_index == _video_stream_index)
						{
							_check = 0;
							_packet.stream_index = _stream_in->id;

							if (avcodec_decode_video2(_codec_ctx, _picture, &_check, &_packet) > 0)
							{
								_frame_info.index++;

								sws_scale(
									_img_convert_ctx,
									_picture->data,
									_picture->linesize,
									0,
									_codec_ctx->height,
									_picture_rgb->data,
									_picture_rgb->linesize);

								_frame_info.pixels = _picture_rgb->data[0];
								pOnGettingStreamVideoFrame.emit(_frame_info);
							}
						}

						//prepare packet
						av_free_packet(&_packet);
						av_init_packet(&_packet);
					}
					else
					{
						//on connection lost
						_connected = false;
						pOnConnectionLost.emit(pURL);

						_is_stream_live.tick([&]()
						{
							_time_out -= _is_stream_live.get_elapsed_seconds() * 1000.000f;
							if (_time_out <= 0)
							{
								_exit = true;
							}
						});
					}
				}
				//on connection lost
				pOnConnectionClosed.emit(pURL);

				//free pixels				
				av_free(_picture);
				av_free(_picture_rgb);

				if (_codec_ctx)
				{
					avcodec_free_context(&_codec_ctx);
				}
				av_read_pause(_stream_in_format_ctx);
				if (_stream_in)
				{
					avcodec_close(_stream_in->codec);
				}
				if (_stream_in_frame_ctx)
				{
					avio_close(_stream_in_frame_ctx->pb);
					avformat_free_context(_stream_in_frame_ctx);
				}

				return W_PASSED;
			}

            int64_t time_to_frame(_In_ int64_t pMilliSecond)
            {
                auto _videoStream = this->_video_codec.avStream;
                auto _frameNumber = av_rescale(pMilliSecond, _videoStream->time_base.den, _videoStream->time_base.num) / 1000;
                return _frameNumber;
            }

            int seek_frame_milliSecond(_In_ int64_t pMilliSecond)
            {
                auto _elapsedTotalMS = get_elapsed_time().get_total_milliseconds();
                auto _totalTimeMS = get_duration_time().get_total_milliseconds();

                if (pMilliSecond == _elapsedTotalMS || pMilliSecond > _totalTimeMS) return 0;

                auto _frame = time_to_frame(pMilliSecond);
                return seek_frame(_frame);
            }

            int seek_frame(_In_ int64_t pFrame)
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
                w_thread::sleep_current_thread(0);

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
                      
			W_RESULT write_video_frame_to_buffer(
					wolf::system::w_memory_pool& pMemory,
					uint32_t pDownSamplingScale,
					bool pBGRA_or_RGBA)
			{
				const std::string _trace_info = this->_name + "::write_video_frame_to_buffer";

				W_RESULT _hr = W_PASSED;

				auto _width = get_video_frame_width();
				auto _height = get_video_frame_height();

				int _got_frame = 0;
				while (!_got_frame && av_read_frame(_av_format_ctx, this->_av_packet) >= 0)
				{
					// Is this a packet from the video stream
					if (this->_av_packet->stream_index == this->_video_stream_index)
					{
#pragma region read video frame
						auto _result = avcodec_decode_video2(this->_video_codec.avCodecCtx, this->_video_codec.avFrame, &_got_frame, this->_av_packet);
						if (_result < 0)
						{
							_hr = W_FAILED;
							V(_hr, w_log_type::W_ERROR, "decoding video frame. trace info: {}", _trace_info);
							return _hr;
						}

						if (this->_av_packet->dts == AV_NOPTS_VALUE &&
							this->_video_codec.avFrame->opaque && *(uint64_t*)this->_video_codec.avFrame->opaque != AV_NOPTS_VALUE)
						{
							this->_video_codec.pts = *(uint64_t *)this->_video_codec.avFrame->opaque;
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
						if (!_got_frame) continue;

						_update_clock(this->_video_codec);
						_update_time(this->_video_codec.clock);
#pragma endregion

#pragma region store av frame in to the memory buffer

						//Set the target convert properties
						auto _format = (AVPixelFormat)this->_video_codec.avFrame->format;

						auto _down_sample_width = _width;
						auto _down_sample_height = _height;

						bool _need_down_sampling = false;
						if (pDownSamplingScale > 1)
						{
							_need_down_sampling = true;
							_down_sample_width /= pDownSamplingScale;
							_down_sample_height /= pDownSamplingScale;
						}

						struct SwsContext* _sws_context = sws_getContext(
							_width,
							_height,
							_format,
							_down_sample_width,
							_down_sample_height,
							pBGRA_or_RGBA ? AV_PIX_FMT_BGRA : AV_PIX_FMT_RGBA,
							SWS_BICUBIC,
							NULL,
							NULL,
							NULL);

						W_RESULT _hr = W_FAILED;
						uint8_t* _frame = nullptr;
						size_t _write_size_in_bytes = 0;
						if (_need_down_sampling)
						{
							//Re-Scale it
							sws_scale(
								_sws_context,
								this->_video_codec.avFrame->data,
								this->_video_codec.avFrame->linesize,
								0,
								_height,
								this->_down_sample_avFrame->data,
								this->_down_sample_avFrame->linesize);

							//get frame data
							_frame = (uint8_t*)(this->_down_sample_avFrame->data[0]);
							_write_size_in_bytes = 4/*RGBA or BGRA*/ * _down_sample_width * _down_sample_height * sizeof(uint8_t);
						}
						else
						{
							sws_scale(
								_sws_context,
								this->_video_codec.avFrame->data,
								this->_video_codec.avFrame->linesize,
								0,
								_height,
								this->_av_frame->data,
								this->_av_frame->linesize);

							//get frame data
							_frame = (uint8_t*)(this->_av_frame->data[0]);
							_write_size_in_bytes = 4/*RGBA or BGRA*/ * _width * _height * sizeof(uint8_t);
						}

						auto _memory_chuck_size_in_bytes = pMemory.get_size_in_bytes();
						if (_write_size_in_bytes > _memory_chuck_size_in_bytes)
						{
							_hr = W_FAILED;
							V(_hr, "size of frame bytes is greater than size of memory pool. trace info: {}", _trace_info);
							break;
						}

						auto _memory = (uint8_t*)pMemory.get_start_ptr();
						if (_memory)
						{
							//first clear memory
							std::memcpy(&_memory[pMemory.write_index], &_frame[0], _write_size_in_bytes);
							pMemory.write_index = (pMemory.write_index + _write_size_in_bytes) % _memory_chuck_size_in_bytes;
						}
						_frame = nullptr;
						sws_freeContext(_sws_context);
#pragma endregion
					}

					//free the packet
					av_free_packet(this->_av_packet);
				}

				return _hr;
			}
//
//            W_RESULT buffer_audio_to_memory(wolf::system::w_memory& pAudioMemory, double& pAudioFrameVolumeDB)
//            {
//                W_RESULT hr = W_PASSED;
//
//                auto width = get_video_frame_width();
//                auto height = get_video_frame_height();
//
//                pAudioFrameVolumeDB = 0;
//                int gotFrame = 0;
//
//                //Start storing all audio untill we get first video frame
//                while (!gotFrame && av_read_frame(_av_format_ctx, this->_av_packet) >= 0)
//                {
//                    if (this->_av_packet->stream_index == this->_audio_stream_index)
//                    {
//#pragma region read audio buffer
//
//                        auto result = avcodec_decode_audio4(this->_audio_codec.avCodecCtx, this->_audio_codec.avFrame, &gotFrame, this->_av_packet);
//                        if (result < 0)
//                        {
//                            auto hr = W_FAILED;
//                            V(hr, L"decoding audio frame", this->_name);
//                            return hr;
//                        }
//
//                        if (this->_av_packet->dts == AV_NOPTS_VALUE && 
//                            this->_audio_codec.avFrame->opaque && 
//                            *(uint64_t*) this->_audio_codec.avFrame->opaque != AV_NOPTS_VALUE)
//                        {
//                            this->_audio_codec.pts = *(uint64_t *) this->_audio_codec.avFrame->opaque;
//                        }
//                        else if (this->_av_packet->dts != AV_NOPTS_VALUE)
//                        {
//                            this->_audio_codec.pts = this->_av_packet->dts;
//                        }
//                        else
//                        {
//                            this->_audio_codec.pts = 0;
//                        }
//                        this->_audio_codec.pts *= av_q2d(this->_audio_codec.avStream->time_base);
//
//                        if (!gotFrame) continue;
//
//                        _update_clock(this->_audio_codec);
//                        _update_time(this->_audio_codec.clock);
//
//                        auto _hr = pAudioMemory.Allocate(sizeof(w_audio_frame_data), std::alignment_of<w_audio_frame_data>::value);
//                        if (_hr)
//                        {
//                            auto _address = pAudioMemory.Get_address() - 1;
//                            auto _memory = pAudioMemory.Read(_address);
//
//                            auto _frame = new (_memory) w_audio_frame_data();
//                            if (_frame)
//                            {
//                                auto _dataPtr = &_frame->data[0];
//                                _frame->usedSize = static_cast<UINT>(swr_convert(
//                                    this->_audio_convert,
//                                    &_dataPtr,
//                                    AUDIO_FRAME_SIZE,
//                                    (const uint8_t **)this->_audio_codec.avFrame->data,
//                                    this->_audio_codec.avFrame->nb_samples));
//
//                                _frame->pts = this->_av_packet->pts;
//
//                                double RootMeanSquare = 0;
//                                for (size_t i = 0; i < _frame->usedSize; ++i)
//                                {
//                                    RootMeanSquare += std::pow(_dataPtr[i], 2);
//                                }
//                                RootMeanSquare = std::sqrt(RootMeanSquare / _frame->usedSize);
//                                //Log("RootMeanSquare : " + QString::number(RootMeanSquare));
//
//                                pAudioFrameVolumeDB = 20.000 * std::log10(RootMeanSquare / 32768);
//
//                            }
//                            else
//                            {
//                                logger.error(L"Could not cast allocated memory for audio");
//                                hr = W_FAILED;
//                            }
//                        }
//                        else
//                        {
//                            logger.error(L"Could not allocate memory for audio");
//                            hr = W_FAILED;
//                        }
//
//#pragma endregion
//                    }
//
//                    //free the packet
//                    av_free_packet(this->_av_packet);
//                }
//
//
//                return hr;
//            }
//
//            W_RESULT buffer_to_memory(wolf::system::w_memory& pVideoMemory, wolf::system::w_memory& pAudioMemory)
//            {
//                W_RESULT hr = W_PASSED;
//
//                auto width = get_video_frame_width();
//                auto height = get_video_frame_height();
//
//                int gotVideoFrame = 0;
//                int gotAudioFrame = 0;
//
//                //Start storing all audio untill we get first video frame
//                while (!gotVideoFrame && av_read_frame(_av_format_ctx, this->_av_packet) >= 0)
//                {
//                    // Is this a packet from the video stream
//                    if (this->_av_packet->stream_index == this->_video_stream_index)
//                    {
//#pragma region read video buffer
//
//                        auto result = avcodec_decode_video2(this->_video_codec.avCodecCtx, this->_video_codec.avFrame, &gotVideoFrame, this->_av_packet);
//                        if (result < 0)
//                        {
//                            hr = W_FAILED;
//                            V(hr, L"decoding video frame", this->_name);
//                            return hr;
//                        }
//
//                        if (this->_av_packet->dts == AV_NOPTS_VALUE && this->_video_codec.avFrame->opaque && *(uint64_t*) this->_video_codec.avFrame->opaque != AV_NOPTS_VALUE)
//                        {
//                            this->_video_codec.pts = *(uint64_t *) this->_video_codec.avFrame->opaque;
//                        }
//                        else if (this->_av_packet->dts != AV_NOPTS_VALUE)
//                        {
//                            this->_video_codec.pts = _av_packet->dts;
//                        }
//                        else
//                        {
//                            this->_video_codec.pts = 0;
//                        }
//                        this->_video_codec.pts *= av_q2d(this->_video_codec.avStream->time_base);
//
//                        if (!gotVideoFrame) continue;
//
//                        _update_clock(this->_video_codec);
//                        _update_time(this->_video_codec.clock);
//
//#pragma region Re-Scale and store av frame in to the DirectX format
//
//                        //Set the target convert properties
//                        auto format = (AVPixelFormat)this->_video_codec.avFrame->format;
//
//                        struct SwsContext* swsContext = sws_getContext(
//                            width,
//                            height,
//                            format,
//                            width,
//                            height,
//                            AV_PIX_FMT_BGRA,
//                            SWS_BICUBIC,
//                            NULL,
//                            NULL,
//                            NULL);
//
//                        //Re-Scale it
//                        sws_scale(
//                            swsContext,
//                            this->_video_codec.avFrame->data,
//                            this->_video_codec.avFrame->linesize,
//                            0,
//                            height,
//                            this->_av_frame->data,
//                            this->_av_frame->linesize);
//
//                        const size_t _rowPitch = width * 4; // 4 bytes of RGBA
//                        const size_t _rowCount = height;
//                        const size_t _slicePitch = _rowPitch * _rowCount;
//
//                        //copy data to the destination
//                        auto _frame = (uint8_t*)(this->_av_frame->data[0]);
//                        if (_frame)
//                        {
//                            auto _size = width * height;
//                            auto _hr = pVideoMemory.Allocate(sizeof(int) * _size,
//                                std::alignment_of<int>::value);
//                            if (_hr)
//                            {
//                                auto _address = pVideoMemory.Get_address() - 1;
//                                auto _memory = pVideoMemory.Read(_address);
//
//                                auto _videoFrameData = static_cast<int*>(_memory);
//                                if (_videoFrameData)
//                                {
//                                    //first clear memory
//                                    std::memset(&_videoFrameData, 0, _size);
//                                    tbb::parallel_for<size_t>(0, _slicePitch, 4, [&](size_t i)
//                                    {
//                                        _videoFrameData[i / 4] = (int)((_frame[i + 3] & 0xFF) << 24) | ((_frame[i] & 0xFF) << 16) | ((_frame[i + 1] & 0xFF) << 8) | (_frame[i + 2] & 0xFF);//argb
//                                    });
//                                }
//                                else
//                                {
//                                    logger.error(L"Could not cast allocated memory for video");
//                                    hr = W_FAILED;
//                                }
//
//                            }
//                            else
//                            {
//                                logger.error(L"Could not allocate memory for video");
//                                hr = W_FAILED;
//                            }
//
//                            _frame = nullptr;
//                        }
//
//                        sws_freeContext(swsContext);
//
//#pragma endregion
//
//#pragma endregion
//                    }
//                    else if (this->_av_packet->stream_index == this->_audio_stream_index)
//                    {
//#pragma region read audio buffer
//
//                        auto result = avcodec_decode_audio4(_audio_codec.avCodecCtx, _audio_codec.avFrame, &gotAudioFrame, _av_packet);
//                        if (result < 0)
//                        {
//                            auto hr = W_FAILED;
//                            V(hr, L"decoding audio frame", this->_name);
//                            return hr;
//                        }
//
//                        if (_av_packet->dts == AV_NOPTS_VALUE && 
//                            this->_audio_codec.avFrame->opaque && 
//                            *(uint64_t*) this->_audio_codec.avFrame->opaque != AV_NOPTS_VALUE)
//                        {
//                            this->_audio_codec.pts = *(uint64_t *) this->_audio_codec.avFrame->opaque;
//                        }
//                        else if (_av_packet->dts != AV_NOPTS_VALUE)
//                        {
//                            this->_audio_codec.pts = _av_packet->dts;
//                        }
//                        else
//                        {
//                            this->_audio_codec.pts = 0;
//                        }
//                        this->_audio_codec.pts *= av_q2d(this->_audio_codec.avStream->time_base);
//
//                        if (!gotAudioFrame) continue;
//
//                        _update_clock(this->_audio_codec);
//                        _update_time(this->_audio_codec.clock);
//
//                        auto _hr = pAudioMemory.Allocate(sizeof(w_audio_frame_data), std::alignment_of<w_audio_frame_data>::value);
//                        if (_hr)
//                        {
//                            auto _address = pAudioMemory.Get_address() - 1;
//                            auto _memory = pAudioMemory.Read(_address);
//
//                            auto _frame = new (_memory) w_audio_frame_data();
//                            if (_frame)
//                            {
//                                auto _dataPtr = &_frame->data[0];
//                                _frame->usedSize = swr_convert(
//                                    this->_audio_convert,
//                                    &_dataPtr,
//                                    AUDIO_FRAME_SIZE,
//                                    (const uint8_t **)_audio_codec.avFrame->data,
//                                    _audio_codec.avFrame->nb_samples);
//                            }
//                            else
//                            {
//                                logger.error("Could not cast allocated memory for audio");
//                                hr = W_FAILED;
//                            }
//                        }
//                        else
//                        {
//                            logger.error("Could not allocate memory for audio");
//                            hr = W_FAILED;
//                        }
//
//#pragma endregion
//                    }
//
//                    //free the packet
//                    av_free_packet(this->_av_packet);
//                }
//
//                return hr;
//            }
//             
            ULONG release_media()
            {
                this->_media_full_path.clear();

                if (this->_video_codec.avCodecCtx)
                {
                    avcodec_close(_video_codec.avCodecCtx);
                }
                if (this->_video_codec.avFrame)
                {
                    av_free(this->_video_codec.avFrame);
                }

                this->_audio_frame_volume_db = 0;
                if (this->_audio_codec.avCodecCtx)
                {
                    avcodec_close(_audio_codec.avCodecCtx);
                }
                if (this->_audio_codec.avFrame)
                {
                    av_free(this->_audio_codec.avFrame);
                }
                
                //release av frame
                if (this->_av_frame)
                {
                    auto getdstFrame = this->_av_frame.get();
                    av_frame_free(&getdstFrame);
                    this->_av_frame.release();
                }

                if (this->_down_sample_avFrame)
                {
                    auto getdstFrame = this->_down_sample_avFrame.get();
                    av_frame_free(&getdstFrame);
                    this->_down_sample_avFrame.release();
                }

                //free the packet
                if (this->_av_packet)
                {
                    av_free_packet(this->_av_packet);
                }

                //free the video format context
                if (this->_av_format_ctx)
                {
                    avformat_close_input(&this->_av_format_ctx);
                }

                //free swr
                if (this->_audio_convert)
                {
                    swr_free(&this->_audio_convert);
                }

                return 0;
            }

			ULONG release_output_stream_server()
			{
				//release objects related to stream
				if (this->_stream_out)
				{
					avcodec_close(this->_stream_out->codec);
				}
				if (this->_stream_out_frame)
				{
					av_frame_free(&this->_stream_out_frame);
				}
				if (this->_stream_out_ctx)
				{
					avformat_free_context(this->_stream_out_ctx);
				}

				return 0;
			}

#pragma region Getters

            bool is_open() const
            {
                return this->_is_media_open;
            }

            const char* get_media_path() const
            {
                return this->_media_full_path.c_str();
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
            W_RESULT _copy_audio_frame_to(uint8_t* pBuffer, int& pBufferSize)
            {
                if (this->_audio_stream_index <= -1) return W_PASSED;

				const std::string _trace_info = this->_name + "::_copy_audio_frame_to";

                int frameFinished = 0;
                auto result = avcodec_decode_audio4(this->_audio_codec.avCodecCtx, this->_audio_codec.avFrame, &frameFinished, _av_packet);
                if (result < 0)
                {
                    auto hr = W_FAILED;
                    V(hr, "decoding audio frame. trace info: {}", _trace_info);
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

                if (!frameFinished) return W_FAILED;

                _update_clock(this->_audio_codec);
                _update_time(this->_audio_codec.clock);

                //pBufferSize = swr_convert(
                //    this->_audio_convert,
                //    &pBuffer,
                //    AUDIO_FRAME_SIZE,
                //    (const uint8_t **)this->_audio_codec.avFrame->data,
                //    this->_audio_codec.avFrame->nb_samples);

                return W_PASSED;
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
                this->_elapsed_video_frames = static_cast<uint8_t>(((float)this->_elapsed_time.get_milliseconds() * this->_frame_rate) / 1000.0);
                if (this->_elapsed_time == this->_duration_time)
                {
                    this->_remained_video_frames = 0;
                }
                else
                {
                    this->_remained_video_frames = static_cast<uint8_t>(this->_frame_rate - (float)this->_elapsed_video_frames);
                }
            }

            std::string                                 _name;

            std::unique_ptr<AVFrame>					_av_frame;
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
            std::string									_media_full_path;

            wolf::system::w_time_span					_duration_time;
            wolf::system::w_time_span					_elapsed_time;
            wolf::system::w_time_span					_remained_time;

            size_t										_total_video_frames;
            uint8_t										_duration_video_frames;
            uint8_t										_elapsed_video_frames;
            uint8_t										_remained_video_frames;

            float										_frame_rate;
            double										_audio_frame_volume_db;

            AVStream*                                   _stream_out;
			AVFrame*                                    _stream_out_frame;
            AVFormatContext*                            _stream_out_ctx;

			double                                      _frame_max_delay_in_ms;
        };
    }
}

using namespace wolf::framework;

w_media_core::w_media_core() :_pimp(new w_media_core_pimp())
{
    _super::set_class_name("w_media_core");
}

w_media_core::~w_media_core()
{
	release();
}

void w_media_core::register_all(_In_ const bool& pRegisterNetwork)
{
	std::call_once(once_initialize_flag, [pRegisterNetwork]()
	{
		// Register all formats and codec(s)
		av_register_all();
        avcodec_register_all();
		if (pRegisterNetwork)
		{
			avformat_network_init();
		}

#ifdef __WIN32
		MFStartup(MF_VERSION);
#endif
	});
}

W_RESULT w_media_core::open_media(_In_z_ std::wstring pMediaPath, _In_ int64_t pSeekToFrame)
{
    return this->_pimp ? this->_pimp->open_media(pMediaPath, pSeekToFrame) : W_FAILED;
}

W_RESULT w_media_core::open_stream_server(
	_In_z_ const char* pURL,
	_In_z_ const char* pProtocol,
	_In_z_ const char* pFormatName,
	_In_ const AVCodecID& pCodecID,
	_In_ const int64_t& pFrameRate,
	_In_ const AVPixelFormat& pPixelFormat,
	_In_ const uint32_t& pWidth,
	_In_ const uint32_t& pHeight,
	_In_ w_signal<void(const w_stream_connection_info&)>& pOnConnectionEstablished,
	_In_ w_signal<void(const w_stream_frame_info&)>& pOnFillingVideoFrameBuffer,
	_In_ w_signal<void(const char*)>& pOnConnectionLost)
{
	if (!this->_pimp) return W_FAILED;

	return this->_pimp->open_stream_server(
		pURL,
		pProtocol,
		pFormatName,
		pCodecID,
		pFrameRate,
		pPixelFormat,
		pWidth,
		pHeight,
		pOnConnectionEstablished,
		pOnFillingVideoFrameBuffer,
		pOnConnectionLost);
}

W_RESULT w_media_core::open_stream_client(
	_In_z_ const char* pURL,
	_In_z_ const char* pProtocol,
	_In_z_ const char* pFormatName,
	_In_ const AVCodecID& pCodecID,
	_In_ const AVPixelFormat& pPixelFormat,
	_In_ const uint32_t& pWidth,
	_In_ const uint32_t& pHeight,
	_In_ const long long& pTimeOut,
	_In_ w_signal<void(const w_stream_connection_info&)>& pOnConnectionEstablished,
	_In_ w_signal<void(const w_stream_frame_info&)>& pOnGettingStreamVideoFrame,
	_In_ w_signal<void(const char*)>& pOnConnectionLost,
	_In_ w_signal<void(const char*)>& pOnConnectionClosed)
{
	if (!this->_pimp) return W_FAILED;

	return this->_pimp->open_stream_client(
		pURL,
		pProtocol,
		pFormatName,
		pCodecID,
		pPixelFormat,
		pWidth,
		pHeight,
		pTimeOut,
		pOnConnectionEstablished,
		pOnGettingStreamVideoFrame,
		pOnConnectionLost,
		pOnConnectionClosed);
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

W_RESULT w_media_core::write_video_frame_to_buffer(
	wolf::system::w_memory_pool& pMemory,
	uint32_t pDownSamplingScale,
	bool pBGRA_or_RGBA)
{
	return this->_pimp ? this->_pimp->write_video_frame_to_buffer(pMemory, pDownSamplingScale, pBGRA_or_RGBA) : W_FAILED;
}

//W_RESULT w_media_core::buffer_audio_to_memory(w_memory& pAudioMemory, double& pAudioFrameVolumeDB)
//{
//    return this->_pimp ? this->_pimp->buffer_audio_to_memory(pAudioMemory, pAudioFrameVolumeDB) : W_FAILED;
//}
//
//W_RESULT w_media_core::buffer_to_memory(w_memory& pVideoMemory, w_memory& pAudioMemory)
//{
//    return this->_pimp ? this->_pimp->buffer_to_memory(pVideoMemory, pAudioMemory) : W_FAILED;
//}

ULONG w_media_core::release()
{
    if (this->get_is_released()) return 1;
    this->_pimp->release_media();
    return _super::release();
}

ULONG w_media_core::release_media()
{
    return this->_pimp ? this->_pimp->release_media() : 1;
}

ULONG w_media_core::release_output_stream_server()
{
	return this->_pimp ? this->_pimp->release_output_stream_server() : 1;
}

void w_media_core::shut_down()
{
#ifdef __WIN32
	std::call_once(once_release_flag, []()
	{
		MFShutdown();
	});
#endif
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
