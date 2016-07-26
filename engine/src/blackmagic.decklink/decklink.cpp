#include "decklink.h"
#include <w_logger.h>
#include <stdint.h>

#if defined (__1080p_50__)

#define PREFERED_DISPLAY_MODE L"1080p50"

#elif defined (__1080_25p__)

#define PREFERED_DISPLAY_MODE L"1080p25"

#elif defined (__PAL__)

#define PREFERED_DISPLAY_MODE L"PAL"

#elif defined (__NTSC__)

#define PREFERED_DISPLAY_MODE L"NTSC"

#endif

#ifndef DEV
//#define DEV
#endif

#define SN1 1214254596
#define SN2 1482879052
#define SN3 1499467268
#define SN4 1747073789
#define SN5 1153437188
#define SN6 2103447044
#define SN7 350484452
#define SN8 1482690052

#define MAX_PREROLL 3

using namespace std;
using namespace wolf::system;

static bool sInValidLicense = true;

decklink::decklink() : 
	isRunning(false), 
	_frame_flag(_BMDFrameFlags::bmdFrameFlagDefault),
	audioSampleRate(bmdAudioSampleRate48kHz), 
	_ideckLink(nullptr), 
	output(nullptr), 
	timeCode(nullptr), 
	isReleased(false)
{
	this->name = typeid(this).name();
}

decklink::~decklink()
{
	free();
}

