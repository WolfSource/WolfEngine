/*
    Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website			 : http://WolfSource.io
    Name			 : w_media_core.h
    Description		 : a media player and streamer class
    Comment          :
*/

#ifndef __W_MEDIA_CORE_H__
#define __W_MEDIA_CORE_H__

#include "w_media_core_export.h"

#include <w_object.h>
#include <w_time_span.h>
#include "w_memory_pool.h"
#include <stdint.h>
#include <memory>
#include <array>
#include <w_signal.h>

#ifdef __WIN32
#pragma warning(disable:4996)//for deprecated ffmpeg objects
#endif

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libswresample/swresample.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/samplefmt.h>
}

//#if defined(__1080p__)
//#define VIDEO_FRAME_SIZE					1920 * 1080
//#elif defined (__720p__)
//#define VIDEO_FRAME_SIZE					1280 * 720
//#elif defined (__PAL__)
//#define VIDEO_FRAME_SIZE					720 * 576
//#elif defined (__NTSC__)
//#define VIDEO_FRAME_SIZE					720 * 480
//#elif defined (__480p__)
//#define VIDEO_FRAME_SIZE					640 * 480
//#elif defined (__360p__)
//#define VIDEO_FRAME_SIZE					480 * 360
//#endif
//
//#define VIDEO_FRAME_DOWN_SAMPLE_SIZE		VIDEO_FRAME_SIZE / 2
//#define AUDIO_FRAME_SIZE		            192000// = 1 second of 48khz 32bit audio

namespace wolf
{
	namespace framework
	{
        class w_media_core_pimp;
		class w_media_core : public system::w_object
		{
		public:
			enum w_video_channels { R, G, B, A };
            struct w_stream_connection_info
            {
                const char*         url;
                AVFormatContext*    context;
                AVStream*           stream;
            };
            struct w_stream_frame_info
            {
				uint8_t*		pixels = nullptr;
				uint32_t		stride = 3;
                uint32_t        width = 0;
                uint32_t        height = 0;
                long long       index = 0;
                long long       stream_duration = 0;
                long long       frame_duration = 0;
				long long		last_dropped_frames = 0;
            };

			//Must be call once before using class
            WMC_EXP static void register_all(_In_ const bool& pRegisterNetwork);
			//Must be call once for shuting statics objects down 
            WMC_EXP static void shut_down();
            
			//Make sure to call ffmpeg::InitializeMF(); once before creating instance
            WMC_EXP w_media_core();
			//Make sure to call ffmpeg::ReleaseMF(); once before releasing any instance
            WMC_EXP virtual ~w_media_core();

			//Open a media 
            WMC_EXP W_RESULT open_media(_In_z_ std::wstring pMediaPath, _In_ int64_t pSeekToFrame = 0);

            /*
                Open a stream server
                This function will wait for client to make a connection
                For testing, use ffplay, e.g. ./ffplay -rtsp_flags listen -i rtsp://127.0.0.1:8554/live.sdp
                @param pURL, the connection url
				@param pProtocol, "tcp" or "udp"
                @param pFormatName, format of streaming, e.g. "rtsp", "udp"
                @param pCodecID, codec of streaming, e.g. "AV_CODEC_ID_H264"
                @param pFrameRate, streaming frame rate, e.g. "25", "60"
				@param pBitRate, bitrate, e.g. 7500 * 1000 for 720p (48,50,60 fps) and 5000 * 1000 for 720p (24, 25, 30) https://support.google.com/youtube/answer/1722171?hl=en
                @param pPixelFormat, streaming pixel format, e.g. "AV_PIX_FMT_YUV420P"
                @param pWidth, frame width
                @param pHeight, frame height
                @param pOnConnectionEstablished, rised when connection esablished. The argument is w_stream_connection_info  
                @param pOnFillingVideoFrameBuffer, rised for each filling video frame buffer. The argument is w_stream_frame_info
                @param pOnConnectionLost, rised when connection lost. The argument is w_stream_connection_info
            */
            WMC_EXP W_RESULT open_stream_server(
                _In_z_ const char* pURL,
				_In_z_ const char* pProtocol,
                _In_z_ const char* pFormatName,
                _In_ const AVCodecID& pCodecID,
                _In_ const int64_t& pFrameRate,
				_In_ const int64_t& pBitRate,
                _In_ const AVPixelFormat& pPixelFormat,
                _In_ const uint32_t& pWidth,
                _In_ const uint32_t& pHeight,
                _In_ system::w_signal<void(const w_stream_connection_info&)>& pOnConnectionEstablished,
                _In_ system::w_signal<void(const w_stream_frame_info&)>& pOnFillingVideoFrameBuffer,
                _In_ system::w_signal<void(const char*)>& pOnConnectionLost);

