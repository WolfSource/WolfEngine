#include "Wolf.ffmpeg.Shared.PCH.h"
#include "W_ffmpeg.h"
#include <IO.h>
#include <wrl\client.h>
#include <iomanip>      // For std::hex
#include <codecvt>      // For Unicode conversions

using namespace std;
using namespace Wolf::System;
using namespace Wolf::Framework;
using namespace Microsoft::WRL;

static const int MaxAudioBufferSize = 192000;// = 1 second of 48khz 32bit audio
static const MFTIME MF_ONE_SEC = 10000000;
static const LONG MF_ONE_MSEC = 1000;
static std::once_flag onceInit;
static std::once_flag onceRelease;

W_ffmpeg::W_ffmpeg() : isMediaOpen(false)
{
	this->Name = typeid(this).name();

	ZeroMemory(&this->videoCodec, sizeof(Codec));
	ZeroMemory(&this->audioCodec, sizeof(Codec));

	// Register all formats and codec(s)
	av_register_all();
	avformat_network_init();

	//Initialize the packet
	this->avPacket = new AVPacket();
	av_init_packet(this->avPacket);

	this->audioConvert = swr_alloc();
	this->avFormatCtx = avformat_alloc_context();
}

W_ffmpeg::~W_ffmpeg()
{
	Release();
}

void W_ffmpeg::InitializeMF()
{
	std::call_once(onceInit, []()
	{
		MFStartup(MF_VERSION);
	});
}