HRESULT decklink::initialize()
{
	this->deviceInfo.clear();

	IDeckLinkIterator* _iterator = nullptr;

	auto _hr = CoInitialize(NULL);
	V(_hr, "CoInitialize", this->name);

	_hr = CoCreateInstance(CLSID_CDeckLinkIterator, NULL, CLSCTX_ALL, IID_IDeckLinkIterator, (void**)&_iterator);
	//On problem happens first release the iDLIterator then release this class and finally write log and close the app
	if (_hr != S_OK)
	{
		string _msg = "BlackMagic DeckLink drivers not found";
		_release_decklink_iterator(_iterator);
		free();
		MessageBoxA(NULL, _msg.c_str(), "WPlayout", MB_OK);
		V(_hr, _msg, this->name);
		return S_FALSE;
	}

	_hr = _iterator->Next(&this->_ideckLink);
	if (_hr == S_FALSE)
	{
		_release_decklink_iterator(_iterator);
		free();
		return S_FALSE;
	}
	V(_hr, "BlackMagic DeckLink device is not installed", this->name, true);

	_hr = this->_ideckLink->QueryInterface(IID_IDeckLinkOutput, (void**)&this->output);
	if (_hr == S_FALSE)
	{
		_release_decklink_iterator(_iterator);
		free();
		return S_FALSE;
	}
	V(_hr, "getting output for DeckLink", this->name, true);

	//Get the name of deckLink
	BSTR _deckLinkModulName = nullptr;
	_hr = this->_ideckLink->GetModelName(&_deckLinkModulName);
	if (_hr == S_FALSE)  return _hr;

#pragma region Get decklink informarion

	IDeckLinkAPIInformation* _decklink_api_information = nullptr;
	IDeckLinkAttributes* _decklink_attributes = nullptr;
	_hr = _iterator->QueryInterface(IID_IDeckLinkAPIInformation, (void**)&_decklink_api_information);
	if (_hr == S_OK)
	{
		LONGLONG		_deckLinkVersion;
		int				_dlVerMajor, _dlVerMinor, _dlVerPoint;

		// We can also use the BMDDeckLinkAPIVersion flag with GetString
		_decklink_api_information->GetInt(BMDDeckLinkAPIVersion, &_deckLinkVersion);

		_dlVerMajor = (_deckLinkVersion & 0xFF000000) >> 24;
		_dlVerMinor = (_deckLinkVersion & 0x00FF0000) >> 16;
		_dlVerPoint = (_deckLinkVersion & 0x0000FF00) >> 8;

		auto decklinkName = wstring(_deckLinkModulName);
		
		this->deviceInfo = L"Blackmagic decklink device : " + 
			wstring(decklinkName.begin(), decklinkName.end()) + L"\r\n";
		
		this->deviceInfo += L"DeckLinkAPI version : " + 
			std::to_wstring(_dlVerMajor) + L"," + std::to_wstring(_dlVerMinor) + L"," + std::to_wstring(_dlVerPoint);

		BSTR								_name = NULL;
		BOOL								_supported;
		LONGLONG							_value;

		// Query the DeckLink for its attributes interface
		_hr = this->_ideckLink->QueryInterface(IID_IDeckLinkAttributes, (void**)&_decklink_attributes);
		if (_hr != S_OK)
		{
			V(_hr, "Could not obtain the IDeckLinkAttributes interface", this->name);
			return S_FALSE;
		}

		// List attributes and their value
		_value = -1;
		_hr = _decklink_attributes->GetInt(BMDDeckLinkPersistentID, &_value);
		this->deviceInfo += L"\r\nDevice Persistent ID : ";
		if (_hr == S_OK)
		{
			this->deviceInfo += to_wstring(_value);
		}
		else
		{
			this->deviceInfo += L"Not Supported on this device";
		}

#ifndef DEV

		if (_value == -1 || !(_value == SN1 || _value == SN2 || _value == SN3 || _value == SN4 || _value == SN5 || _value == SN6 || _value == SN7 || _value == SN8))
		{
			logger.error("You need to register this software. Please contact support described in \"About\". Error code :" + std::to_string(_value));
			sInValidLicense = false;
			//Release();
			//return S_FALSE;
			////abort();
		}

#endif

		_hr = _decklink_attributes->GetInt(BMDDeckLinkTopologicalID, &_value);
		this->deviceInfo += L"\r\nDevice Topological ID : ";
		if (_hr == S_OK)
		{
			this->deviceInfo += to_wstring(_value);
		}
		else
		{
			this->deviceInfo += L"Not Supported on this device";
		}

		_hr = _decklink_attributes->GetInt(BMDDeckLinkNumberOfSubDevices, &_value);
		if (_hr == S_OK)
		{
			this->deviceInfo += L"\r\nNumber of sub-devices : " + to_wstring(_value);
			if (_value != 0)
			{
				_hr = _decklink_attributes->GetInt(BMDDeckLinkSubDeviceIndex, &_value);
				this->deviceInfo += L"\r\nSub-devices index : ";
				if (_hr == S_OK)
				{
					this->deviceInfo += to_wstring(_value);
				}
				else
				{
					this->deviceInfo += L"Could not query the sub-device index attribute";
				}
			}
		}
		else
		{
			this->deviceInfo += L"\r\nCould not query the number of sub-device attribute";
		}

		_hr = _decklink_attributes->GetInt(BMDDeckLinkMaximumAudioChannels, &_value);
		this->deviceInfo += L"\r\nMaximum number of audio channels: ";
		if (_hr == S_OK)
		{
			this->deviceInfo += to_wstring(_value);
		}
		else
		{
			this->deviceInfo += L"Could not query the number of audio channels";
		}
		_decklink_api_information->Release();

		this->deviceInfo += L"\r\n";
		logger.user(this->deviceInfo);
		//clear the decklinkName which defined as wstring
		decklinkName.clear();
	}
	else
	{
		logger.user("Could not get the information of decklink API");
	}

	_decklink_api_information = nullptr;

#pragma endregion

	//Create callback
	//hr = this->output->SetScreenPreviewCallback(this);
	_hr = this->output->SetScheduledFrameCompletionCallback(this);
	//hr = this->output->SetAudioCallback(this);

	return _hr;
}

