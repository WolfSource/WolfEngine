#include "w_media_core_pch.h"
#include "w_media_core.h"
#include <io.h>
#include <wrl/client.h>
#include <iomanip>      // For std::hex
#include <codecvt>      // For Unicode conversions

using namespace wolf::system;
using namespace wolf::framework;
using namespace Microsoft::WRL;

static const MFTIME MF_ONE_SEC = 10000000;
static const LONG MF_ONE_MSEC = 1000;
static std::once_flag onceInit;
static std::once_flag onceRelease;


w_ffmpeg::w_ffmpeg() : isMediaOpen(false), isReleased(false), avFormatCtx(nullptr), avPacket(nullptr), audioConvert(nullptr)
{
	this->name = "ffmpeg";// setClassName(typeid(this).name());

	std::memset(&this->videoCodec, 0, sizeof(Codec));
	std::memset(&this->audioCodec, 0, sizeof(Codec));

#if defined(__1080p_50__)
	this->frameRate = 50;
#elif defined(__1080p_25__) || defined(__PAL__)
	this->frameRate = 25;
#elif defined(__NTSC__)
	this->frameRate = 29.97f;
#endif
}

w_ffmpeg::~w_ffmpeg()
{
	release();
}

void w_ffmpeg::initialize_MF()
{
	std::call_once(onceInit, []()
	{
		// Register all formats and codec(s)
		av_register_all();
		//avformat_network_init();
		MFStartup(MF_VERSION);
	});
}

