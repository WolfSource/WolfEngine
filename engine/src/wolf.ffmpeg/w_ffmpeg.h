/*
Project			 : Wolf Engine (http://WolfSource.io). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
Source			 : https://github.com/PooyaEimandar/Wolf.Engine - Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
Website			 : http://WolfSource.io
Name			 : w_ffmpeg.h
Description		 : ffmpeg interface
Comment          :
*/

#ifndef __W_FFMPEG_H__
#define __W_FFMPEG_H__

#include "w_ffmpeg_dll.h"

#include <w_object.h>
#include <w_time_span.h>
#include "w_memory.h"
#include <stdint.h>
#include <memory>
#include <array>

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

#if defined(__1080p_50__) || defined(__1080p_25__)

#define VIDEO_FRAME_SIZE					2073600//Width = 1920 * Height = 1080
#define VIDEO_FRAME_DOWN_SAMPLE_SIZE		518400//Width = 960 * Height = 540

#elif defined (__PAL__)

#define VIDEO_FRAME_SIZE					414720//Width = 720 * Height = 576
#define VIDEO_FRAME_DOWN_SAMPLE_SIZE		103680//Width = 360 * Height = 288

#elif defined (__NTSC__)

#define VIDEO_FRAME_SIZE					345600//Width = 720 * Height = 480
#define VIDEO_FRAME_DOWN_SAMPLE_SIZE		86400//Width = 360 * Height = 240

#endif

#define AUDIO_FRAME_SIZE		192000// = 1 second of 48khz 32bit audio

namespace wolf
{
	namespace framework
	{
		//class w_video_frame_data
		//{
		//public:
		//	w_video_frame_data(int pWidth, int pHeight)
		//	{
		//		this->data = new int[pWidth * pHeight];
		//		this->size = sizeof(this->data);
		//		this->alignment = std::alignment_of<int>::value;
		//	}
		//	int*		data;
		//	int			size;
		//	int			alignment;
		//};

		class w_video_frame_down_sample_data
		{
		public:
			std::array<int, VIDEO_FRAME_DOWN_SAMPLE_SIZE> data;
		};

		class w_audio_frame_data
		{
		public:
			uint8_t data[AUDIO_FRAME_SIZE];
			UINT usedSize;
			int64_t pts;
		};

		class w_ffmpeg
		{
		public:

			//Must be call once before using class
			FF_EXP static void initialize_MF();
			//Must be call once after releasing this class
			FF_EXP static void release_MF();

			enum PacketType : BYTE { None = 0, Video, Audio };

			//Make sure to call ffmpeg::InitializeMF(); once before creating instance
			FF_EXP w_ffmpeg();
			//Make sure to call ffmpeg::ReleaseMF(); once before releasing any instance
			FF_EXP virtual ~w_ffmpeg();

			//Open a media with ffmpeg
			FF_EXP HRESULT open_media(std::wstring pMediaPath, int64_t pSeekToFrame = 0, bool pLog4User = true);

			//Convert specific milliseconds to frame number
			FF_EXP int64_t time_to_frame(int64_t pMilliSecond);

			/*
				Seek to specific frame of media
				return value =
					1 = OK
					0 = Out of range
					-1 = Video Error
					-2 = Audio Error
					-3 = Both Video & Audio Error
			*/
			FF_EXP int seek_frame(int64_t pFrame);

			/*
				Seek to specific milliseconds time of media
				return value =
					1 = OK
					0 = Out of range
					-1 = Video Error
					-2 = Audio Error
					-3 = Both Video & Audio Error
			*/
			FF_EXP int seek_frame_milliSecond(int64_t pMilliSecond);

			//Store the current video frame in to both direct3D resource
			//return values: 0  : Bad end of file, -1 : Error happended during deconding, 1 : OK
//			DLL int get_video_frame(_In_ ID3D11DeviceContext1* pContext, _Inout_ ID3D11Resource* pResource
//#ifdef __QT_DLL__
//				, bool pCopyToDirectXBuffer, _Inout_ QImage& pQImage, bool pStoreInQImage = false, int pQTDownSampleScale = 1
//#endif
//				);
//
			//Get the current audio frame buffer
			//return values: 0  : Bad end of file, -1 : Error happended during deconding, 1  : OK
			FF_EXP int get_audio_frame(uint8_t* pBuffer, int& pBufferSize);

