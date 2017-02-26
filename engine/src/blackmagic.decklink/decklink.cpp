#include "decklink.h"
#include <w_logger.h>
#include <stdint.h>

#if defined (__1080p_50__)

#define PREFERED_DISPLAY_MODE "1080p50"

#elif defined (__1080_25p__)

#define PREFERED_DISPLAY_MODE "1080p25"

#elif defined (__PAL__)

#define PREFERED_DISPLAY_MODE "PAL"

#elif defined (__NTSC__)

#define PREFERED_DISPLAY_MODE "NTSC"

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
	_is_running(false),
	_support_input_format_detection(false),
	_frame_flag(_BMDFrameFlags::bmdFrameFlagDefault),
	_audio_sample_rate(bmdAudioSampleRate48kHz),
	_time_code(nullptr),
	_deckLink_interface(nullptr),
	_deckLink_input(nullptr),
	_decklink_iterator(nullptr),
	_decklink_output(nullptr)
{
	_super::set_class_name(typeid(this).name());
}

decklink::~decklink()
{
	free();
}

HRESULT decklink::initialize()
{
	this->_device_info.clear();

	auto _hr = CoInitialize(NULL);
	V(_hr, L"CoInitialize", this->name);

	_hr = CoCreateInstance(CLSID_CDeckLinkIterator, NULL, CLSCTX_ALL, IID_IDeckLinkIterator, (void**)&this->_decklink_iterator);
	//On problem happens first release the iDLIterator then release this class and finally write log and close the app
	if (_hr != S_OK)
	{
		wstring _msg = L"BlackMagic DeckLink drivers not found";
		free();
		MessageBox(NULL, _msg.c_str(), L"WPlayout", MB_OK);
		V(_hr, _msg, this->name);
		return S_FALSE;
	}

	_hr = this->_decklink_iterator->Next(&this->_deckLink_interface);
	if (_hr == S_FALSE)
	{
		free();
		return S_FALSE;
	}
	V(_hr, L"BlackMagic DeckLink device is not installed", this->name, true);

	this->_deckLink_interface->AddRef();
	//get output interface of decklink
	_hr = this->_deckLink_interface->QueryInterface(IID_IDeckLinkOutput, (void**)&this->_decklink_output);
	if (_hr == S_FALSE)
	{
		free();
		return S_FALSE;
	}
	V(_hr, L"getting output for DeckLink", this->name, true);

	//get decklink key for output
	_hr = this->_decklink_output->QueryInterface(IID_IDeckLinkKeyer, (void**)&this->_decklink_keyer);
	if (_hr == S_FALSE)
	{
		free();
		return S_FALSE;
	}
	else if(_hr == E_NOINTERFACE)
	{
		V(_hr, L"no such interface for decklink keyer", this->name);
	}
	
	//get input capture interface of decklink
	_hr = this->_deckLink_interface->QueryInterface(IID_IDeckLinkInput, (void**)&this->_deckLink_input);
	if (_hr == S_FALSE)
	{
		free();
		return S_FALSE;
	}
	V(_hr, L"getting input capture for DeckLink", this->name, true);

	_get_decklink_information();
	_get_output_supported_display_modes();
	//_get_input_supported_display_modes();

	//initialize callback
	//hr = this->output->SetScreenPreviewCallback(this);
	_hr = this->_decklink_output->SetScheduledFrameCompletionCallback(this);
	//hr = this->output->SetAudioCallback(this);

	return _hr;
}