HRESULT W_ffmpeg::OpenMedia(wstring pMediaPath, double pSeekTo, bool pLog4User)
{
	//Todo : Later check we must change the auto mode, this will uncomment on that situation
	///////*if (this->GetMediaPath() == pMediaPath)
	//////{
	//////	this->isMediaOpen = true;
	//////	return;
	//////}*/
	
	if (this->isMediaOpen) Release();
	this->isMediaOpen = false;
	{
		this->fullPath = ToUTF8(pMediaPath).c_str();

		//Output the message
		wstring msg = L"Media from following path \"" + pMediaPath + L"\" is going to decode";
		//OutputDebugString(msg.c_str());
		if (pLog4User)
		{
			Logger.User(msg);
		}
		msg.clear();

		HRESULT hr = S_OK;
		// Open video file
		bool containVideo = true, containAudio = true;
		//Open file
		if (avformat_open_input(&avFormatCtx, this->fullPath.c_str(), NULL, NULL) != 0)
		{
			hr = S_FALSE;
			OnFailed(hr, "openning file", this->Name);
			Release();
			return hr;
		}

		// Retrieve stream information
		if (avformat_find_stream_info(avFormatCtx, NULL) < 0)
		{
			hr = S_FALSE;
			OnFailed(hr, "finding file stream information", this->Name);
			Release();
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
				OnFailed(hr, "finding stream of video", this->Name);
				Logger.User("Media file does not have video frames");
			}
		}
		if (this->audioStreamIndex <= -1)
		{
			hr = S_FALSE;
			hasAudioStream = false;
			if (pLog4User)
			{
				OnFailed(hr, "finding stream of audio", this->Name);
				Logger.User("Media file does not have audio frames");
			}
		}

		if (!hasVideoStream && !hasAudioStream)
		{
			Logger.User("File " + this->fullPath + " did not load");
			Release();
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
			Logger.Write("creating source resolver");
		}
		hr = sourceResolver->CreateObjectFromURL(
			pMediaPath.c_str(),
			MF_RESOLUTION_MEDIASOURCE,
			NULL,
			&objectType,
			&pSource);
		if (FAILED(hr))
		{
			Logger.Write("creating object from source for source resolver");
		}

		hr = pSource.Get()->QueryInterface(IID_PPV_ARGS(&source));
		if (FAILED(hr))
		{
			Logger.Write("Query interface for media source");
		}

		ComPtr<IMFPresentationDescriptor> pPD = nullptr;
		hr = source->CreatePresentationDescriptor(&pPD);
		if (FAILED(hr))
		{
			Logger.Write("creating presentation descriptor");
		}
		auto pDuration = new MFTIME(0);
		pPD->GetUINT64(MF_PD_DURATION, (UINT64*) pDuration);
		auto duration = (LONG) (*pDuration / (MF_ONE_SEC / MF_ONE_MSEC));
		pDuration = nullptr;

		//Release 
		COM_RELEASE(pPD);
		COM_RELEASE(pSource);
		COM_RELEASE(sourceResolver);

#pragma endregion

		this->totalTime = W_TimeSpan::FromMilliseconds(duration);
		this->remainedTime = this->totalTime;
		this->elapsedTime = W_TimeSpan::Zero();
		
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
		this->totalVideoFrames = (this->totalTime.GetMilliseconds() * static_cast<double>(this->frameRate)) / 1000;
		this->remainedVideoFrames = this->totalVideoFrames;
		this->elapsedVideoFrames = 0.0;

		//Find the decoder for the video and audio and also allocate frames for video & audio

		if (hasVideoStream)
		{
			videoCodec.avCodec = avcodec_find_decoder(videoCodec.avCodecCtx->codec_id);
			if (videoCodec.avCodec == NULL)
			{
				hr = S_FALSE;
				OnFailed(hr, "Unsupported codec for video", this->Name);
				Release();
				return hr;
			}
			//Open codec
			if (avcodec_open2(videoCodec.avCodecCtx, videoCodec.avCodec, NULL) < 0)
			{
				hr = S_FALSE;
				OnFailed(hr, "Could not open video codec", this->Name);
				Release();
				return hr;
			}
			videoCodec.avFrame = avcodec_alloc_frame();

			auto videoBitRater = GetBitRate();
			auto videoChannels = GetVideoChannels();
			auto videoSampleRate = GetVideoSampleRate();

			auto width = GetVideoWidth();
			auto height = GetVideoHeight();

			//Create destination avFrame
			this->dxFrame.reset(avcodec_alloc_frame());
			avcodec_get_frame_defaults(this->dxFrame.get());

			//This time we need BGRA for directX
			avpicture_alloc(
				(AVPicture*) this->dxFrame.get(),
				AV_PIX_FMT_BGRA,
				width,
				height);
		}

		if (hasAudioStream)
		{
			audioCodec.avCodec = avcodec_find_decoder(audioCodec.avCodecCtx->codec_id);
			if (audioCodec.avCodec == NULL)
			{
				hr = S_FALSE;
				OnFailed(hr, "Unsupported codec for audio", this->Name);
				Release();
				return hr;
			}

			if (avcodec_open2(audioCodec.avCodecCtx, audioCodec.avCodec, NULL) < 0)
			{
				hr = S_FALSE;
				OnFailed(hr, "Could not open audio codec", this->Name);
				Release();
				return hr;
			}
			audioCodec.avFrame = avcodec_alloc_frame();

			auto audioChannels = GetAudioChannels();
			auto audioSampleRate = GetAudioSampleRate();

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
		}

		//Seek to the specific time in seconds
		if (pSeekTo != 0)
		{
			SeekTo(pSeekTo);
		}

		//Output the infomation
		if (pLog4User)
		{
			msg = L"The media decoded successfully with the following information : time: " + this->totalTime.ToWString() + L" , fps: " + std::to_wstring(this->frameRate);
			Logger.User(msg);
		}

		msg.clear();
	}

	pMediaPath.clear();
	this->isMediaOpen = true;

	return S_OK;
}

HRESULT W_ffmpeg::OpenVideoStreamer(wstring pMediaPath)
{
	return S_OK;
}

HRESULT W_ffmpeg::OpenAudioStreamer(wstring pMediaPath)
{
	return S_OK;
}