HRESULT decklink::start()
{
	IDeckLinkDisplayModeIterator* _decklink_display_mode_iter = nullptr;

	//Todo : this must be config able from app
	_BMDPixelFormat _pixelFormat = bmdFormat8BitYUV;

	//clear all display modes
	this->displayModes.clear();
	this->supportedDisplayModes.clear();

	HRESULT _hr = S_FALSE;
	wstring _displayModesMsg = L"Supported display modes : \r\n";
	wstring _unsupportedDisplayModesMsg = L"UnSupported display modes : \r\n";
	logger.user(L"Deck link is going to start");

	if (this->output->GetDisplayModeIterator(&_decklink_display_mode_iter) == S_OK)
	{
		IDeckLinkDisplayMode* _dLDisplayMode = nullptr;
		while (_decklink_display_mode_iter->Next(&_dLDisplayMode) == S_OK)
		{
			BSTR					_modeName;
			BMDDisplayModeSupport	_displayModeSupport;
			BMDVideoOutputFlags		_videoOutputFlags = bmdVideoOutputDualStream3D;

			//If could not get the name then continiue
			if (_dLDisplayMode->GetName(&_modeName) != S_OK)
			{
				SysFreeString(_modeName);
				continue;
			}

			_hr = this->output->DoesSupportVideoMode(
				_dLDisplayMode->GetDisplayMode(),
				_pixelFormat,
				_videoOutputFlags,
				&_displayModeSupport,
				NULL);

			//Save the name
			auto _wModeName = wstring(_modeName);
			if (_displayModeSupport)
			{
				_unsupportedDisplayModesMsg.append(_wModeName).append(L" \r\n");
			}
			else
			{
				this->supportedDisplayModes.push_back(_wModeName);
				_displayModesMsg.append(_wModeName).append(L" \r\n");
			}

			//Save the name and the displayMode in the map
			this->displayModes[_wModeName] = _dLDisplayMode;
			_dLDisplayMode->AddRef();

			//Free the strings
			_wModeName.clear();
			SysFreeString(_modeName);
		}

		_dLDisplayMode = nullptr;
		_decklink_display_mode_iter->Release();
	}
	else
	{
		//Show message box and exit the appilication
		auto _msg = L"Couldn't find any display mode.";
		wstring _caption = L"DeckLink error.";
		logger.user(_msg);
		MessageBox(NULL, _msg, _caption.c_str(), MB_OK);
		_decklink_display_mode_iter->Release();
		free();
		V(S_FALSE, string(_caption.begin(), _caption.end()), this->name, true, false);
	}

	//Log the display modes
	logger.user(_displayModesMsg);
	logger.user(_unsupportedDisplayModesMsg);

	//Todo : This must change from setting
	if (displayModes.find(PREFERED_DISPLAY_MODE) == displayModes.end())
	{
		logger.user(L"Display mode " + std::wstring(PREFERED_DISPLAY_MODE) + L" is not supported");
		return S_FALSE;
	}

	auto selectedDisplayMode = displayModes[PREFERED_DISPLAY_MODE];
	this->width = selectedDisplayMode->GetWidth();
	this->height = selectedDisplayMode->GetHeight();
	this->bufferSize = this->width * this->height * 4;//4 = BGRA

	selectedDisplayMode->GetFrameRate(&frameDuration, &frameTimescale);
	framesPerSecond = ((frameTimescale + (frameDuration - 1)) / frameDuration);

	auto msg = std::wstring(L"Display mode FPS is " + std::to_wstring(framesPerSecond) + L", frame width is " + std::to_wstring(this->width) +
		L", frame height is " + std::to_wstring(this->height));
	logger.user(msg);

	if (this->timeCode)
	{
		delete this->timeCode;
	}
	this->timeCode = new time_code(framesPerSecond);

	this->totalAudioSecondsScheduled = 0;
	this->totalFramesScheduled = 0;

#pragma region Enable Video

	BMDVideoOutputFlags _videoOutputFlags;
	auto _videoDisplayMode = selectedDisplayMode->GetDisplayMode();
	if (_videoDisplayMode == bmdModeNTSC ||
		_videoDisplayMode == bmdModeNTSC2398 ||
		_videoDisplayMode == bmdModePAL)
	{
		this->timeCodeFormat = BMDTimecodeFormat::bmdTimecodeVITC;
		_videoOutputFlags = BMDVideoOutputFlags::bmdVideoOutputVITC;
	}
	else
	{
		this->timeCodeFormat = BMDTimecodeFormat::bmdTimecodeRP188Any;
		_videoOutputFlags = BMDVideoOutputFlags::bmdVideoOutputRP188;
	}

	if (this->output->EnableVideoOutput(_videoDisplayMode, _videoOutputFlags) != S_OK)
	{
		free();
		return S_FALSE;
	}

	if (this->output->CreateVideoFrame(this->width, this->height, this->width * 4, bmdFormat8BitBGRA, this->_frame_flag, &this->videoFrame) != S_OK)
	{
		free();
		return S_FALSE;
	}
	for (size_t i = 0; i < MAX_PREROLL; ++i)
	{
		auto _frame = (uint8_t*)scalable_malloc(this->bufferSize);
		std::fill(&_frame[0], &_frame[0] + this->bufferSize, 0);
		this->videoFramesQueue.push(_frame);
		_schedule_next_frame(true);
	}

#pragma endregion

#pragma region Enable Audio

	auto _audioChannelCount = 2;
	auto _audioSampleDepth = 16; // bmdAudioSampleType16bitInteger
	// Set the audio output mode, bmdAudioOutputStreamContinuous need for continusly fill buffer from my code, time step did not schedual
	if (this->output->EnableAudioOutput(this->audioSampleRate, bmdAudioSampleType16bitInteger, _audioChannelCount, bmdAudioOutputStreamContinuous) != S_OK)//bmdAudioOutputStreamTimestamped
	{
		free();
		return S_FALSE;
	}

	this->audioSamplesPerFrame = ((audioSampleRate * frameDuration) / frameTimescale);
	this->audioBufferSampleLength = (framesPerSecond * audioSampleRate * frameDuration) / frameTimescale;

#pragma endregion

	if (this->output->BeginAudioPreroll() != S_OK)
	{
		V(S_FALSE, "beginning pre-rolling audio frames in deckLink", this->name, true);
	}

	msg.clear();

	this->output->StartScheduledPlayback(0, this->frameTimescale, 1.0);

	this->isRunning = true;

	return S_OK;
}

