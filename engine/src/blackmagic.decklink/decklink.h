/*
	Project			 : Wolf Engine (http://WolfSource.io). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to contact@WolfSource.io
	Name			 : decklink.h
	Description		 : The decklink interface header
	Comment          :
*/

#ifndef __DECKLINK_H__
#define __DECKLINK_H__

#include "DeckLinkAPI_h.h"
#include "blackmagic_decklink_dll.h"
#include <w_object.h>
#include <guiddef.h>
#include <Unknwnbase.h>
#include <vector>
#include <memory>
#include <map>
#include "time_code.h"
#include <tbb/scalable_allocator.h>
#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_queue.h>
#include <tbb/critical_section.h>

#pragma comment(lib, "Wolf.System.Win32.lib")

#ifdef _DEBUG
#pragma comment(lib, "tbb_debug.lib")
#pragma comment(lib, "tbbmalloc_debug.lib")
#else 
#pragma comment(lib, "tbb.lib")
#pragma comment(lib, "tbbmalloc.lib")
#endif

class decklink : public wolf::system::w_object, IDeckLinkVideoOutputCallback, IDeckLinkAudioOutputCallback, IDeckLinkScreenPreviewCallback
{
public:
	DCK_EXP decklink();
	DCK_EXP virtual ~decklink();

	//initialize decklink device card
	DCK_EXP HRESULT initialize();
	//start decklink device card
	DCK_EXP HRESULT start_output();
	//stop decklink device card
	DCK_EXP HRESULT stop_output();
	//start capturing from decklink input
	DCK_EXP HRESULT start_capturing();
	//stop capturing from decklink input
	DCK_EXP HRESULT stop_capturing();

	//set video frame
	DCK_EXP void set_video_frame(uint8_t* pData);
	//set audio frame
	DCK_EXP void set_audio_sample_frame(_In_ uint8_t* pSamples, const UINT pSamplesCount);
	//clear queue of video frames
	DCK_EXP void clear_video_queue();
	//Use this method to release and free decklink device
	DCK_EXP void free();

	//Implemented for IDeckLinkVideoOutputCallback
	virtual HRESULT STDMETHODCALLTYPE DrawFrame(IDeckLinkVideoFrame* pFrame);
	//Implemented for IDeckLinkVideoOutputCallback
	virtual HRESULT STDMETHODCALLTYPE ScheduledFrameCompleted(IDeckLinkVideoFrame* pCompletedFrame, BMDOutputFrameCompletionResult pResult);
	//Implemented for IDeckLinkVideoOutputCallback
	virtual HRESULT STDMETHODCALLTYPE ScheduledPlaybackHasStopped();

	//Implementation for IDeckLinkAudioOutputCallback
	virtual HRESULT STDMETHODCALLTYPE RenderAudioSamples(BOOL pPreroll);

	//Dummy implementation 		
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID* pLPVOID);
	virtual ULONG STDMETHODCALLTYPE	AddRef();
	virtual ULONG STDMETHODCALLTYPE	Release() { return 0; };
	
#pragma region Getters

	//get whether license is valid
	DCK_EXP const bool get_is_valid_license() const;
	//get supported display modes
	DCK_EXP std::vector<std::string> get_supported_display_modes() const			{ return this->_supported_output_display_modes; }
	//get decklink device information
	DCK_EXP const char* get_decklink_device_info() const							{ return this->_device_info.c_str(); }
	//get frame flag
	DCK_EXP _BMDFrameFlags get_frame_flag() const									{ return this->_frame_flag; }

#pragma endregion

#pragma region Setters

	//set frame flag
	DCK_EXP void set_frame_flag(_BMDFrameFlags pValue)								{ this->_frame_flag = pValue; }

#pragma endregion

private:
	typedef		wolf::system::w_object											_super;

	//get decklink information
	void		_get_decklink_information();
	void		_get_output_supported_display_modes();
	void		_get_input_supported_display_modes();
	void		_schedule_next_frame(bool pPreroll);


	IDeckLinkMutableVideoFrame*													_video_frame;
	bool																		_is_running;
	bool																		_support_input_format_detection;

	_BMDFrameFlags																_frame_flag;
	time_code*																	_time_code;

	uint32_t																	_display_mode_width;
	uint32_t																	_display_mode_height;

	uint32_t																	_fps;
	uint32_t																	_total_frames_scheduled;
	uint32_t																	_total_audio_seconds_scheduled;
	tbb::atomic<uint32_t>														_audio_samples_per_frame;
	uint32_t																	_audio_buffer_sample_length;

	IDeckLink*																	_deckLink_interface;
	IDeckLinkInput*																_deckLink_input;
	IDeckLinkOutput*															_decklink_output;
	IDeckLinkIterator*															_decklink_iterator;

	tbb::concurrent_queue<uint8_t*>												_video_frames_queue;

	IDeckLinkKeyer*																_decklink_keyer;
	_BMDAudioSampleRate															_audio_sample_rate;
	BMDTimeValue																_frame_duration;
	BMDTimeScale																_frame_time_scale;
	
	std::map<std::string, IDeckLinkDisplayMode*>								_output_display_modes;
	std::vector<std::string>													_supported_output_display_modes;

	std::map<std::string, IDeckLinkDisplayMode*>								_input_display_modes;
	std::vector<std::string>													_supported_input_display_modes;

	std::string																	_device_info;
	size_t																		_buffer_size;
	BMDTimecodeFormat															_time_code_format;

};

#endif