void decklink::_get_decklink_information()
{
	IDeckLinkAPIInformation* _decklink_api_information = nullptr;
	IDeckLinkAttributes* _decklink_attributes = nullptr;

	//Get the name of deckLink
	BSTR _deckLinkModulName = nullptr;
	auto _hr = this->_deckLink_interface->GetModelName(&_deckLinkModulName);
	V(_hr, L"getting decklink device name", this->name);

	_hr = this->_decklink_iterator->QueryInterface(IID_IDeckLinkAPIInformation, (void**)&_decklink_api_information);
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

		this->_device_info = "Blackmagic decklink device : " + string(decklinkName.begin(), decklinkName.end()) + "\r\n";
		this->_device_info += "DeckLinkAPI version : " + std::to_string(_dlVerMajor) + "," + std::to_string(_dlVerMinor) + "," + std::to_string(_dlVerPoint);

		BSTR								_name = NULL;
		BOOL								_supported;
		LONGLONG							_value;

		// Query the DeckLink for its attributes interface
		_hr = this->_deckLink_interface->QueryInterface(IID_IDeckLinkAttributes, (void**)&_decklink_attributes);
		if (_hr != S_OK)
		{
			V(_hr, L"Could not obtain the IDeckLinkAttributes interface", this->name);
			return;
		}

		// List attributes and their value
		_value = -1;
		_hr = _decklink_attributes->GetInt(BMDDeckLinkPersistentID, &_value);
		this->_device_info += "\r\nDevice Persistent ID : ";
		if (_hr == S_OK)
		{
			this->_device_info += to_string(_value);
		}
		else
		{
			this->_device_info += "Not Supported on this device";
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
		this->_device_info += "\r\nDevice Topological ID : ";
		if (_hr == S_OK)
		{
			this->_device_info += to_string(_value);
		}
		else
		{
			this->_device_info += "Not Supported on this device";
		}

		_hr = _decklink_attributes->GetInt(BMDDeckLinkNumberOfSubDevices, &_value);
		if (_hr == S_OK)
		{
			this->_device_info += "\r\nNumber of sub-devices : " + to_string(_value);
			if (_value != 0)
			{
				_hr = _decklink_attributes->GetInt(BMDDeckLinkSubDeviceIndex, &_value);
				this->_device_info += "\r\nSub-devices index : ";
				if (_hr == S_OK)
				{
					this->_device_info += to_string(_value);
				}
				else
				{
					this->_device_info += "Could not query the sub-device index attribute";
				}
			}
		}
		else
		{
			this->_device_info += "\r\nCould not query the number of sub-device attribute";
		}

		_hr = _decklink_attributes->GetInt(BMDDeckLinkMaximumAudioChannels, &_value);
		this->_device_info += "\r\nMaximum number of audio channels: ";
		if (_hr == S_OK)
		{
			this->_device_info += to_string(_value);
		}
		else
		{
			this->_device_info += "Could not query the number of audio channels";
		}

		BOOL _key_value = FALSE;
		_hr = _decklink_attributes->GetFlag(BMDDeckLinkSupportsInternalKeying, &_key_value);
		if (_hr == S_OK)
		{
			this->_device_info += _key_value == TRUE ? "Support internal keying" : "Does not support internal keying";
		}
		else
		{
			this->_device_info += "Could not get flag of internal keying";
		}

		_hr = _decklink_attributes->GetFlag(BMDDeckLinkSupportsExternalKeying, &_key_value);
		if (_hr == S_OK)
		{
			this->_device_info += _key_value == TRUE ? "Support external keying" : "Does not support external keying";
		}
		else
		{
			this->_device_info += "Could not get flag of external keying";
		}

		_hr = _decklink_attributes->GetFlag(BMDDeckLinkSupportsHDKeying, &_key_value);
		if (_hr == S_OK)
		{
			this->_device_info += _key_value == TRUE ? "Support HD keying" : "Does not support HD keying";
		}
		else
		{
			this->_device_info += "Could not get flag of HD keying";
		}

		//does support input format detection
		_hr = _decklink_attributes->GetFlag(BMDDeckLinkSupportsInputFormatDetection, &_key_value);
		if (_hr == S_OK)
		{
			if (_key_value == TRUE)
			{
				this->_support_input_format_detection = true;
				this->_device_info += "Support input format detection";
			}
			else
			{
				this->_support_input_format_detection = false;
				this->_device_info += "Does not support input format detection";
			}
		}
		else
		{
			this->_device_info += "Could not get flag of input format detection";
		}

		_decklink_api_information->Release();

		this->_device_info += "\r\n";

		logger.user(this->_device_info);
		//clear the decklinkName which defined as wstring
		decklinkName.clear();
	}
	else
	{
		logger.user("Could not get the information of decklink API");
	}

	_decklink_api_information = nullptr;

}