HRESULT W_ffmpeg::SeekTo(double pSecond)
{
	auto hr = S_OK;

	auto elapsedTotalSec = GetElapsedTime().GetTotalSeconds();
	auto totalSec = GetDurationTime().GetTotalSeconds();

	if (pSecond == elapsedTotalSec || pSecond < 0) return hr;

	int flags = AVSEEK_FLAG_ANY;
	int seek_ts = 0;

	//Seek the video
	if (this->videoStreamIndex > -1)
	{
		auto videoStream = this->videoCodec.avStream;
		seek_ts = (pSecond * (videoStream->time_base.den)) / (videoStream->time_base.num);
		if (av_seek_frame(avFormatCtx, this->videoStreamIndex, seek_ts, flags) < 0)
		{
			hr = S_FALSE;
			OnFailed(hr, "seeking video to the " + to_string(pSecond) + " time", this->Name);
			return hr;
		}
		//UpdateClock(this->videoCodec);
		UpdateTime(this->videoCodec.clock);

		this->remainedVideoFrames = this->totalVideoFrames;
		this->elapsedVideoFrames = 0.0;

	}


	//Seek the audio
	if (this->audioStreamIndex > -1)
	{
		auto audioStream = this->audioCodec.avStream;
		seek_ts = (pSecond * (audioStream->time_base.den)) / (audioStream->time_base.num);
		if (av_seek_frame(avFormatCtx, this->audioStreamIndex, seek_ts, flags) < 0)
		{
			hr = S_FALSE;
			OnFailed(hr, "seeking audio to the " + to_string(pSecond) + " time", this->Name);
			return hr;
		}

		UpdateTime(this->audioCodec.clock);
	}


	//Sleep(10);

	return hr;
}

int W_ffmpeg::GetVideoFrame(_In_ ID3D11DeviceContext1* pContext, _Inout_ ID3D11Resource* pResource
#ifdef QT_DLL
	, bool pCopyToDirectXBuffer,
	QImage& pQImage, bool pStoreInQImage, int pQTDownSampleScale
#endif
	)
{
	auto hr = 1;
	//We need the first video frame from current time
	while (true)
	{
		auto frameRead = av_read_frame(avFormatCtx, avPacket);
		if (frameRead < 0)
		{
			//Bad end of file
			hr = 0;
			break;
		}
		if (avPacket->stream_index == this->videoStreamIndex)
		{
			auto hr_False = make_unique<HRESULT>(S_FALSE);
			auto _hr = CopyVideoFrameTo(pContext, pResource
#ifdef QT_DLL
				, pCopyToDirectXBuffer, pQImage, pStoreInQImage, pQTDownSampleScale
#endif
			);
			av_free_packet(avPacket);

			if (_hr == NULL)
			{
				continue;
			}
			if (_hr == hr_False.get())
			{
				OnFailed(S_FALSE, "decoding video frame", this->Name);
				hr = -1;
			}

			break;
		}
		//free the packet
		av_free_packet(avPacket);
	}

	return hr;
}

int W_ffmpeg::GetAudioFrame(uint8_t* pBuffer, int& pBufferSize)
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
			if (CopyAudioFrameTo(pBuffer, pBufferSize) == S_OK)
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
				OnFailed(S_FALSE, "decoding audio frame", this->Name);
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