			/*
				Open a stream client
				This function will wait for client to make a connection
				For testing, use ffplay, e.g. ./ffplay -rtsp_flags listen -i rtsp://127.0.0.1:8554/live.sdp
				@param pURL, the connection url
				@param pProtocol, "tcp" or "udp"
				@param pFormatName, format of streaming, e.g. "rtsp", "udp"
				@param pCodecID, codec of streaming, e.g. "AV_CODEC_ID_H264"
				@param pPixelFormat, streaming pixel format, e.g. "AV_PIX_FMT_YUV420P"
				@param pWidth, frame width
				@param pHeight, frame height
				@param pOnConnectionEstablished, rised when connection esablished. The argument is w_stream_connection_info
				@param pOnGettingStreamVideoFrame, rised for each filling video frame buffer. The argument is w_stream_frame_info
				@param pOnConnectionLost, rised when connection lost. The argument is w_stream_connection_info
			*/
			WMC_EXP W_RESULT open_stream_client(
				_In_z_ const char* pURL,
				_In_z_ const char* pProtocol,
				_In_z_ const char* pFormatName,
				_In_ const AVCodecID& pCodecID,
				_In_ const AVPixelFormat& pPixelFormat,
				_In_ const uint32_t& pWidth,
				_In_ const uint32_t& pHeight,
				_In_ const long long& pTimeOut,
				_In_ system::w_signal<void(const w_stream_connection_info&)>& pOnConnectionEstablished,
				_In_ system::w_signal<void(const w_stream_frame_info&)>& pOnGettingStreamVideoFrame,
				_In_ system::w_signal<void(const char*)>& pOnConnectionLost,
				_In_ system::w_signal<void(const char*)>& pOnConnectionClosed);

			//Convert specific milliseconds to frame number
            WMC_EXP int64_t time_to_frame(int64_t pMilliSecond);

			/*
				Seek to specific frame of media
				return value =
					1 = OK
					0 = Out of range
					-1 = Video Error
					-2 = Audio Error
					-3 = Both Video & Audio Error
			*/
            WMC_EXP int seek_frame(int64_t pFrame);

			/*
				Seek to specific milliseconds time of media
				return value =
					1 = OK
					0 = Out of range
					-1 = Video Error
					-2 = Audio Error
					-3 = Both Video & Audio Error
			*/
            WMC_EXP int seek_frame_milliSecond(int64_t pMilliSecond);

			/*
				Store video frames data in to the memory pool:
				@param	pMemory, memory pool for storing
				@param	pDownSamplingScale, down sampling value, 1 is default value, send greater than 1 for downsampling
				@param	pBGRA_or_RGBA, Number of samples of the image; The value greater than 1 means multisampling
				@return W_PASSED when succeeded 
			*/
			WMC_EXP W_RESULT write_video_frame_to_buffer(
				wolf::system::w_memory_pool& pMemory,
				uint32_t pDownSamplingScale = 1,
				bool pBGRA_or_RGBA = true);

			//Store audio frame data in to the memory
            //WMC_EXP W_RESULT buffer_audio_to_memory(wolf::system::w_memory& pAudioMemory, double& pAudioFrameVolumeDB);

			//Read all data in to the memory, make sure the capacity size of video and audio chuck is equal 
            //WMC_EXP W_RESULT buffer_to_memory(wolf::system::w_memory& pVideoMemory, wolf::system::w_memory& pAudioMemory);

			//release all resources
            WMC_EXP ULONG release();

            //release media resources
            WMC_EXP ULONG release_media();

			//release output stream server resources
			WMC_EXP ULONG release_output_stream_server();

#pragma region Getters

			//Is the media opened successfully
            WMC_EXP bool is_open() const;
			//Get theb type of this media
            WMC_EXP const char* get_media_path() const;

			//Get the time of media
            WMC_EXP wolf::system::w_time_span get_duration_time() const;
			//Get the elpased time of media
            WMC_EXP wolf::system::w_time_span get_elapsed_time() const;
			//Get the remained time of media
            WMC_EXP wolf::system::w_time_span get_remained_time() const;

			//Get number of total video frames
            WMC_EXP size_t get_total_video_frames() const;
			//Get number of duration frames
            WMC_EXP uint8_t get_duration_video_frames() const;
			//Get number of elpased frames 
            WMC_EXP uint8_t get_elapsed_video_frames() const;
			//Get number of remained frames 
            WMC_EXP uint8_t get_remained_video_frames() const;

			//Get the frame per seconds of video
            WMC_EXP float get_video_frame_rate() const;
			//Get the ticks per frame of audio
            WMC_EXP float get_audio_frame_rate() const;

			//Get the bit rate of media
            WMC_EXP int get_bit_rate() const;
			//Get the total channels of video
            WMC_EXP int get_video_channels() const;
			//Get the sample rate of video
            WMC_EXP int get_video_sample_rate() const;
			//Get the width of video frame
            WMC_EXP int get_video_frame_width() const;
			//Get the height of video frame
            WMC_EXP int get_video_frame_height() const;
			//Get the total channels of audio
            WMC_EXP int get_audio_channels() const;
			//Get the sample rate of audio
            WMC_EXP int get_audio_sample_rate() const;
			//Get the current volume of audio frame in DB
            WMC_EXP double get_audio_frame_volume_db() const;
			//Get the current timestamp of packet
            WMC_EXP int64_t get_packet_pts() const;

#pragma endregion

		private:
            typedef	system::w_object                                _super;
            w_media_core_pimp*                                      _pimp;
		};
	}
}


#endif