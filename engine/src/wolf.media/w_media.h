/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_media.h
	Description		 : the main media header of wolf
	Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <wolf.h>
#include "w_media_export.h"
#include <memory/w_mem_pool.h>

	struct w_media_converter_t;
	typedef struct w_media_converter_t* w_media_converter;

	typedef struct w_video_frame_info_t
	{
		int64_t			index;
		uint32_t        width;
		uint32_t        height;
		int				format;
		int				line_size;
		int				pts;
	} w_video_frame_info_t;
	typedef w_video_frame_info_t* w_video_frame_info;

	typedef struct w_audio_frame_info_t
	{
		int64_t			index;
		int64_t			channel_layout;
		int				sample_rate;
		int				format;
		int				line_size;
		int				nb_samples;
		int				pts;
	} w_audio_frame_info_t;
	typedef w_audio_frame_info_t* w_audio_frame_info;

	typedef void w_media_connection_callback(const char* /*url*/);
	typedef void w_media_video_frame_rcv_callback(const w_video_frame_info /*pFrameInfo*/, const uint8_t* /*pFrameBuffer*/);
	typedef void w_media_audio_frame_rcv_callback(const w_audio_frame_info /*pFrameInfo*/, const uint8_t* /*pFrameBuffer*/);

	/**
	 * initialize wolf media core
	 * @return W_RESULT the result code
	 * @note must be called once before using w_media functions
	*/
	W_MEDIA_EXPORT
		W_RESULT w_media_init(
			_Inout_ w_mem_pool pMemPool,
			_Inout_ bool pRegisterNetwork);

	/*
	* open a stream reciever
	* @param pMemPool The pool to allocate out of
	* @param pURL, the rtsp connection
	* @param pProtocol, "tcp" or "udp"
	* @param pFormatName, format of streaming, e.g. "rtsp", "udp"
	* @param pStreamTimeOut, the stream timeout in seconds
	* @param pSocketTimeOut, ths socket timeout in seconds
	* @param pListen, listen to local port as puller
	* @param pOnConnectionEstablished, will rise when connection esablished.
	* @param pOnConnectionLost, will rise when connection lost.
	* @param pOnConnectionClosed, will rise when connection closed.
	* @param pOnVideoFrameRecieved, will rise when video frame is ready
	* @param pOnAudioFrameRecieved, will rise when audio frame is ready
	*/
	W_MEDIA_EXPORT
		W_RESULT w_media_open_stream_receiver(
			_In_ w_mem_pool pMemPool,
			_In_z_ const char* pURL,
			_In_z_ const char* pProtocol,
			_In_z_ const char* pFormatName,
			_In_ long long pStreamTimeOut,
			_In_ long long pSocketTimeOut,
			_In_ bool pListen,
			_In_opt_ w_media_connection_callback pOnConnectionEstablished,
			_In_opt_ w_media_connection_callback pOnConnectionLost,
			_In_opt_ w_media_connection_callback pOnConnectionClosed,
			_In_opt_ w_media_video_frame_rcv_callback pOnVideoFrameRecieved,
			_In_opt_ w_media_audio_frame_rcv_callback pOnAudioFrameRecieved);

   /*
    * convert video
    * @param pMemPool The pool to allocate out of
    * @param pSrcFrameInfo, the source video frame info
    * @param pDstFrameInfo, the destination video frame info
    * @param pSrcFrameBuffer, the source video frame buffer
    * @param pDstFrameBuffer, the destination video frame buffer
    * @param pMediaConverter, pointer to media converter
	* @return number of converted bytes
    */
	W_MEDIA_EXPORT
		int w_media_convert_video(
			_In_ w_mem_pool pMemPool,
			_In_ w_video_frame_info pSrcFrameInfo,
			_In_ w_video_frame_info pDstFrameInfo,
			_In_ const uint8_t* const pSrcFrameBuffer,
			_Inout_ uint8_t** pDstFrameBuffer,
			_Inout_ w_media_converter* pMediaConverter);

   /*
	* convert audio
	* @param pMemPool The pool to allocate out of
	* @param pSrcFrameInfo, the source audio audio frame info
	* @param pDstFrameInfo, the destination audio frame info
	* @param pSrcFrameBuffer, the source audio frame buffer
	* @param pDstFrameBuffer, the destination audio frame buffer
	* @param pMediaConverter, pointer to media converter
	* @return number of converted bytes
	*/
	W_MEDIA_EXPORT
		int w_media_convert_audio(
			_In_ w_mem_pool pMemPool,
			_In_ w_audio_frame_info pSrcFrameInfo,
			_In_ w_audio_frame_info pDstFrameInfo,
			_In_ const uint8_t* const pSrcFrameBuffer,
			_Inout_ uint8_t** pDstFrameBuffer,
			_Inout_ w_media_converter* pMediaConverter);

   /**
	* shudown wolf media core 
	* @return W_RESULT the result code
	*/
	W_MEDIA_EXPORT
		W_RESULT w_media_fini(void);