HRESULT* W_ffmpeg::CopyVideoFrameTo(_In_ ID3D11DeviceContext1* pContext, _Inout_ ID3D11Resource* pResource
#ifdef QT_DLL
	,bool pCopyToDirectXBuffer,
	QImage& pQImage, bool pStoreInQImage, int pQTDownSampleScale
#endif
	)
{
	auto hr = S_OK;

	if (this->videoStreamIndex <= -1) return &hr;

	int frameFinished = 0;

	auto result = avcodec_decode_video2(this->videoCodec.avCodecCtx, this->videoCodec.avFrame, &frameFinished, this->avPacket);
	if (result < 0)
	{
		hr = S_FALSE;
		OnFailed(hr, "decoding video frame", this->Name);
		return &hr;
	}

	if (avPacket->dts == AV_NOPTS_VALUE && this->videoCodec.avFrame->opaque && *(uint64_t*) this->videoCodec.avFrame->opaque != AV_NOPTS_VALUE)
	{
		this->videoCodec.pts = *(uint64_t *) this->videoCodec.avFrame->opaque;
	}
	else if (avPacket->dts != AV_NOPTS_VALUE)
	{
		this->videoCodec.pts = avPacket->dts;
	}
	else
	{
		this->videoCodec.pts = 0;
	}
	this->videoCodec.pts *= av_q2d(this->videoCodec.avStream->time_base);

	if (!frameFinished) return nullptr;

	UpdateClock(this->videoCodec);
	UpdateTime(this->videoCodec.clock);
	
	//Set the target convert properties
	auto format = (AVPixelFormat) videoCodec.avFrame->format;
	
	auto width = this->GetVideoWidth();
	auto height = this->GetVideoHeight();

#ifdef QT_DLL
	//If we need to store data in qt image too
	if (pStoreInQImage)
	{
#pragma region Store av frame in to the qt image

		auto downSampleWidth = width;
		auto downSampleHeight = height;

		if (pQTDownSampleScale > 1)
		{
			downSampleWidth /= pQTDownSampleScale;
			downSampleHeight /= pQTDownSampleScale;
		}
		else
		{
			hr = S_FALSE;
			OnFailed(hr, "unsupported down sample value for re-scaling video frame", this->Name);
			return &hr;
		}

		//check qImage
		if (pQImage.isNull())
		{
			pQImage = QImage(downSampleWidth, downSampleHeight, QImage::Format_RGB32);
		}
		else if (pQImage.width() != downSampleWidth || pQImage.height() != downSampleHeight)
		{
			pQImage = QImage(downSampleWidth, downSampleHeight, QImage::Format_RGB32);
		}

		//We need BGR for QT image
		struct SwsContext* swsContext = sws_getContext(
			width,
			height,
			format,
			downSampleWidth,
			downSampleHeight,
			AV_PIX_FMT_BGR24,
			SWS_BICUBIC,
			NULL,
			NULL,
			NULL);

		//Re-Scale it
		sws_scale(
			swsContext,
			videoCodec.avFrame->data,
			videoCodec.avFrame->linesize,
			0,
			height,
			this->dxFrame->data,
			this->dxFrame->linesize);

		auto src = (uint8_t *) (this->dxFrame->data[0]);
		for (int j = 0; j < downSampleHeight; j++)
		{
			auto scanLine = (QRgb *) pQImage.scanLine(j);
			for (size_t i = 0; i < downSampleWidth; i += 1)
			{
				auto index = i * 3;
				scanLine[i] = qRgb(src[index + 2], src[index + 1], src[index]);
			}
			src += this->dxFrame->linesize[0];
		}

		//Release
		src = nullptr;
		sws_freeContext(swsContext);

#pragma endregion
	}

#endif

	if (pResource == nullptr)
	{
		Logger.Warning("Resource is null");
		hr = S_FALSE;
		return &hr;
	}

#pragma region Store av frame in to the DirectX texture

		//Copy av frame data to the directX texture
		const size_t rowPitch = width * 4; // 4 bytes of RGBA
		const size_t rowCount = height;
		const size_t slicePitch = rowPitch * rowCount;

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
			videoCodec.avFrame->data,
			videoCodec.avFrame->linesize,
			0,
			height,
			this->dxFrame->data,
			this->dxFrame->linesize);

		D3D11_MAPPED_SUBRESOURCE mapsub;
		ZeroMemory(&mapsub, sizeof(D3D11_MAPPED_SUBRESOURCE));
		pContext->Map(pResource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapsub);
		{
			auto data = static_cast<uint8_t*>(mapsub.pData);
			if (!data)
			{
				hr = S_FALSE;
				pContext->Unmap(pResource, 0);
				OnFailed(hr, "E_POINTER on getting mapped pixels", this->Name);
				return &hr;
			}

			auto src = (uint8_t *) (this->dxFrame->data[0]);
			size_t msize = std::fmin<size_t>(rowPitch, mapsub.RowPitch);
			for (size_t h = 0; h < rowCount; ++h)
			{
				memcpy_s(data, rowPitch, src, msize);
				data += mapsub.RowPitch;
				src += rowPitch;
			}

			pContext->Unmap(pResource, 0);
			src = nullptr;
			data = nullptr;
		}

		sws_freeContext(swsContext);