HRESULT decklink::stop()
{
	if (!this->isRunning) return S_FALSE;

	this->output->StopScheduledPlayback(0, NULL, 0);

	this->output->DisableVideoOutput();
	this->output->DisableAudioOutput();

	this->isRunning = false;

	return S_OK;
}

void decklink::_schedule_next_frame(bool pPreroll)
{
	uint8_t* _frameOfQueue = nullptr;
	auto _hr = false;

	auto _size = this->videoFramesQueue.unsafe_size();
	if (_size > 0)
	{
		_hr = this->videoFramesQueue.try_pop(_frameOfQueue);
	}

	if (_hr && _frameOfQueue)
	{
		uint8_t* _frame = nullptr;
		this->videoFrame->GetBytes((void**)&_frame);

		if (_frame != nullptr)
		{
			//copy from queue to the video frame
			std::copy(&_frameOfQueue[0], &_frameOfQueue[0] + this->bufferSize, &_frame[0]);
		}

		scalable_free(_frameOfQueue);
	}
	else
	{
		//uint8_t* _frame = nullptr;
		//this->videoFrame->GetBytes((void**)&_frame);
		//std::fill(&_frame[0], &_frame[0] + this->bufferSize, 255);
	}

	this->videoFrame->SetTimecodeFromComponents(
		this->timeCodeFormat,
		this->timeCode->hours(),
		this->timeCode->minutes(),
		this->timeCode->seconds(),
		this->timeCode->frames(),
		bmdTimecodeFlagDefault);

	if (output->ScheduleVideoFrame(this->videoFrame, (totalFramesScheduled * this->frameDuration), this->frameDuration, frameTimescale) == S_OK)
	{
		this->totalFramesScheduled += 1;
	}
	else
	{
		V(S_FALSE, "Sending video buffer to decklink", this->name);
	}
	timeCode->update();
}

