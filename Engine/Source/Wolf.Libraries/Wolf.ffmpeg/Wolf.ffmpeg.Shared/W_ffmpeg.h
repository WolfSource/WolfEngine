/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @Wolf_Engine on twitter
	Name			 : W_ffmpeg.h
	Description		 : Manage ffmpeg commands for decoding video & audio
	Comment          :
*/

#pragma once

#include <W_Object.h>
#include <W_TimeSpan.h>
#include <stdint.h>
#include <memory>
#include <d3d11.h>
#include <d3d11_1.h>

#ifdef QT_DLL
#include <qimage.h>
#endif

extern "C"
{
#include <libavcodec\avcodec.h>
#include <libavformat\avformat.h>
#include <libswscale\swscale.h>
#include "libswresample\swresample.h"
#include <libavutil\imgutils.h>
#include <libavutil\samplefmt.h>
}

//Conflicts with D2D1::PixelFormat
#undef PixelFormat

namespace Wolf
{
	namespace Framework
	{
		class W_ffmpeg : public System::W_Object
		{
		public:

			//Must be call once before using class
			API static void InitializeMF();
			//Must be call once after releasing this class
			API static void ReleaseMF();

			enum PacketType : BYTE { None = 0, Video, Audio };

			//Make sure to call ffmpeg::InitializeMF(); once before creating instance
			API W_ffmpeg();
			//Make sure to call ffmpeg::ReleaseMF(); once before realeasing any instance
			API virtual ~W_ffmpeg();

			//Open a media with ffmpeg
			API HRESULT OpenMedia(std::wstring pMediaPath, double pSeekTo = 0, bool pLog4User = true);
			API HRESULT OpenVideoStreamer(std::wstring pMediaPath);
			API HRESULT OpenAudioStreamer(std::wstring pMediaPath);

			//Seek to specific seconds of media
			API HRESULT SeekTo(double pSecond);

			//Store the current video frame in to both direct3D resource
			//return values: 0  : Bad end of file, -1 : Error happended during deconding, 1 : OK
			API int GetVideoFrame(_In_ ID3D11DeviceContext1* pContext, _Inout_ ID3D11Resource* pResource
#ifdef QT_DLL
				, bool pCopyToDirectXBuffer, _Inout_ QImage& pQImage, bool pStoreInQImage = false, int pQTDownSampleScale = 1
#endif
			);

			//Get the current audio frame buffer
			//return values: 0  : Bad end of file, -1 : Error happended during deconding, 1  : OK
			API int GetAudioFrame(uint8_t* pBuffer, int& pBufferSize);

			//Read all audio packets utill we catch first video frame
			API PacketType ReadPacktFrame(_In_ ID3D11DeviceContext1* pContext, _Inout_ ID3D11Resource* pVideoBuffer,
				_Inout_ uint8_t* pAudioBuffer, _Inout_ int& pAudioSize);

			//Release resources
			API ULONG Release() override;

#pragma region Getters

			//Is the media opened successfully
			API bool IsOpen() const;
			//Get theb type of this media
			API const char* GetMediaPath() const;
			//Get theb type of this media
			API std::wstring GetMediaType() const;

			//Get the time of media
			API Wolf::System::W_TimeSpan GetDurationTime() const;
			//Get the elpased time of media
			API Wolf::System::W_TimeSpan GetElapsedTime() const;
			//Get the remained time of media
			API Wolf::System::W_TimeSpan GetRemainedTime() const;
			
			//Get number of frames
			API double GetTotalVideoFrames() const;
			//Get number of elpased frames 
			API double GetElapsedVideoFrames() const;
			//Get number of remained frames 
			API double GetRemainedVideoFrames() const;

			//Get the frame per seconds of media
			API UINT GetMediaFPS() const;
			//Get the bit rate of media
			API int GetBitRate() const;
			//Get the total channels of video
			API int GetVideoChannels() const;
			//Get the sample rate of video
			API int GetVideoSampleRate() const;
			//Get the width of video frame
			API int GetVideoWidth() const;
			//Get the height of video frame
			API int GetVideoHeight() const;
			//Get the total channels of audio
			API int GetAudioChannels() const;
			//Get the sample rate of audio
			API int GetAudioSampleRate() const;
			//Get the maximum audio frame size
			API int GetMaxAudioframeSize() const;
			//Get the current volume of audio frame in DB
			API double GetAudioframeVolumeDB() const;

#pragma endregion

		private:
			struct Codec
			{
				AVStream*			avStream;
				AVCodecContext*		avCodecCtx;
				AVCodec*			avCodec;
				AVFrame*			avFrame;
				double				pts;
				double				clock;
			};

			//Copy video frame to the DirectX buffer
			//Return values are : 
			//S_OK, everything ok
			//S_FALSE, error just happended
			//NULL, did not get the frame, try for another time
			HRESULT* CopyVideoFrameTo(_In_ ID3D11DeviceContext1* pContext, _Inout_ ID3D11Resource* pResource
#ifdef QT_DLL
				, bool pCopyToDirectXBuffer,
				QImage& pQImage, bool pStoreInQImage, int pQTDownSampleScale
#endif
				);
			//Copy audio frame to the buffer
			HRESULT CopyAudioFrameTo(uint8_t* pBuffer, int& pBufferSize);
			void UpdateClock(Codec& pCodec);
			void UpdateTime(double pclock);

#ifdef QT_DLL
			QImage										fakeQImage;
#endif
			std::unique_ptr<AVFrame>					dxFrame;
			bool										isMediaOpen;
			AVFormatContext*							avFormatCtx;
			Codec										videoCodec;
			Codec										audioCodec;
			AVPacket*									avPacket;
			struct SwrContext*							audioConvert;
			int											audio_out_channels_layout;
			int											videoStreamIndex;
			int											audioStreamIndex;
			std::string									fullPath;
			std::wstring								type;
			
			Wolf::System::W_TimeSpan					totalTime;
			Wolf::System::W_TimeSpan					elapsedTime;
			Wolf::System::W_TimeSpan					remainedTime;

			double										totalVideoFrames;
			double										elapsedVideoFrames;
			double										remainedVideoFrames;
			
			float										frameRate;
			double										audioframeVolumeDB;
		};
	}
}