void decklink::_get_output_supported_display_modes()
{
	IDeckLinkDisplayModeIterator* _decklink_display_mode_iter = nullptr;

	//Todo : this must be config able from app
	_BMDPixelFormat _pixelFormat = bmdFormat8BitYUV;

	//clear all display modes
	this->_output_display_modes.clear();
	this->_supported_output_display_modes.clear();

	string _display_modes_msg = "Supported output display modes : \r\n";
	string _unsupported_display_modes_msg = "UnSupported output display modes : \r\n";
	
	HRESULT _hr = S_FALSE;
	if (this->_decklink_output->GetDisplayModeIterator(&_decklink_display_mode_iter) == S_OK)
	{
		IDeckLinkDisplayMode* _dLDisplayMode = nullptr;
		while (_decklink_display_mode_iter->Next(&_dLDisplayMode) == S_OK)
		{
			BSTR					_bstr_mode_name;
			BMDDisplayModeSupport	_displayModeSupport;
			BMDVideoOutputFlags		_videoOutputFlags = bmdVideoOutputDualStream3D;

			//If could not get the name then continiue
			if (_dLDisplayMode->GetName(&_bstr_mode_name) != S_OK)
			{
				SysFreeString(_bstr_mode_name);
				continue;
			}

			_hr = this->_decklink_output->DoesSupportVideoMode(
				_dLDisplayMode->GetDisplayMode(),
				_pixelFormat,
				_videoOutputFlags,
				&_displayModeSupport,
				NULL);

			//Save the name
			auto _w_mode_name = wstring(_bstr_mode_name);
			auto _mode_name = std::string(_w_mode_name.begin(), _w_mode_name.end());

			if (_displayModeSupport)
			{
				_unsupported_display_modes_msg.append(_mode_name).append(" \r\n");
			}
			else
			{
				this->_supported_output_display_modes.push_back(_mode_name);
				_display_modes_msg.append(_mode_name).append(" \r\n");
			}

			//Save the name and the displayMode in the map
			this->_output_display_modes[_mode_name] = _dLDisplayMode;
			_dLDisplayMode->AddRef();

			//Free the strings
			_mode_name.clear();
			_w_mode_name.clear();
			SysFreeString(_bstr_mode_name);
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
		V(S_FALSE, wstring(_caption.begin(), _caption.end()), this->name, true, false);
	}

	//Log the display modes
	logger.user(_display_modes_msg);
	logger.user(_unsupported_display_modes_msg);

	//Todo : This must change from setting
	if (this->_output_display_modes.find(PREFERED_DISPLAY_MODE) == this->_output_display_modes.end())
	{
		logger.user("Display mode " + std::string(PREFERED_DISPLAY_MODE) + " is not supported");
	}
}

void decklink::_get_input_supported_display_modes()
{
	IDeckLinkDisplayModeIterator* _decklink_display_mode_iter = nullptr;

	_BMDPixelFormat _pixelFormat = bmdFormat8BitYUV;

	//clear all display modes
	this->_input_display_modes.clear();
	this->_supported_input_display_modes.clear();

	string _display_modes_msg = "Supported input display modes : \r\n";
	string _unsupported_display_modes_msg = "UnSupported input display modes : \r\n";

	HRESULT _hr = S_FALSE;
	if (this->_deckLink_input->GetDisplayModeIterator(&_decklink_display_mode_iter) == S_OK)
	{
		IDeckLinkDisplayMode* _dLDisplayMode = nullptr;
		while (_decklink_display_mode_iter->Next(&_dLDisplayMode) == S_OK)
		{
			BSTR					_bstr_mode_name;
			BMDDisplayModeSupport	_displayModeSupport;
			BMDVideoOutputFlags		_videoOutputFlags = bmdVideoOutputDualStream3D;

			//If could not get the name then continiue
			if (_dLDisplayMode->GetName(&_bstr_mode_name) != S_OK)
			{
				SysFreeString(_bstr_mode_name);
				continue;
			}

			_hr = this->_decklink_output->DoesSupportVideoMode(
				_dLDisplayMode->GetDisplayMode(),
				_pixelFormat,
				_videoOutputFlags,
				&_displayModeSupport,
				NULL);

			//Save the name
			auto _w_mode_name = wstring(_bstr_mode_name);
			auto _mode_name = string(_w_mode_name.begin(), _w_mode_name.end());

			if (_displayModeSupport)
			{
				_unsupported_display_modes_msg.append(_mode_name).append(" \r\n");
			}
			else
			{
				this->_supported_input_display_modes.push_back(_mode_name);
				_display_modes_msg.append(_mode_name).append(" \r\n");
			}

			//Save the name and the displayMode in the map
			this->_input_display_modes[_mode_name] = _dLDisplayMode;
			_dLDisplayMode->AddRef();

			//Free the strings
			_mode_name.clear();
			_w_mode_name.clear();
			SysFreeString(_bstr_mode_name);
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
		V(S_FALSE, wstring(_caption.begin(), _caption.end()), this->name, true, false);
	}

	//Log the display modes
	logger.user(_display_modes_msg);
	logger.user(_unsupported_display_modes_msg);

	if (this->_input_display_modes.find(PREFERED_DISPLAY_MODE) == this->_input_display_modes.end())
	{
		logger.user("Display mode " + std::string(PREFERED_DISPLAY_MODE) + " is not supported");
	}
}

HRESULT decklink::start_output()
{
	this->_is_running = true;

	logger.user(L"Decklink is going to start outputing");

	auto selectedDisplayMode = this->_output_display_modes[PREFERED_DISPLAY_MODE];
	this->_display_mode_width = selectedDisplayMode->GetWidth();
	this->_display_mode_height = selectedDisplayMode->GetHeight();
	this->_buffer_size = this->_display_mode_width * this->_display_mode_height * 4;//4 = BGRA

	selectedDisplayMode->GetFrameRate(&this->_frame_duration, &this->_frame_time_scale);
	this->_fps = (this->_frame_time_scale + (this->_frame_duration - 1)) / this->_frame_duration;

	auto msg = std::wstring(L"Display mode FPS is " + std::to_wstring(this->_fps) + L", frame width is " + std::to_wstring(this->_display_mode_width) +
		L", frame height is " + std::to_wstring(this->_display_mode_height));
	logger.user(msg);
	msg.clear();

	if (this->_time_code)
	{
		delete this->_time_code;
	}
	this->_time_code = new time_code(this->_fps);

	this->_total_audio_seconds_scheduled = 0;
	this->_total_frames_scheduled = 0;

#pragma region Enable Video

	//IDeckLinkAttributes::GetFlag using BMDDeckLinkSupportsInternalKeying or BMDDeckLinkSupportsExternalKeying to determine internal / external keying support

	BMDVideoOutputFlags _videoOutputFlags;
	auto _videoDisplayMode = selectedDisplayMode->GetDisplayMode();
	if (_videoDisplayMode == bmdModeNTSC ||
		_videoDisplayMode == bmdModeNTSC2398 ||
		_videoDisplayMode == bmdModePAL)
	{
		this->_time_code_format = BMDTimecodeFormat::bmdTimecodeVITC;
		_videoOutputFlags = BMDVideoOutputFlags::bmdVideoOutputVITC;
	}
	else
	{
		this->_time_code_format = BMDTimecodeFormat::bmdTimecodeRP188Any;
		_videoOutputFlags = BMDVideoOutputFlags::bmdVideoOutputRP188;
	}

	if (this->_decklink_output->EnableVideoOutput(_videoDisplayMode, _videoOutputFlags) != S_OK)
	{
		free();
		return S_FALSE;
	}

	if (this->_decklink_output->CreateVideoFrame(
		this->_display_mode_width, 
		this->_display_mode_height, 
		this->_display_mode_width * 4, 
		bmdFormat8BitBGRA, 
		this->_frame_flag, 
		&this->_video_frame) != S_OK)
	{
		free();
		return S_FALSE;
	}

	//enable alpha transparency
	if (_decklink_keyer)
	{
		this->_decklink_keyer->Enable(TRUE);
		this->_decklink_keyer->SetLevel(255);
		this->_decklink_output->DisplayVideoFrameSync(this->_video_frame);
	}

	for (size_t i = 0; i < MAX_PREROLL; ++i)
	{
		auto _frame = (uint8_t*)scalable_malloc(this->_buffer_size);
		std::fill(&_frame[0], &_frame[0] + this->_buffer_size, 0);
		this->_video_frames_queue.push(_frame);
		_schedule_next_frame(true);
	}

#pragma endregion

#pragma region Enable Audio

	auto _audioChannelCount = 2;
	auto _audioSampleDepth = 16; // bmdAudioSampleType16bitInteger
	// Set the audio output mode, bmdAudioOutputStreamContinuous need for continusly fill buffer from my code, time step did not schedual
	if (this->_decklink_output->EnableAudioOutput(this->_audio_sample_rate, bmdAudioSampleType16bitInteger, _audioChannelCount, bmdAudioOutputStreamContinuous) != S_OK)//bmdAudioOutputStreamTimestamped
	{
		free();
		return S_FALSE;
	}

	this->_audio_samples_per_frame = (this->_audio_sample_rate * this->_frame_duration) / this->_frame_time_scale;
	this->_audio_buffer_sample_length = (this->_fps * this->_audio_sample_rate * this->_frame_duration) / this->_frame_time_scale;

#pragma endregion

	if (this->_decklink_output->BeginAudioPreroll() != S_OK)
	{
		V(S_FALSE, "beginning pre-rolling audio frames in deckLink", this->name, true);
	}

	msg.clear();

	this->_decklink_output->StartScheduledPlayback(0, this->_frame_time_scale, 1.0);

	return S_OK;
}

HRESULT decklink::stop_output()
{
	if (!this->_is_running) return S_FALSE;

	this->_decklink_output->StopScheduledPlayback(0, NULL, 0);

	this->_decklink_output->DisableVideoOutput();
	this->_decklink_output->DisableAudioOutput();

	if (this->_decklink_keyer)
	{
		this->_decklink_keyer->Disable();
	}

	this->_is_running = false;

	return S_OK;
}

HRESULT decklink::start_capturing()
{
	BMDVideoInputFlags	_video_input_flags = bmdVideoInputFlagDefault;
	if (this->_support_input_format_detection)
	{
		_video_input_flags |= bmdVideoInputEnableFormatDetection;
	}

	return S_OK;
}

HRESULT decklink::stop_capturing()
{
	return S_OK;
}

void decklink::_schedule_next_frame(bool pPreroll)
{
	if (!this->_is_running) return;
	
	uint8_t* _frameOfQueue = nullptr;
	auto _hr = false;

	auto _size = this->_video_frames_queue.unsafe_size();
	if (_size > 0)
	{
		_hr = this->_video_frames_queue.try_pop(_frameOfQueue);
	}

	if (_hr && _frameOfQueue)
	{
		uint8_t* _frame = nullptr;
		this->_video_frame->GetBytes((void**)&_frame);

		if (_frame != nullptr)
		{
			//copy from queue to the video frame
			std::copy(&_frameOfQueue[0], &_frameOfQueue[0] + this->_buffer_size, &_frame[0]);
		}

		scalable_free(_frameOfQueue);
	}
	//else
	//{
		//uint8_t* _frame = nullptr;
		//this->_video_frame->GetBytes((void**)&_frame);
		//std::fill(&_frame[0], &_frame[0] + this->_buffer_size, 255);
	//}

	this->_video_frame->SetTimecodeFromComponents(
		this->_time_code_format,
		this->_time_code->hours(),
		this->_time_code->minutes(),
		this->_time_code->seconds(),
		this->_time_code->frames(),
		bmdTimecodeFlagDefault);

	if (_decklink_output->ScheduleVideoFrame(
		this->_video_frame, 
		this->_total_frames_scheduled * this->_frame_duration,
		this->_frame_duration,
		this->_frame_time_scale) == S_OK)
	{
		this->_total_frames_scheduled += 1;
	}
	else
	{
		V(S_FALSE, "Sending video buffer to decklink", this->name);
	}
	this->_time_code->update();
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
		this->_total_frames_scheduled += 2;
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


void decklink::clear_video_queue()
{
	while (this->_video_frames_queue.unsafe_size() > 0)
	{
		uint8_t* _frame = nullptr;
		auto _hr = this->_video_frames_queue.try_pop(_frame);
		if (_hr && _frame)
		{
			scalable_free(_frame);
		}
	}

	this->_video_frames_queue.clear();
}

void decklink::free()
{
	if (_super::get_is_released()) return;

	clear_video_queue();

	this->_output_display_modes.clear();
	this->_supported_output_display_modes.clear();

	this->_is_running = false;
	if (this->_time_code)
	{
		delete this->_time_code;
	}
	if (this->_video_frame)
	{
		this->_video_frame->Release();
		this->_video_frame = nullptr;
	}
	if (this->_decklink_iterator != nullptr)
	{
		this->_decklink_iterator->Release();
		this->_decklink_iterator = nullptr;
	}
	if (this->_decklink_output != nullptr)
	{
		this->_decklink_output->DisableAudioOutput();
		this->_decklink_output->DisableVideoOutput();

		this->_decklink_output->SetScreenPreviewCallback(NULL);
		this->_decklink_output->SetScheduledFrameCompletionCallback(NULL);
		this->_decklink_output->Release();
		this->_decklink_output = nullptr;
	}
	if (this->_deckLink_interface != nullptr)
	{
		this->_deckLink_interface->Release();
		this->_deckLink_interface = nullptr;
	}
	if (this->_decklink_keyer)
	{
		this->_decklink_keyer->Release();
	}

	_super::release();
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
	if (!this->_is_running) return;

	this->_video_frames_queue.push(pData);
}

void decklink::set_audio_sample_frame(_In_ uint8_t* pSamples, _In_ const UINT pSamplesCount)
{
	//BMDTimeValue _audioStreamTime = pPTS + off;// this->totalAudioSecondsScheduled * this->audioSampleRate * this->frameDuration / this->frameTimescale;

	unsigned int _written = 0;
	if (this->_decklink_output->ScheduleAudioSamples(
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
		this->_decklink_output->FlushBufferedAudioSamples();
	}
}

#pragma endregion