#pragma region Interfaces Implementation

HRESULT decklink::QueryInterface(REFIID iid, LPVOID *ppv)
{
	return E_NOINTERFACE;
}

ULONG decklink::AddRef()
{
	return 1;
}

HRESULT decklink::DrawFrame(IDeckLinkVideoFrame* pFrame)
{
	return S_OK;
}

HRESULT	decklink::ScheduledFrameCompleted(IDeckLinkVideoFrame* completedFrame, BMDOutputFrameCompletionResult pCompletionResult)
{
	auto _hr = S_OK;

	if (pCompletionResult != bmdOutputFrameCompleted)
	{
		switch (pCompletionResult)
		{
		default:
			logger.write(L"frame did not complete: Unknown error");
		case bmdOutputFrameDisplayedLate:
			_hr = S_FALSE;
			logger.write(L"frame display late");
			break;
		case bmdOutputFrameDropped:
			_hr = S_FALSE;
			logger.write(L"frame dropped");
			break;
		case bmdOutputFrameCompleted:
			break;
		case bmdOutputFrameFlushed:
			logger.write(L"frame flushed");
			break;
		}
	}


	if (pCompletionResult == bmdOutputFrameDisplayedLate || pCompletionResult == bmdOutputFrameDropped)
	{
		this->totalFramesScheduled += 2;
	}

	_schedule_next_frame(false);

	return S_OK;
}

HRESULT	decklink::RenderAudioSamples(BOOL pPreroll)
{
	return S_OK;
}

HRESULT	decklink::ScheduledPlaybackHasStopped(void)
{
	return S_OK;
}

#pragma endregion

#pragma region Release

ULONG decklink::Release()
{
	return 1;
}

void decklink::clear_video_queue()
{
	while (this->videoFramesQueue.unsafe_size() > 0)
	{
		uint8_t* _frame = nullptr;
		auto _hr = this->videoFramesQueue.try_pop(_frame);
		if (_hr && _frame)
		{
			scalable_free(_frame);
		}
	}

	this->videoFramesQueue.clear();
}

void decklink::_release_decklink_iterator(IDeckLinkIterator* iDLIterator)
{
	if (iDLIterator != nullptr)
	{
		iDLIterator->Release();
		iDLIterator = nullptr;
	}
}

void decklink::free()
{
	if (this->isReleased) return;

	clear_video_queue();

	if (this->output != nullptr)
	{
		this->output->DisableAudioOutput();
		this->output->DisableVideoOutput();

		this->output->SetScreenPreviewCallback(NULL);
		this->output->SetScheduledFrameCompletionCallback(NULL);
		this->output->Release();
		this->output = nullptr;
	}
	if (this->_ideckLink != nullptr)
	{
		this->_ideckLink->Release();
		this->_ideckLink = nullptr;
	}

	this->isReleased = true;
}

#pragma endregion

#pragma region Getters

const bool decklink::get_is_valid_license() const
{
	return sInValidLicense;
}

#pragma endregion

#pragma region Setters

void decklink::set_video_frame(uint8_t* pData)
{
	if (!this->isRunning) return;

	this->videoFramesQueue.push(pData);
}

void decklink::set_audio_sample_frame(_In_ uint8_t* pSamples, _In_ const UINT pSamplesCount)
{
	//BMDTimeValue _audioStreamTime = pPTS + off;// this->totalAudioSecondsScheduled * this->audioSampleRate * this->frameDuration / this->frameTimescale;

	unsigned int _written = 0;
	if (this->output->ScheduleAudioSamples(
		pSamples,
		pSamplesCount,
		0,
		0,
		&_written) != S_OK)
	{
		V(S_FALSE, "sending audio buffer to decklink", "Decklink");
	}
	if (_written != pSamplesCount)
	{
		this->output->FlushBufferedAudioSamples();
	}
}

#pragma endregion