#pragma endregion

	return &hr;
}

HRESULT W_ffmpeg::CopyAudioFrameTo(uint8_t* pBuffer, int& pBufferSize)
{
	if (this->audioStreamIndex <= -1) return S_OK;

	int frameFinished = 0;
	auto result = avcodec_decode_audio4(audioCodec.avCodecCtx, audioCodec.avFrame, &frameFinished, avPacket);
	if (result < 0)
	{
		auto hr = S_FALSE;
		OnFailed(hr, "decoding audio frame", this->Name);
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

	UpdateClock(this->audioCodec);
	UpdateTime(this->audioCodec.clock);

	pBufferSize = swr_convert(
		this->audioConvert,
		&pBuffer,
		MaxAudioBufferSize,
		(const uint8_t **) audioCodec.avFrame->data,
		audioCodec.avFrame->nb_samples);

	return S_OK;
}

W_ffmpeg::PacketType W_ffmpeg::ReadPacktFrame(_In_ ID3D11DeviceContext1* pContext, _Inout_ ID3D11Resource* pVideoBuffer,
	_Inout_ uint8_t* pAudioBuffer, _Inout_ int& pAudioBufferSize)
{
	pAudioBufferSize = 0;
	//We are giong to store audio buffers to pAudioBuffer untill we catch our fist video frame buffer
	auto packetType = PacketType::None;
	if (av_read_frame(avFormatCtx, avPacket) >= 0)
	{
		// Is this a packet from the video stream
		if (avPacket->stream_index == videoStreamIndex)
		{
			packetType = PacketType::Video;

			auto hr = CopyVideoFrameTo(pContext, pVideoBuffer
#ifdef QT_DLL
				, true, this->fakeQImage, false, 1
#endif
			);

			if (*hr == S_FALSE)
			{
				OnFailed(S_FALSE, "decoding video frame", this->Name);
			}
		}
		else if (avPacket->stream_index == audioStreamIndex)
		{
			packetType = PacketType::Audio;

			//Store audio in to the local audio buffer
			if (CopyAudioFrameTo(pAudioBuffer, pAudioBufferSize) == S_FALSE)
			{
				OnFailed(S_FALSE, "decoding audio frame", this->Name);
			}
		}

		//free the packet
		av_free_packet(avPacket);
	}
	
	return packetType;
}

void W_ffmpeg::UpdateClock(Codec& pCodec)
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

void W_ffmpeg::UpdateTime(double pClock)
{
	this->elapsedTime = W_TimeSpan::FromMilliseconds(pClock * 1000.000);
	this->remainedTime = this->totalTime - this->elapsedTime;

	//If we just finished
	this->elapsedVideoFrames = (this->elapsedTime.GetMilliseconds() * this->frameRate) / 1000;
	if (this->elapsedTime == this->totalTime)
	{
		this->remainedVideoFrames = 0;
	}
	else
	{
		this->remainedVideoFrames = this->frameRate - this->elapsedVideoFrames;
	}
}

ULONG W_ffmpeg::Release()
{
	if (this->IsReleased()) return 0;

	this->audioframeVolumeDB = 0;


	//if (this->videoCodec.avCodec != nullptr)
	//{
	//	av_free(this->videoCodec.avCodec);
	//}
	//Free the video format context
	/*if (this->avFormatCtx != nullptr)
	{
		avformat_close_input(&this->avFormatCtx);
	}*/

	//Free videoCodec
	if (this->videoCodec.avFrame != nullptr)
	{
		av_free(this->videoCodec.avFrame);
	}
	if (this->videoCodec.avStream != nullptr)
	{
		av_free(this->videoCodec.avStream);
	}
	if (this->videoCodec.avCodecCtx != nullptr)
	{
		av_free(this->videoCodec.avCodecCtx);
	}

	//Free videoCodec
	if (this->audioCodec.avFrame != nullptr)
	{
		av_free(this->audioCodec.avFrame);
	}
	if (this->audioCodec.avStream != nullptr)
	{
		av_free(this->audioCodec.avStream);
	}
	if (this->audioCodec.avCodecCtx != nullptr)
	{
		av_free(this->audioCodec.avCodecCtx);
	}

	//free swr
	if (audioConvert != nullptr)
	{
		swr_free(&this->audioConvert);
	}


	//Free the packet
	if (this->avPacket != nullptr)
	{
		av_free_packet(this->avPacket);
	}

	//Release directX frame
	if (this->dxFrame != nullptr)
	{
		auto getdstFrame = this->dxFrame.get();
		av_frame_free(&getdstFrame);
		this->dxFrame.release();
		this->dxFrame = nullptr;
	}

	
	return 1;// W_Object::Release();
}

void W_ffmpeg::ReleaseMF()
{
	std::call_once(onceRelease, []()
	{
		MFShutdown();
	});
}

#pragma region Getters

bool W_ffmpeg::IsOpen() const
{
	return this->isMediaOpen;
}

const char* W_ffmpeg::GetMediaPath() const
{
	return this->fullPath.c_str();
}

wstring W_ffmpeg::GetMediaType() const
{
	return this->type.c_str();
}

W_TimeSpan W_ffmpeg::GetDurationTime() const
{
	return this->totalTime;
}

W_TimeSpan W_ffmpeg::GetElapsedTime() const
{
	return this->elapsedTime;
}

W_TimeSpan W_ffmpeg::GetRemainedTime() const
{
	return this->remainedTime;
}

double W_ffmpeg::GetTotalVideoFrames() const
{
	return this->totalVideoFrames;
}

double W_ffmpeg::GetElapsedVideoFrames() const
{
	return this->elapsedVideoFrames;
}

double W_ffmpeg::GetRemainedVideoFrames() const
{
	return this->remainedVideoFrames;
}

UINT W_ffmpeg::GetMediaFPS() const
{
	return static_cast<UINT>(this->frameRate);
}

int W_ffmpeg::GetBitRate() const
{
	if (this->avFormatCtx)
	{
		return this->avFormatCtx->bit_rate;
	}
	return 0;
}

int W_ffmpeg::GetVideoChannels() const
{
	if (this->videoCodec.avCodecCtx)
	{
		return this->videoCodec.avCodecCtx->channels;
	}
	return 0;
}

int W_ffmpeg::GetVideoSampleRate() const
{
	if (this->videoCodec.avCodecCtx)
	{
		return this->videoCodec.avCodecCtx->sample_rate;
	}
	return 0;
}

int W_ffmpeg::GetVideoWidth() const
{
	if (this->videoCodec.avCodecCtx)
	{
		return this->videoCodec.avCodecCtx->width;
	}
	return 0;
}

int W_ffmpeg::GetVideoHeight() const
{
	if (this->videoCodec.avCodecCtx)
	{
		return this->videoCodec.avCodecCtx->height;
	}
	return 0;
}

int W_ffmpeg::GetAudioChannels() const
{
	if (this->audioCodec.avCodecCtx)
	{
		return this->audioCodec.avCodecCtx->channels;
	}
	return 0;
}

int W_ffmpeg::GetAudioSampleRate() const
{
	if (this->audioCodec.avCodecCtx)
	{
		return this->audioCodec.avCodecCtx->sample_rate;
	}
	return 0;
}

int W_ffmpeg::GetMaxAudioframeSize() const
{
	return MaxAudioBufferSize;
}

double W_ffmpeg::GetAudioframeVolumeDB() const
{
	return this->audioframeVolumeDB;
}

#pragma endregion