			//Read all audio packets utill we catch first video frame
			//DLL PacketType read_packt_frame(_In_ ID3D11DeviceContext1* pContext, _Inout_ ID3D11Resource* pVideoBuffer,
			//	_Inout_ uint8_t* pAudioBuffer, _Inout_ int& pAudioSize);

			//Cache all data into the file
			//API HRESULT BufferToFile(std::string pPath, LONG64 pMaxFrames);

			//Store video frame data in to the memory
			FF_EXP HRESULT buffer_video_to_memory(wolf::system::w_memory& pVideoMemory, UINT pDownSampling = 1);

			//Store audio frame data in to the memory
			FF_EXP HRESULT buffer_audio_to_memory(wolf::system::w_memory& pAudioMemory, double& pAudioFrameVolumeDB);

			//Read all data in to the memory, make sure the capacity size of video and audio chuck is equal 
			FF_EXP HRESULT buffer_to_memory(wolf::system::w_memory& pVideoMemory, wolf::system::w_memory& pAudioMemory);

			//Release resources
			FF_EXP ULONG release();

#pragma region Getters

			//Is the media opened successfully
			FF_EXP bool is_open() const;
			//Get theb type of this media
			FF_EXP const char* get_media_path() const;
			//Get theb type of this media
			FF_EXP std::wstring get_media_type() const;

			//Get the time of media
			FF_EXP wolf::system::w_time_span get_duration_time() const;
			//Get the elpased time of media
			FF_EXP wolf::system::w_time_span get_elapsed_time() const;
			//Get the remained time of media
			FF_EXP wolf::system::w_time_span get_remained_time() const;

			//Get number of total video frames
			FF_EXP size_t get_total_video_frames() const;
			//Get number of duration frames
			FF_EXP byte get_duration_video_frames() const;
			//Get number of elpased frames 
			FF_EXP byte get_elapsed_video_frames() const;
			//Get number of remained frames 
			FF_EXP byte get_remained_video_frames() const;

			//Get the frame per seconds of video
			FF_EXP float get_video_frame_rate() const;
			//Get the ticks per frame of audio
			FF_EXP float get_audio_frame_rate() const;

			//Get the bit rate of media
			FF_EXP int get_bit_rate() const;
			//Get the total channels of video
			FF_EXP int get_video_channels() const;
			//Get the sample rate of video
			FF_EXP int get_video_sample_rate() const;
			//Get the width of video frame
			FF_EXP int get_video_width() const;
			//Get the height of video frame
			FF_EXP int get_video_height() const;
			//Get the total channels of audio
			FF_EXP int get_audio_channels() const;
			//Get the sample rate of audio
			FF_EXP int get_audio_sample_rate() const;
			//Get the current volume of audio frame in DB
			FF_EXP double get_audio_frame_volume_db() const;
			//Get the current timestamp of packet
			FF_EXP int64_t w_ffmpeg::get_packet_pts() const;

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
//			HRESULT* _copy_video_frame_to(_In_ ID3D11DeviceContext1* pContext, _Inout_ ID3D11Resource* pResource
//#ifdef __QT_DLL__
//				, bool pCopyToDirectXBuffer,
//				QImage& pQImage, bool pStoreInQImage, int pQTDownSampleScale
//#endif
//				);
			//Copy audio frame to the buffer
			HRESULT _copy_audio_frame_to(uint8_t* pBuffer, int& pBufferSize);
			void _update_clock(Codec& pCodec);
			void _update_time(double pclock);

			std::string									name;
			bool										isReleased;

			std::unique_ptr<AVFrame>					_av_Frame;
			std::unique_ptr<AVFrame>					_down_sample_avFrame;

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

			wolf::system::w_time_span					durationTime;
			wolf::system::w_time_span					elapsedTime;
			wolf::system::w_time_span					remainedTime;

			size_t										totalVideoFrames;
			byte										durationVideoFrames;
			byte										elapsedVideoFrames;
			byte										remainedVideoFrames;

			float										frameRate;
			double										audioframeVolumeDB;
		};
	}
}


#endif