#ifdef __cplusplus
}
#endif

//#include <w_time_span.h>
//#include "w_memory_pool.h"
//#include <stdint.h>
//#include <memory>
//#include <array>
//#include <w_signal.h>
//

//
////#if defined(__1080p__)
////#define VIDEO_FRAME_SIZE					1920 * 1080
////#elif defined (__720p__)
////#define VIDEO_FRAME_SIZE					1280 * 720
////#elif defined (__PAL__)
////#define VIDEO_FRAME_SIZE					720 * 576
////#elif defined (__NTSC__)
////#define VIDEO_FRAME_SIZE					720 * 480
////#elif defined (__480p__)
////#define VIDEO_FRAME_SIZE					640 * 480
////#elif defined (__360p__)
////#define VIDEO_FRAME_SIZE					480 * 360
////#endif
////
////#define VIDEO_FRAME_DOWN_SAMPLE_SIZE		VIDEO_FRAME_SIZE / 2
////#define AUDIO_FRAME_SIZE		            192000// = 1 second of 48khz 32bit audio
//
//namespace wolf
//{
//	namespace framework
//	{
//        class w_media_core_pimp;
//		class w_media_core
//		{
//		public:
//			enum w_video_channels { R, G, B, A };
//            struct w_stream_connection_info
//            {
//                const char*         url;
//                AVFormatContext*    context;
//                AVStream*           stream;
//            };