HRESULT w_ffmpeg::open_media(std::wstring pMediaPath, int64_t pSeekToFrame, bool pLog4User)
{
	//Todo : Later check we must change the auto mode, this will uncomment on that situation
	///////*if (this->GetMediaPath() == pMediaPath)
	//////{
	//////	this->isMediaOpen = true;
	//////	return;
	//////}*/

	if (pMediaPath.empty()) return S_FALSE;

	release();
	
	this->isReleased = false;

	std::memset(&this->videoCodec, 0, sizeof(Codec));
	std::memset(&this->audioCodec, 0, sizeof(Codec));

	//Initialize the packet
	this->avPacket = new AVPacket();
	av_init_packet(this->avPacket);

	this->audioConvert = swr_alloc();
	this->avFormatCtx = avformat_alloc_context();

	this->isMediaOpen = false;
	{
		this->fullPath = wolf::system::convert::to_utf8(pMediaPath).c_str();

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
		if (avformat_open_input(&avFormatCtx, this->fullPath.c_str(), NULL, NULL) != 0)
		{
			hr = S_FALSE;
			V(hr, L"openning file", this->name);
			release();
			return hr;
		}

		// Retrieve stream information
		if (avformat_find_stream_info(avFormatCtx, NULL) < 0)
		{
			hr = S_FALSE;
			V(hr, L"finding file stream information", this->name);
			release();
			return hr;
		}



#ifdef _DEBUG
		// Dump information about file onto standard error
		//	av_dump_format(avFormatCtx, 0, path, 0);
#endif

		// Find the first video & audio stream
		this->videoStreamIndex = av_find_best_stream(avFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
		this->audioStreamIndex = av_find_best_stream(avFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);

		auto hasAudioStream = true;
		auto hasVideoStream = true;

		if (this->videoStreamIndex <= -1)
		{
			hr = S_FALSE;
			hasVideoStream = false;
			if (pLog4User)
			{
				V(hr, L"finding stream of video", this->name);
				logger.user(L"Media file does not have video frames");
			}
		}
		if (this->audioStreamIndex <= -1)
		{
			hr = S_FALSE;
			hasAudioStream = false;
			if (pLog4User)
			{
				V(hr, L"finding stream of audio", this->name);
				logger.user(L"Media file does not have audio frames");
			}
		}

		if (!hasVideoStream && !hasAudioStream)
		{
			logger.user("File " + this->fullPath + " did not load");
			release();
			return S_FALSE;
		}

		//Get a pointer to the stream of video & audio 
		//Get a pointer to the codec context of video & audio 
		if (hasVideoStream)
		{
			videoCodec.avStream = avFormatCtx->streams[this->videoStreamIndex];
			videoCodec.avCodecCtx = avFormatCtx->streams[this->videoStreamIndex]->codec;
		}

		if (hasAudioStream)
		{
			audioCodec.avStream = avFormatCtx->streams[this->audioStreamIndex];
			audioCodec.avCodecCtx = avFormatCtx->streams[this->audioStreamIndex]->codec;
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
		ComPtr<IUnknown> pSource = nullptr;
		ComPtr<IMFSourceResolver> sourceResolver = NULL;
		ComPtr<IMFMediaSource> source;

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

		ComPtr<IMFPresentationDescriptor> pPD = nullptr;
		hr = source->CreatePresentationDescriptor(&pPD);
		if (FAILED(hr))
		{
			logger.write(L"creating presentation descriptor");
		}
		auto pDuration = new MFTIME(0);
		pPD->GetUINT64(MF_PD_DURATION, (UINT64*) pDuration);
		auto duration = (LONG) (*pDuration / (MF_ONE_SEC / MF_ONE_MSEC));
		pDuration = nullptr;

		//Release 
		COMPTR_RELEASE(pPD);
		COMPTR_RELEASE(pSource);
		COMPTR_RELEASE(sourceResolver);

#pragma endregion


		this->durationTime = w_time_span::from_milliseconds(duration);
		this->remainedTime = this->durationTime;
		this->elapsedTime = w_time_span::zero();

		if (hasVideoStream)
		{
			AVRational avr = videoCodec.avCodecCtx->time_base;
			auto ticksPerFrame = videoCodec.avCodecCtx->ticks_per_frame;
			this->frameRate = static_cast<float>(avr.den) / static_cast<float>(avr.num * videoCodec.avCodecCtx->ticks_per_frame);
		}
		else if (hasAudioStream)
		{
			AVRational avr = audioCodec.avCodecCtx->time_base;
			auto ticksPerFrame = audioCodec.avCodecCtx->ticks_per_frame;
			this->frameRate = static_cast<float>(avr.den) / static_cast<float>(avr.num * audioCodec.avCodecCtx->ticks_per_frame);
		}

		//Set total frames
		this->durationVideoFrames = static_cast<byte>(std::round(((double)this->durationTime.get_milliseconds() * this->frameRate) / 1000.0));
		this->remainedVideoFrames = this->durationVideoFrames;
		this->elapsedVideoFrames = 0;

		this->totalVideoFrames = static_cast<size_t>(((double)this->durationTime.get_total_milliseconds() * this->frameRate) / 1000.0);
		//Find the decoder for the video and audio and also allocate frames for video & audio

		if (hasVideoStream)
		{
			videoCodec.avCodec = avcodec_find_decoder(videoCodec.avCodecCtx->codec_id);
			if (videoCodec.avCodec == NULL)
			{
				hr = S_FALSE;
				V(hr, L"unsupported codec for video", this->name);
				release();
				return hr;
			}

			//Open codec
			if (avcodec_open2(videoCodec.avCodecCtx, videoCodec.avCodec, NULL) < 0)
			{
				hr = S_FALSE;
				V(hr, L"could not open video codec", this->name);
				release();
				return hr;
			}

			videoCodec.avFrame = av_frame_alloc();//avcodec_alloc_frame deprecated function

			auto videoBitRater = get_bit_rate();
			auto videoChannels = get_video_channels();
			auto videoSampleRate = get_video_sample_rate();

			auto width = get_video_width();
			auto height = get_video_height();

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
			audioCodec.avCodec = avcodec_find_decoder(audioCodec.avCodecCtx->codec_id);
			if (audioCodec.avCodec == NULL)
			{
				hr = S_FALSE;
				V(hr, L"unsupported codec for audio", this->name);
				release();
				return hr;
			}

			if (avcodec_open2(audioCodec.avCodecCtx, audioCodec.avCodec, NULL) < 0)
			{
				hr = S_FALSE;
				V(hr, L"could not open audio codec", this->name);
				release();
				return hr;
			}
			audioCodec.avFrame = av_frame_alloc();//avcodec_alloc_frame deprecated function

			auto audioChannels = get_audio_channels();
			auto audioSampleRate = get_audio_sample_rate();

			AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
			uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO;
			int64_t audio_in_channel_layout = av_get_default_channel_layout(audioChannels);
			this->audio_out_channels_layout = av_get_channel_layout_nb_channels(out_channel_layout);

			//Create swr for converting audio
			this->audioConvert = swr_alloc_set_opts(
				audioConvert,
				out_channel_layout,
				out_sample_fmt,
				audioSampleRate,
				audio_in_channel_layout,
				audioCodec.avCodecCtx->sample_fmt,
				audioSampleRate,
				0,
				NULL);
			swr_init(this->audioConvert);
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
				msg = L"The media decoded successfully with the following information : time: " + this->durationTime.to_wstring() + L" , fps: " + std::to_wstring(this->frameRate);
				logger.user(msg);
			}

			msg.clear();
		}

		pMediaPath.clear();
		this->isMediaOpen = true;
	}
	return S_OK;
}

//HRESULT W_ffmpeg::SeekTo(double pMilliSecond)
//{
//	auto hr = S_OK;
//
//	auto elapsedTotalMS = GetElapsedTime().GetTotalMilliseconds();
//	auto totalMS = GetDurationTime().GetTotalMilliseconds();
//
//	if (pMilliSecond == elapsedTotalMS || pMilliSecond < 0) return hr;
//
//	int flags = AVSEEK_FLAG_ANY;
//	int seek_ts = 0;
//
//	//Wait one cycle
//	Sleep(0);
//
//	//Seek the video
//	if (this->videoStreamIndex > -1)
//	{
//		auto videoStream = this->videoCodec.avStream;
//		seek_ts = (pMilliSecond * (videoStream->time_base.den)) / (videoStream->time_base.num * 1000);
//		if (av_seek_frame(avFormatCtx, this->videoStreamIndex, seek_ts, flags) < 0)
//		{
//			hr = S_FALSE;
//			OnFailed(hr, "seeking video to the " + std::to_string(pMilliSecond) + " ms time", this->Name);
//			return hr;
//		}
//		//UpdateClock(this->videoCodec);
//		UpdateTime(this->videoCodec.clock);
//
//		this->remainedVideoFrames = this->durationVideoFrames;
//		this->elapsedVideoFrames = 0;
//
//	}
//
//
//	//Seek the audio
//	if (this->audioStreamIndex > -1)
//	{
//		auto audioStream = this->audioCodec.avStream;
//		seek_ts = (pMilliSecond * (audioStream->time_base.den)) / (audioStream->time_base.num * 1000);
//		if (av_seek_frame(avFormatCtx, this->audioStreamIndex, seek_ts, flags) < 0)
//		{
//			hr = S_FALSE;
//			OnFailed(hr, "seeking audio to the " + std::to_string(pMilliSecond) + " time", this->Name);
//			return hr;
//		}
//
//		UpdateTime(this->audioCodec.clock);
//	}
//
//
//	//Sleep(10);
//
//	return hr;
//}

int64_t w_ffmpeg::time_to_frame(int64_t pMilliSecond)
{
	auto _videoStream = this->videoCodec.avStream;
	auto _frameNumber = av_rescale(pMilliSecond, _videoStream->time_base.den, _videoStream->time_base.num) / 1000;
	return _frameNumber;
}

int w_ffmpeg::seek_frame_milliSecond(int64_t pMilliSecond)
{
	auto _elapsedTotalMS = get_elapsed_time().get_total_milliseconds();
	auto _totalTimeMS = get_duration_time().get_total_milliseconds();

	if (pMilliSecond == _elapsedTotalMS || pMilliSecond > _totalTimeMS) return 0;

	auto _frame = time_to_frame(pMilliSecond);
	return seek_frame(_frame);
}

int w_ffmpeg::seek_frame(int64_t pFrame)
{
	if (pFrame <0) return 0;

	int _hr = 1;
	bool _clockUpdated = false;
	//Seek the video
	if (this->videoStreamIndex > -1)
	{
		auto _videoStream = this->videoCodec.avStream;
		if (avformat_seek_file(avFormatCtx, this->videoStreamIndex, 0, pFrame, pFrame, AVSEEK_FLAG_FRAME) >= 0)
		{
			avcodec_flush_buffers(this->videoCodec.avCodecCtx);
		}
		else
		{
			_hr = -1;
		}

		_update_time(this->videoCodec.clock);
		_clockUpdated = true;
	}

	//Wait one cycle
	Sleep(0);

	//Seek the audio
	if (this->videoStreamIndex > -1)
	{
		auto _videoStream = this->videoCodec.avStream;
		if (avformat_seek_file(avFormatCtx, this->videoStreamIndex, 0, pFrame, pFrame, AVSEEK_FLAG_FRAME) >= 0)
		{
			avcodec_flush_buffers(this->videoCodec.avCodecCtx);
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
			_update_time(this->audioCodec.clock);
		}
	}

	return _hr;
}


//int w_ffmpeg::get_video_frame(_In_ ID3D11DeviceContext1* pContext, _Inout_ ID3D11Resource* pResource
//#ifdef __QT_DLL__
//	, bool pCopyToDirectXBuffer,
//	QImage& pQImage, bool pStoreInQImage, int pQTDownSampleScale
//#endif
//	)
//{
//	auto hr = 1;
//	//We need the first video frame from current time
//	while (true)
//	{
//		//Todo:Error handling
//		auto frameRead = av_read_frame(avFormatCtx, avPacket);
//		if (frameRead < 0)
//		{
//			//Bad end of file
//			hr = 0;
//			break;
//		}
//		if (avPacket->stream_index == this->videoStreamIndex)
//		{
//			auto hr_False = std::make_unique<HRESULT>(S_FALSE);
//			auto _hr = _copy_video_frame_to(
//				pContext,
//				pResource
//#ifdef __QT_DLL__
//				,
//				pCopyToDirectXBuffer,
//				pQImage,
//				pStoreInQImage,
//				pQTDownSampleScale
//#endif
//				);
//			av_free_packet(avPacket);
//
//			if (_hr == NULL)
//			{
//				continue;
//			}
//			if (_hr == hr_False.get())
//			{
//				V(S_FALSE, L"decoding video frame", this->name);
//				hr = -1;
//			}
//
//			break;
//		}
//		//free the packet
//		av_free_packet(avPacket);
//	}
//
//	return hr;
//}

int w_ffmpeg::get_audio_frame(uint8_t* pBuffer, int& pBufferSize)
{
	this->audioframeVolumeDB = 0;
	pBufferSize = 0;
	auto hr = 1;

	if (this->audioStreamIndex <= -1) return hr;

	while (true)
	{
		auto frameRead = av_read_frame(avFormatCtx, avPacket);
		if (frameRead < 0)
		{
			//Bad end of file
			hr = 0;
			break;
		}
		if (avPacket->stream_index == this->audioStreamIndex)
		{
			if (_copy_audio_frame_to(pBuffer, pBufferSize) == S_OK)
			{
				//Calculate the audio volume in db per this frame
				double RootMeanSquare = 0;
				for (size_t i = 0; i < pBufferSize; ++i)
				{
					RootMeanSquare += std::pow(pBuffer[i], 2);
				}
				RootMeanSquare = std::sqrt(RootMeanSquare / pBufferSize);
				//Log("RootMeanSquare : " + QString::number(RootMeanSquare));

				this->audioframeVolumeDB = 20.000 * std::log10(RootMeanSquare / 32768);
				//Log("volume(db): " + QString::number(this->audioframeVolumeDB));
			}
			else
			{
				V(S_FALSE, L"decoding audio frame", this->name);
				hr = -1;
			}

			av_free_packet(avPacket);
			break;
		}
		//free the packet
		av_free_packet(avPacket);
	}
	return hr;
}

//HRESULT* w_ffmpeg::_copy_video_frame_to(_In_ ID3D11DeviceContext1* pContext, _Inout_ ID3D11Resource* pResource
//#ifdef __QT_DLL__
//	, bool pCopyToDirectXBuffer,
//	QImage& pQImage, bool pStoreInQImage, int pQTDownSampleScale
//#endif
//	)
//{
//	auto hr = S_OK;
//
//	if (this->videoStreamIndex <= -1) return &hr;
//
//	int frameFinished = 0;
//
//	auto result = avcodec_decode_video2(this->videoCodec.avCodecCtx, this->videoCodec.avFrame, &frameFinished, this->avPacket);
//	if (result < 0)
//	{
//		hr = S_FALSE;
//		V(hr, L"decoding video frame", this->name);
//		return &hr;
//	}
//
//	if (avPacket->dts == AV_NOPTS_VALUE && this->videoCodec.avFrame->opaque && *(uint64_t*) this->videoCodec.avFrame->opaque != AV_NOPTS_VALUE)
//	{
//		this->videoCodec.pts = *(uint64_t *) this->videoCodec.avFrame->opaque;
//	}
//	else if (avPacket->dts != AV_NOPTS_VALUE)
//	{
//		this->videoCodec.pts = avPacket->dts;
//	}
//	else
//	{
//		this->videoCodec.pts = 0;
//	}
//	this->videoCodec.pts *= av_q2d(this->videoCodec.avStream->time_base);
//
//	if (!frameFinished) return nullptr;
//
//	_update_clock(this->videoCodec);
//	_update_time(this->videoCodec.clock);
//
//	//Set the target convert properties
//	auto format = (AVPixelFormat) videoCodec.avFrame->format;
//
//	auto width = get_video_width();
//	auto height = get_video_height();
//
//#ifdef __QT_DLL__
//	//If we need to store data in qt image too
//	if (pStoreInQImage)
//	{
//#pragma region Store av frame in to the qt image
//
//		auto downSampleWidth = width;
//		auto downSampleHeight = height;
//
//		if (pQTDownSampleScale > 1)
//		{
//			downSampleWidth /= pQTDownSampleScale;
//			downSampleHeight /= pQTDownSampleScale;
//		}
//		else
//		{
//			hr = S_FALSE;
//			V(hr, L"unsupported down sample value for re-scaling video frame", this->name);
//			return &hr;
//		}
//
//		//check qImage
//		if (pQImage.isNull())
//		{
//			pQImage = QImage(downSampleWidth, downSampleHeight, QImage::Format_RGB32);
//		}
//		else if (pQImage.width() != downSampleWidth || pQImage.height() != downSampleHeight)
//		{
//			pQImage = QImage(downSampleWidth, downSampleHeight, QImage::Format_RGB32);
//		}
//
//		//We need BGR for QT image
//		struct SwsContext* swsContext = sws_getContext(
//			width,
//			height,
//			format,
//			downSampleWidth,
//			downSampleHeight,
//			AV_PIX_FMT_BGR24,
//			SWS_BICUBIC,
//			NULL,
//			NULL,
//			NULL);
//
//		//Re-Scale it
//		sws_scale(
//			swsContext,
//			videoCodec.avFrame->data,
//			videoCodec.avFrame->linesize,
//			0,
//			height,
//			this->_av_Frame->data,
//			this->_av_Frame->linesize);
//
//		auto src = (uint8_t *) (this->_av_Frame->data[0]);
//		for (int j = 0; j < downSampleHeight; j++)
//		{
//			auto scanLine = (QRgb *) pQImage.scanLine(j);
//			for (size_t i = 0; i < downSampleWidth; i += 1)
//			{
//				auto index = i * 3;
//				scanLine[i] = qRgb(src[index + 2], src[index + 1], src[index]);
//			}
//			src += this->_av_Frame->linesize[0];
//		}
//
//		//Release
//		src = nullptr;
//		sws_freeContext(swsContext);
//
//#pragma endregion
//	}
//
//#endif
//
//	if (pResource == nullptr)
//	{
//		logger.warning("Resource is null");
//		hr = S_FALSE;
//		return &hr;
//	}
//
//#pragma region Store av frame in to the DirectX texture
//
//	//Copy av frame data to the directX texture
//	const size_t rowPitch = width * 4; // 4 bytes of RGBA
//	const size_t rowCount = height;
//	const size_t slicePitch = rowPitch * rowCount;
//
//	struct SwsContext* swsContext = sws_getContext(
//		width,
//		height,
//		format,
//		width,
//		height,
//		AV_PIX_FMT_BGRA,
//		SWS_BICUBIC,
//		NULL,
//		NULL,
//		NULL);
//
//	//Re-Scale it
//	sws_scale(
//		swsContext,
//		videoCodec.avFrame->data,
//		videoCodec.avFrame->linesize,
//		0,
//		height,
//		this->_av_Frame->data,
//		this->_av_Frame->linesize);
//
//	D3D11_MAPPED_SUBRESOURCE mapsub;
//	ZeroMemory(&mapsub, sizeof(D3D11_MAPPED_SUBRESOURCE));
//	pContext->Map(pResource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapsub);
//	{
//		auto data = static_cast<uint8_t*>(mapsub.pData);
//		if (!data)
//		{
//			hr = S_FALSE;
//			pContext->Unmap(pResource, 0);
//			V(hr, L"E_POINTER on getting mapped pixels", this->name);
//			return &hr;
//		}
//
//		auto src = (uint8_t *) (this->_av_Frame->data[0]);
//		size_t msize = std::fmin<size_t>(rowPitch, mapsub.RowPitch);
//		for (size_t h = 0; h < rowCount; ++h)
//		{
//			memcpy_s(data, rowPitch, src, msize);
//			data += mapsub.RowPitch;
//			src += rowPitch;
//		}
//
//		pContext->Unmap(pResource, 0);
//		src = nullptr;
//		data = nullptr;
//	}
//
//	sws_freeContext(swsContext);
//
//#pragma endregion
//
//	return &hr;
//}

HRESULT w_ffmpeg::_copy_audio_frame_to(uint8_t* pBuffer, int& pBufferSize)
{
	if (this->audioStreamIndex <= -1) return S_OK;

	int frameFinished = 0;
	auto result = avcodec_decode_audio4(audioCodec.avCodecCtx, audioCodec.avFrame, &frameFinished, avPacket);
	if (result < 0)
	{
		auto hr = S_FALSE;
		V(hr, L"decoding audio frame", this->name);
		return hr;
	}

	if (avPacket->dts == AV_NOPTS_VALUE && this->audioCodec.avFrame->opaque && *(uint64_t*) this->audioCodec.avFrame->opaque != AV_NOPTS_VALUE)
	{
		this->audioCodec.pts = *(uint64_t *) this->audioCodec.avFrame->opaque;
	}
	else if (avPacket->dts != AV_NOPTS_VALUE)
	{
		this->audioCodec.pts = avPacket->dts;
	}
	else
	{
		this->audioCodec.pts = 0;
	}
	this->audioCodec.pts *= av_q2d(this->audioCodec.avStream->time_base);

	if (!frameFinished) return S_FALSE;

	_update_clock(this->audioCodec);
	_update_time(this->audioCodec.clock);

	pBufferSize = swr_convert(
		this->audioConvert,
		&pBuffer,
		AUDIO_FRAME_SIZE,
		(const uint8_t **) audioCodec.avFrame->data,
		audioCodec.avFrame->nb_samples);

	return S_OK;
}

//w_ffmpeg::PacketType w_ffmpeg::read_packt_frame(_In_ ID3D11DeviceContext1* pContext, _Inout_ ID3D11Resource* pVideoBuffer,
//	_Inout_ uint8_t* pAudioBuffer, _Inout_ int& pAudioBufferSize)
//{
//	pAudioBufferSize = 0;
//	//We are giong to store audio buffers to pAudioBuffer untill we catch our fist video frame buffer
//	auto packetType = PacketType::None;
//	if (av_read_frame(avFormatCtx, avPacket) >= 0)
//	{
//		// Is this a packet from the video stream
//		if (avPacket->stream_index == videoStreamIndex)
//		{
//			packetType = PacketType::Video;
//
//			HRESULT* _hr = new HRESULT(S_FALSE);
//#ifdef __QT_DLL__
//			QImage _fakeQImage;
//			_hr = _copy_video_frame_to(pContext, pVideoBuffer, true, _fakeQImage, false, 1);
//#else
//			_hr = _copy_video_frame_to(pContext, pVideoBuffer);
//#endif
//			if (*_hr == S_FALSE)
//			{
//				V(S_FALSE, L"decoding video frame", this->name);
//			}
//
//			delete _hr;
//		}
//		else if (avPacket->stream_index == audioStreamIndex)
//		{
//			packetType = PacketType::Audio;
//
//			//Store audio in to the local audio buffer
//			if (_copy_audio_frame_to(pAudioBuffer, pAudioBufferSize) == S_FALSE)
//			{
//				V(S_FALSE, L"decoding audio frame", this->name);
//			}
//		}
//
//		//free the packet
//		av_free_packet(avPacket);
//	}
//
//	return packetType;
//}

//HRESULT W_ffmpeg::BufferToFile(std::string pPath, LONG64 pMaxFrames)
//{
//	HRESULT hr = S_OK;
//	size_t frameCounter = 0;
//	FILE* cacheFile = fopen(pPath.c_str(), "wb");
//
//	auto width = GetVideoWidth();
//	auto height = GetVideoHeight();
//
//	//Copy av frame data to the directX texture
//	const size_t rowPitch = width * 4; // 4 bytes of RGBA
//	const size_t rowCount = height;
//	const size_t slicePitch = rowPitch * rowCount;
//	auto dest = new UINT[slicePitch];
//
//	while ((pMaxFrames == -1 || frameCounter < pMaxFrames) && av_read_frame(avFormatCtx, this->avPacket) >= 0)
//	{
//		// Is this a packet from the video stream
//		if (this->avPacket->stream_index == videoStreamIndex)
//		{
//#pragma region read video buffer
//
//			int gotFrame = 0;
//			auto result = avcodec_decode_video2(this->videoCodec.avCodecCtx, this->videoCodec.avFrame, &gotFrame, this->avPacket);
//			if (result < 0)
//			{
//				hr = S_FALSE;
//				OnFailed(hr, "decoding video frame", this->Name);
//				return hr;
//			}
//
//			if (this->avPacket->dts == AV_NOPTS_VALUE && this->videoCodec.avFrame->opaque && *(uint64_t*) this->videoCodec.avFrame->opaque != AV_NOPTS_VALUE)
//			{
//				this->videoCodec.pts = *(uint64_t *) this->videoCodec.avFrame->opaque;
//			}
//			else if (this->avPacket->dts != AV_NOPTS_VALUE)
//			{
//				this->videoCodec.pts = avPacket->dts;
//			}
//			else
//			{
//				this->videoCodec.pts = 0;
//			}
//			this->videoCodec.pts *= av_q2d(this->videoCodec.avStream->time_base);
//
//			if (!gotFrame) continue;
//
//			UpdateClock(this->videoCodec);
//			UpdateTime(this->videoCodec.clock);
//
//#pragma region Re-Scale and store av frame in to the DirectX format
//
//			//Set the target convert properties
//			auto format = (AVPixelFormat)this->videoCodec.avFrame->format;
//
//			struct SwsContext* swsContext = sws_getContext(
//				width,
//				height,
//				format,
//				width,
//				height,
//				AV_PIX_FMT_BGRA,
//				SWS_BICUBIC,
//				NULL,
//				NULL,
//				NULL);
//
//			//Re-Scale it
//			sws_scale(
//				swsContext,
//				this->videoCodec.avFrame->data,
//				this->videoCodec.avFrame->linesize,
//				0,
//				height,
//				this->dxFrame->data,
//				this->dxFrame->linesize);
//
//			//copy data to the file
//			auto src = (UINT*)(this->dxFrame->data[0]);
//			memcpy(dest, src, width * height * sizeof(UINT));
//
//			fwrite(dest, width * height * sizeof(UINT), 1, cacheFile);
//			frameCounter++;
//
//			src = nullptr;
//			sws_freeContext(swsContext);
//
//#pragma endregion
//
//
//#pragma endregion
//		}
//		else if (this->avPacket->stream_index == audioStreamIndex)
//		{
//
//		}
//
//		//free the packet
//		av_free_packet(this->avPacket);
//	}
//
//	delete[] dest;
//	fflush(cacheFile);
//	fclose(cacheFile);
//
//	//Test for read
//	//cacheFile = fopen(pPath.c_str(), "rb");
//	//auto b = new UINT[720 * 576];
//	//fread(b, 720 * 576 * sizeof(UINT), 1, cacheFile);
//
//	return hr;
//}

HRESULT w_ffmpeg::buffer_video_to_memory(wolf::system::w_memory& pVideoMemory, UINT pDownSampling)
{
	HRESULT hr = S_OK;

	auto width = get_video_width();
	auto height = get_video_height();

	int gotFrame = 0;
	//Start reading
	while (!gotFrame && av_read_frame(avFormatCtx, this->avPacket) >= 0)
	{
		// Is this a packet from the video stream
		if (this->avPacket->stream_index == videoStreamIndex)
		{
#pragma region read video buffer

			auto result = avcodec_decode_video2(this->videoCodec.avCodecCtx, this->videoCodec.avFrame, &gotFrame, this->avPacket);
			if (result < 0)
			{
				hr = S_FALSE;
				V(hr, L"decoding video frame", this->name);
				return hr;
			}

			if (this->avPacket->dts == AV_NOPTS_VALUE && this->videoCodec.avFrame->opaque && *(uint64_t*) this->videoCodec.avFrame->opaque != AV_NOPTS_VALUE)
			{
				this->videoCodec.pts = *(uint64_t *) this->videoCodec.avFrame->opaque;
			}
			else if (this->avPacket->dts != AV_NOPTS_VALUE)
			{
				this->videoCodec.pts = avPacket->dts;
			}
			else
			{
				this->videoCodec.pts = 0;
			}
			this->videoCodec.pts *= av_q2d(this->videoCodec.avStream->time_base);

			if (!gotFrame) continue;

			_update_clock(this->videoCodec);
			_update_time(this->videoCodec.clock);

#pragma endregion

#pragma region Re-Scale and store av frame in to the DirectX format

			//Set the target convert properties
			auto format = (AVPixelFormat)this->videoCodec.avFrame->format;

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

			const size_t _slice_pitch = _des_width * _des_height  * 4;

			auto _hr = S_FALSE;
			if (_down_sampling)
			{
				//Re-Scale it
				sws_scale(
					swsContext,
					this->videoCodec.avFrame->data,
					this->videoCodec.avFrame->linesize,
					0,
					height,
					this->_down_sample_avFrame->data,
					this->_down_sample_avFrame->linesize);

				//copy data to the destination
				auto _frame = (uint8_t*) (this->_down_sample_avFrame->data[0]);

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
								_videoFrameData->data.at(i / 4) = (int) ((_frame[i + 3] & 0xFF) << 24) | ((_frame[i] & 0xFF) << 16) | ((_frame[i + 1] & 0xFF) << 8) | (_frame[i + 2] & 0xFF);//argb
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
					this->videoCodec.avFrame->data,
					this->videoCodec.avFrame->linesize,
					0,
					height,
					this->_av_Frame->data,
					this->_av_Frame->linesize);

				//copy data to the destination
				auto _frame = (uint8_t*) (this->_av_Frame->data[0]);
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
								_data[i / 4] = (int) ((_frame[i + 3] & 0xFF) << 24) | ((_frame[i] & 0xFF) << 16) | ((_frame[i + 1] & 0xFF) << 8) | (_frame[i + 2] & 0xFF);//argb
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
		av_free_packet(this->avPacket);
	}

	return hr;
}

HRESULT w_ffmpeg::buffer_audio_to_memory(wolf::system::w_memory& pAudioMemory, double& pAudioFrameVolumeDB)
{
	HRESULT hr = S_OK;

	auto width = get_video_width();
	auto height = get_video_height();

	pAudioFrameVolumeDB = 0;
	int gotFrame = 0;

	//Start storing all audio untill we get first video frame
	while (!gotFrame && av_read_frame(avFormatCtx, this->avPacket) >= 0)
	{
		if (this->avPacket->stream_index == audioStreamIndex)
		{
#pragma region read audio buffer

			auto result = avcodec_decode_audio4(audioCodec.avCodecCtx, audioCodec.avFrame, &gotFrame, avPacket);
			if (result < 0)
			{
				auto hr = S_FALSE;
				V(hr, L"decoding audio frame", this->name);
				return hr;
			}

			if (avPacket->dts == AV_NOPTS_VALUE && this->audioCodec.avFrame->opaque && *(uint64_t*) this->audioCodec.avFrame->opaque != AV_NOPTS_VALUE)
			{
				this->audioCodec.pts = *(uint64_t *) this->audioCodec.avFrame->opaque;
			}
			else if (avPacket->dts != AV_NOPTS_VALUE)
			{
				this->audioCodec.pts = avPacket->dts;
			}
			else
			{
				this->audioCodec.pts = 0;
			}
			this->audioCodec.pts *= av_q2d(this->audioCodec.avStream->time_base);

			if (!gotFrame) continue;

			_update_clock(this->audioCodec);
			_update_time(this->audioCodec.clock);

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
						this->audioConvert,
						&_dataPtr,
						AUDIO_FRAME_SIZE,
						(const uint8_t **) audioCodec.avFrame->data,
						audioCodec.avFrame->nb_samples));

					_frame->pts = this->avPacket->pts;

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
		av_free_packet(this->avPacket);
	}


	return hr;
}

HRESULT w_ffmpeg::buffer_to_memory(wolf::system::w_memory& pVideoMemory, wolf::system::w_memory& pAudioMemory)
{
	HRESULT hr = S_OK;

	auto width = get_video_width();
	auto height = get_video_height();

	int gotVideoFrame = 0;
	int gotAudioFrame = 0;

	//Start storing all audio untill we get first video frame
	while (!gotVideoFrame && av_read_frame(avFormatCtx, this->avPacket) >= 0)
	{
		// Is this a packet from the video stream
		if (this->avPacket->stream_index == videoStreamIndex)
		{
#pragma region read video buffer

			auto result = avcodec_decode_video2(this->videoCodec.avCodecCtx, this->videoCodec.avFrame, &gotVideoFrame, this->avPacket);
			if (result < 0)
			{
				hr = S_FALSE;
				V(hr, L"decoding video frame", this->name);
				return hr;
			}

			if (this->avPacket->dts == AV_NOPTS_VALUE && this->videoCodec.avFrame->opaque && *(uint64_t*) this->videoCodec.avFrame->opaque != AV_NOPTS_VALUE)
			{
				this->videoCodec.pts = *(uint64_t *) this->videoCodec.avFrame->opaque;
			}
			else if (this->avPacket->dts != AV_NOPTS_VALUE)
			{
				this->videoCodec.pts = avPacket->dts;
			}
			else
			{
				this->videoCodec.pts = 0;
			}
			this->videoCodec.pts *= av_q2d(this->videoCodec.avStream->time_base);

			if (!gotVideoFrame) continue;

			_update_clock(this->videoCodec);
			_update_time(this->videoCodec.clock);

#pragma region Re-Scale and store av frame in to the DirectX format

			//Set the target convert properties
			auto format = (AVPixelFormat)this->videoCodec.avFrame->format;

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
				this->videoCodec.avFrame->data,
				this->videoCodec.avFrame->linesize,
				0,
				height,
				this->_av_Frame->data,
				this->_av_Frame->linesize);

			const size_t _rowPitch = width * 4; // 4 bytes of RGBA
			const size_t _rowCount = height;
			const size_t _slicePitch = _rowPitch * _rowCount;

			//copy data to the destination
			auto _frame = (uint8_t*) (this->_av_Frame->data[0]);
			if (_frame)
			{
				auto _size = this->get_video_width() * this->get_video_height();
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
		else if (this->avPacket->stream_index == audioStreamIndex)
		{
#pragma region read audio buffer

			auto result = avcodec_decode_audio4(audioCodec.avCodecCtx, audioCodec.avFrame, &gotAudioFrame, avPacket);
			if (result < 0)
			{
				auto hr = S_FALSE;
				V(hr, L"decoding audio frame", this->name);
				return hr;
			}

			if (avPacket->dts == AV_NOPTS_VALUE && this->audioCodec.avFrame->opaque && *(uint64_t*) this->audioCodec.avFrame->opaque != AV_NOPTS_VALUE)
			{
				this->audioCodec.pts = *(uint64_t *) this->audioCodec.avFrame->opaque;
			}
			else if (avPacket->dts != AV_NOPTS_VALUE)
			{
				this->audioCodec.pts = avPacket->dts;
			}
			else
			{
				this->audioCodec.pts = 0;
			}
			this->audioCodec.pts *= av_q2d(this->audioCodec.avStream->time_base);

			if (!gotAudioFrame) continue;

			_update_clock(this->audioCodec);
			_update_time(this->audioCodec.clock);

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
						this->audioConvert,
						&_dataPtr,
						AUDIO_FRAME_SIZE,
						(const uint8_t **) audioCodec.avFrame->data,
						audioCodec.avFrame->nb_samples);
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
		av_free_packet(this->avPacket);
	}

	return hr;
}

void w_ffmpeg::_update_clock(Codec& pCodec)
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

void w_ffmpeg::_update_time(double pClock)
{
	this->elapsedTime = w_time_span::from_milliseconds(pClock * 1000.000);
	this->remainedTime = this->durationTime - this->elapsedTime;

	//If we just finished
	this->elapsedVideoFrames = static_cast<byte>(((float)this->elapsedTime.get_milliseconds() * this->frameRate) / 1000.0);
	if (this->elapsedTime == this->durationTime)
	{
		this->remainedVideoFrames = 0;
	}
	else
	{
		this->remainedVideoFrames = static_cast<byte>(this->frameRate - (float)this->elapsedVideoFrames);
	}
}

//ULONG W_ffmpeg::Release()
//{
//	if (this->IsReleased()) return 0;
//
//	this->audioframeVolumeDB = 0;
//	
//	//if (this->videoCodec.avCodec != nullptr)
//	//{
//	//	av_free(this->videoCodec.avCodec);
//	//}
//	//Free the video format context
//	/*if (this->avFormatCtx != nullptr)
//	{
//		avformat_close_input(&this->avFormatCtx);
//	}*/
//
//	//Free videoCodec
//	if (this->videoCodec.avFrame != nullptr)
//	{
//		av_free(this->videoCodec.avFrame);
//	}
//	if (this->videoCodec.avStream != nullptr)
//	{
//		av_free(this->videoCodec.avStream);
//	}
//	if (this->videoCodec.avCodecCtx != nullptr)
//	{
//		av_free(this->videoCodec.avCodecCtx);
//	}
//
//	//Free videoCodec
//	if (this->audioCodec.avFrame != nullptr)
//	{
//		av_free(this->audioCodec.avFrame);
//	}
//	if (this->audioCodec.avStream != nullptr)
//	{
//		av_free(this->audioCodec.avStream);
//	}
//	if (this->audioCodec.avCodecCtx != nullptr)
//	{
//		av_free(this->audioCodec.avCodecCtx);
//	}
//
//	//free swr
//	if (audioConvert != nullptr)
//	{
//		swr_free(&this->audioConvert);
//	}
//
//
//	//Free the packet
//	if (this->avPacket != nullptr)
//	{
//		av_free_packet(this->avPacket);
//	}
//
//	//Release directX frame
//	if (this->dxFrame != nullptr)
//	{
//		auto getdstFrame = this->dxFrame.get();
//		av_frame_free(&getdstFrame);
//		this->dxFrame.release();
//		this->dxFrame = nullptr;
//	}
//
//	
//	return 1;// W_Object::Release();
//}

ULONG w_ffmpeg::release()
{
	if (this->isReleased) return 0;

	this->fullPath.clear();

	this->audioframeVolumeDB = 0;

	//if (this->videoCodec.avCodec)
	//{
		//av_free(this->videoCodec.avCodec);
	//}
	if (this->videoCodec.avCodecCtx)
	{
		avcodec_close(videoCodec.avCodecCtx);
	}
	if (this->videoCodec.avFrame)
	{
		av_free(this->videoCodec.avFrame);
	}


	//if (this->audioCodec.avCodec)
	//{
		//av_free(this->audioCodec.avCodec);
	//}
	if (this->audioCodec.avCodecCtx)
	{
		avcodec_close(audioCodec.avCodecCtx);
	}
	if (this->audioCodec.avFrame)
	{
		av_free(this->audioCodec.avFrame);
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
	if (this->avPacket)
	{
		av_free_packet(this->avPacket);
	}

	//Free the video format context
	if (this->avFormatCtx)
	{
		avformat_close_input(&this->avFormatCtx);
	}

	//free swr
	if (audioConvert)
	{
		swr_free(&this->audioConvert);
	}

	this->isReleased = true;
	return 1;
}

void w_ffmpeg::release_MF()
{
	std::call_once(onceRelease, []()
	{
		MFShutdown();
	});
}

#pragma region Getters

bool w_ffmpeg::is_open() const
{
	return this->isMediaOpen;
}

const char* w_ffmpeg::get_media_path() const
{
	return this->fullPath.c_str();
}

std::wstring w_ffmpeg::get_media_type() const
{
	return this->type.c_str();
}

//size_t W_ffmpeg::GetTotalFrames() const
//{
//	return this->totalTime;
//}

w_time_span w_ffmpeg::get_duration_time() const
{
	return this->durationTime;
}

w_time_span w_ffmpeg::get_elapsed_time() const
{
	return this->elapsedTime;
}

w_time_span w_ffmpeg::get_remained_time() const
{
	return this->remainedTime;
}

size_t w_ffmpeg::get_total_video_frames() const
{
	return this->totalVideoFrames;
}

byte w_ffmpeg::get_duration_video_frames() const
{
	return this->durationVideoFrames;
}

byte w_ffmpeg::get_elapsed_video_frames() const
{
	return this->elapsedVideoFrames;
}

byte w_ffmpeg::get_remained_video_frames() const
{
	return this->remainedVideoFrames;
}

float w_ffmpeg::get_video_frame_rate() const
{
	return this->frameRate;
}

float w_ffmpeg::get_audio_frame_rate() const
{
	if (audioCodec.avCodecCtx != nullptr)
	{
		return audioCodec.avCodecCtx->ticks_per_frame;
	}
	return 0;
}

int w_ffmpeg::get_bit_rate() const
{
	if (this->avFormatCtx)
	{
		return this->avFormatCtx->bit_rate;
	}
	return 0;
}

int w_ffmpeg::get_video_channels() const
{
	if (this->videoCodec.avCodecCtx)
	{
		return this->videoCodec.avCodecCtx->channels;
	}
	return 0;
}

int w_ffmpeg::get_video_sample_rate() const
{
	if (this->videoCodec.avCodecCtx)
	{
		return this->videoCodec.avCodecCtx->sample_rate;
	}
	return 0;
}

int w_ffmpeg::get_video_width() const
{
	if (this->videoCodec.avCodecCtx)
	{
		return this->videoCodec.avCodecCtx->width;
	}
	return 0;
}

int w_ffmpeg::get_video_height() const
{
	if (this->videoCodec.avCodecCtx)
	{
		return this->videoCodec.avCodecCtx->height;
	}
	return 0;
}

int w_ffmpeg::get_audio_channels() const
{
	if (this->audioCodec.avCodecCtx)
	{
		return this->audioCodec.avCodecCtx->channels;
	}
	return 0;
}

int w_ffmpeg::get_audio_sample_rate() const
{
	if (this->audioCodec.avCodecCtx)
	{
		return this->audioCodec.avCodecCtx->sample_rate;
	}
	return 0;
}

double w_ffmpeg::get_audio_frame_volume_db() const
{
	return this->audioframeVolumeDB;
}

int64_t w_ffmpeg::get_packet_pts() const
{
	return this->avPacket->pts;
}

#pragma endregion
