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

//Forward declaration
class time_code;

class decklink : public wolf::system::w_object, IDeckLinkVideoOutputCallback, IDeckLinkAudioOutputCallback, IDeckLinkScreenPreviewCallback
{
public:
	DLL decklink();
	DLL virtual ~decklink();

	DLL HRESULT initialize();
	DLL HRESULT start();
	DLL HRESULT stop();

	DLL void set_video_frame(uint8_t* pData);
	DLL void set_audio_sample_frame(_In_ uint8_t* pSamples, const UINT pSamplesCount);
	DLL void clear_video_queue();
	//Use this method to release and free decklink device
	DLL void free();

	//Implementation for IDeckLinkVideoOutputCallback
	virtual HRESULT STDMETHODCALLTYPE DrawFrame(IDeckLinkVideoFrame* pFrame);
	virtual HRESULT STDMETHODCALLTYPE ScheduledFrameCompleted(IDeckLinkVideoFrame* pCompletedFrame, BMDOutputFrameCompletionResult pResult);
	virtual HRESULT STDMETHODCALLTYPE ScheduledPlaybackHasStopped();

	//Implementation for IDeckLinkAudioOutputCallback
	virtual HRESULT STDMETHODCALLTYPE RenderAudioSamples(BOOL pPreroll);

	//Dummy implementation 		
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID* pLPVOID);
	virtual ULONG STDMETHODCALLTYPE	AddRef();
	virtual ULONG STDMETHODCALLTYPE	Release();

#pragma region Getters

	DLL const bool get_is_valid_license() const;
	DLL std::vector<std::wstring> get_supported_display_modes() const			{ return this->supportedDisplayModes; }
	DLL std::wstring get_decklink_device_info() const							{ return this->deviceInfo; }
	DLL _BMDFrameFlags get_frame_flag() const									{ return this->_frame_flag; }

#pragma endregion

#pragma region Setters

	DLL void set_frame_flag(_BMDFrameFlags pValue)								{ this->_frame_flag = pValue; }

#pragma endregion

private:
	void _schedule_next_frame(bool pPreroll);
	void _release_decklink_iterator(IDeckLinkIterator* pIDeckLinkIterator);

	IDeckLinkMutableVideoFrame*									videoFrame;
	IDeckLinkMutableVideoFrame*									blackFrame;

	bool														isReleased;
	std::string													name;
	bool														isRunning;
	std::vector<uint8_t>										remained;

	_BMDFrameFlags												_frame_flag;
	time_code*													timeCode;

	//DeckLink
	uint32_t													width;
	uint32_t													height;
	uint32_t													framesPerSecond;
	uint32_t													totalFramesScheduled;
	uint32_t													totalAudioSecondsScheduled;
	tbb::atomic<uint32_t>										audioSamplesPerFrame;
	uint32_t													audioBufferSampleLength;

	IDeckLink*													_ideckLink;

	tbb::concurrent_queue<uint8_t*>								videoFramesQueue;
	//tbb::concurrent_queue<std::tuple<uint8_t*, UINT>>			audioSamplesQueue;

	IDeckLinkOutput*											output;
	_BMDAudioSampleRate											audioSampleRate;
	BMDTimeValue												frameDuration;
	BMDTimeScale												frameTimescale;
	std::map<std::wstring, IDeckLinkDisplayMode*>				displayModes;
	std::vector<std::wstring>									supportedDisplayModes;
	std::wstring												deviceInfo;
	size_t														bufferSize;
	BMDTimecodeFormat											timeCodeFormat;

};

class time_code
{
public:
	time_code(int f) : _fps(f), _frames(0), _seconds(0), _minutes(0), _hours(0) { }
	void update()
	{
		if (this->_frames >= (unsigned)this->_fps - 1)
		{
			this->_frames = 0;
			this->_seconds++;
		}
		else
		{
			this->_frames++;
		}

		if (this->_seconds >= 60)
		{
			this->_seconds = 0;
			this->_minutes++;
		}

		if (this->_minutes >= 60)
		{
			this->_minutes = 0;
			this->_hours++;
		}
		if (this->_hours >= 24)
		{
			this->_frames = 0;
			this->_seconds = 0;
			this->_minutes = 0;
			this->_hours = 0;
		}
	}
	int hours() { return this->_hours; }
	int minutes() { return this->_minutes; }
	int seconds() { return this->_seconds; }
	int frames() { return this->_frames; }

private:
	int _fps;
	unsigned long _frames;
	int _seconds;
	int _minutes;
	int _hours;
};

#endif