//
//
//			//Open a media 
//            WMC_EXP W_RESULT open_media(_In_z_ std::wstring pMediaPath, _In_ int64_t pSeekToFrame = 0);
//
//            /*
//                Open a stream sender
//                This function will wait for client to make a connection
//                For testing, use ffplay, e.g. ./ffplay -rtsp_flags listen -i rtsp://127.0.0.1:8554/live.sdp
//                @param pURL, the connection url
//				@param pRTSPProtocol, "tcp" or "udp", leave nullptr for rtmp
//                @param pFormatName, format of streaming, e.g. "rtsp", "udp"
//                @param pCodecID, codec of streaming, e.g. "AV_CODEC_ID_H264"
//                @param pFrameRate, streaming frame rate, e.g. "25", "60"
//				@param pBitRate, bitrate, e.g. 7500 * 1000 for 720p (48,50,60 fps) and 5000 * 1000 for 720p (24, 25, 30) https://support.google.com/youtube/answer/1722171?hl=en
//                @param pPixelFormat, streaming pixel format, e.g. "AV_PIX_FMT_YUV420P"
//                @param pWidth, frame width
//                @param pHeight, frame height
//                @param pOnConnectionEstablished, rised when connection esablished. The argument is w_stream_connection_info  
//                @param pOnFillingVideoFrameBuffer, rised for each filling video frame buffer. The argument is w_stream_frame_info
//                @param pOnConnectionLost, rised when connection lost. The argument is w_stream_connection_info
//            */
//            WMC_EXP W_RESULT open_stream_sender(
//                _In_z_ const char* pURL,
//				_In_z_ const char* pRTSPProtocol,
//                _In_z_ const char* pFormatName,
//                _In_ const AVCodecID& pCodecID,
//                _In_ const int64_t& pFrameRate,
//				_In_ const int64_t& pBitRate,
//                _In_ const AVPixelFormat& pPixelFormat,
//                _In_ const uint32_t& pWidth,
//                _In_ const uint32_t& pHeight,
//                _In_ system::w_signal<void(const w_stream_connection_info&)>& pOnConnectionEstablished,
//                _In_ system::w_signal<void(const w_stream_frame_info&)>& pOnFillingVideoFrameBuffer,
//                _In_ system::w_signal<void(const char*)>& pOnConnectionLost);
//
//			
//
//			//Convert specific milliseconds to frame number
//            WMC_EXP int64_t time_to_frame(int64_t pMilliSecond);
//
//			/*
//				Seek to specific frame of media
//				return value =
//					1 = OK
//					0 = Out of range
//					-1 = Video Error
//					-2 = Audio Error
//					-3 = Both Video & Audio Error
//			*/
//            WMC_EXP int seek_frame(int64_t pFrame);
//
//			/*
//				Seek to specific milliseconds time of media
//				return value =
//					1 = OK
//					0 = Out of range
//					-1 = Video Error
//					-2 = Audio Error
//					-3 = Both Video & Audio Error
//			*/
//            WMC_EXP int seek_frame_milliSecond(int64_t pMilliSecond);
//
//			/*
//				Store video frames data in to the memory pool:
//				@param	pMemory, memory pool for storing
//				@param	pDownSamplingScale, down sampling value, 1 is default value, send greater than 1 for downsampling
//				@param	pBGRA_or_RGBA, Number of samples of the image; The value greater than 1 means multisampling
//				@return W_PASSED when succeeded 
//			*/
//			WMC_EXP W_RESULT write_video_frame_to_buffer(
//				wolf::system::w_memory_pool& pMemory,
//				uint32_t pDownSamplingScale = 1,
//				bool pBGRA_or_RGBA = true);
//
//			//Store audio frame data in to the memory
//            //WMC_EXP W_RESULT buffer_audio_to_memory(wolf::system::w_memory& pAudioMemory, double& pAudioFrameVolumeDB);
//
//			//Read all data in to the memory, make sure the capacity size of video and audio chuck is equal 
//            //WMC_EXP W_RESULT buffer_to_memory(wolf::system::w_memory& pVideoMemory, wolf::system::w_memory& pAudioMemory);
//
//			//release all resources
//            WMC_EXP ULONG release();
//
//            //release media resources
//            WMC_EXP ULONG release_media();
//
//			//release output stream server resources
//			WMC_EXP ULONG release_output_stream_server();
//
//#pragma region Getters
//
//			//Is the media opened successfully
//            WMC_EXP bool is_open() const;
//			//Get theb type of this media
//            WMC_EXP const char* get_media_path() const;
//
//			//Get the time of media
//            WMC_EXP wolf::system::w_time_span get_duration_time() const;
//			//Get the elpased time of media
//            WMC_EXP wolf::system::w_time_span get_elapsed_time() const;
//			//Get the remained time of media
//            WMC_EXP wolf::system::w_time_span get_remained_time() const;
//
//			//Get number of total video frames
//            WMC_EXP size_t get_total_video_frames() const;
//			//Get number of duration frames
//            WMC_EXP uint8_t get_duration_video_frames() const;
//			//Get number of elpased frames 
//            WMC_EXP uint8_t get_elapsed_video_frames() const;
//			//Get number of remained frames 
//            WMC_EXP uint8_t get_remained_video_frames() const;
//
//			//Get the frame per seconds of video
//            WMC_EXP float get_video_frame_rate() const;
//			//Get the ticks per frame of audio
//            WMC_EXP float get_audio_frame_rate() const;
//
//			//Get the bit rate of media
//            WMC_EXP int get_bit_rate() const;
//			//Get the total channels of video
//            WMC_EXP int get_video_channels() const;
//			//Get the sample rate of video
//            WMC_EXP int get_video_sample_rate() const;
//			//Get the width of video frame
//            WMC_EXP int get_video_frame_width() const;
//			//Get the height of video frame
//            WMC_EXP int get_video_frame_height() const;
//			//Get the total channels of audio
//            WMC_EXP int get_audio_channels() const;
//			//Get the sample rate of audio
//            WMC_EXP int get_audio_sample_rate() const;
//			//Get the current volume of audio frame in DB
//            WMC_EXP double get_audio_frame_volume_db() const;
//			//Get the current timestamp of packet
//            WMC_EXP int64_t get_packet_pts() const;
//
//#pragma endregion
//
//		private:
//			bool													_is_released;
//            w_media_core_pimp*                                      _pimp;
//		};
//	}
//}
