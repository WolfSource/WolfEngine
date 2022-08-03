#include "w_vcm_capturer.hpp"
#include <wolf.hpp>

#include "DISABLE_ANALYSIS_BEGIN"
#include <modules/video_capture/video_capture_factory.h>
#include "DISABLE_ANALYSIS_END"

using w_vcm_capturer = wolf::stream::webRTC::w_vcm_capturer;

w_vcm_capturer::~w_vcm_capturer()
{
	fini();
}

void w_vcm_capturer::OnFrame(const webrtc::VideoFrame &p_frame)
{
	this->broadcaster.OnFrame(p_frame);
}

bool w_vcm_capturer::init(size_t p_width,
						  size_t p_height,
						  size_t p_target_fps,
						  const std::string &p_video_url)
{
	bool _ret = true;
	auto _fin = gsl::finally([&]()
							 {
			if (!_ret)
			{
				fini();
			} });

	this->_width = p_width;
	this->_height = p_height;

	std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> _device_info(webrtc::VideoCaptureFactory::CreateDeviceInfo());
	if (_device_info == nullptr)
	{
		return _ret = false;
	}

	std::string _device_id, _name, _id;

	_name.reserve(W_MAX_PATH);
	_id.reserve(W_MAX_PATH);

	const auto _num_video_devices = _device_info->NumberOfDevices();

	if (p_video_url.find("videocap://") == 0)
	{
		auto _str = p_video_url.substr(strlen("videocap://"));
		const int _device_number = atoi(_str.c_str());

		if (_device_info->GetDeviceName(
				_device_number,
				&_name[0],
				W_MAX_PATH,
				&_id[0],
				W_MAX_PATH) == 0)
		{
			_device_id = std::move(_id);
		}
	}
	else
	{
		for (uint32_t i = 0; i < _num_video_devices; ++i)
		{
			if (_device_info->GetDeviceName(i,
											_name.data(),
											W_MAX_PATH,
											_id.data(),
											W_MAX_PATH) == 0)
			{
				if (p_video_url == _name)
				{
					_device_id = std::move(_id);
					break;
				}
			}
		}
	}

	if (_device_id.empty())
	{
		return _ret = false;
	}

	this->_vcm = webrtc::VideoCaptureFactory::Create(_device_id.c_str());
	if (this->_vcm == nullptr)
	{
		return _ret = false;
	}

	this->_vcm->RegisterCaptureDataCallback(this);

	webrtc::VideoCaptureCapability _capability = {};
	_capability.width = gsl::narrow_cast<int32_t>(this->_width);
	_capability.height = gsl::narrow_cast<int32_t>(this->_height);
	_capability.maxFPS = gsl::narrow_cast<int32_t>(p_target_fps);
	_capability.videoType = webrtc::VideoType::kI420;

	if (_device_info->GetBestMatchedCapability(
			this->_vcm->CurrentDeviceName(),
			_capability,
			_capability) < 0)
	{
		_device_info->GetCapability(
			this->_vcm->CurrentDeviceName(),
			0,
			_capability);
	}

	if (this->_vcm->StartCapture(_capability) != 0)
	{
		return _ret = false;
	}

	return _ret;
}

void w_vcm_capturer::fini()
{
	if (this->_vcm != nullptr)
	{
		this->_vcm->StopCapture();
		this->_vcm->DeRegisterCaptureDataCallback();
		this->_vcm->Release();
		this->_vcm = nullptr;
	}
}

size_t w_vcm_capturer::get_width() const noexcept
{
	return this->_width;
}
size_t w_vcm_capturer::get_height() const noexcept
{
	return this->_height;
}

w_vcm_capturer *w_vcm_capturer::create(
	const std::string &p_video_url,
	const std::map<std::string,
				   std::string> &p_opts)
{
	gsl::owner<w_vcm_capturer *> _vcm_capturer = new (std::nothrow) w_vcm_capturer();
	if (_vcm_capturer == nullptr)
	{
		return nullptr;
	}

	bool _ret = true;
	auto _fin = gsl::finally([&]()
							 {
			if (!_ret)
			{
				_vcm_capturer->fini();
				delete _vcm_capturer;
			} });

	size_t width = 0;
	size_t height = 0;
	size_t fps = 0;

	if (p_opts.find("width") != p_opts.end())
	{
		width = std::stoi(p_opts.at("width"));
	}
	if (p_opts.find("height") != p_opts.end())
	{
		height = std::stoi(p_opts.at("height"));
	}
	if (p_opts.find("fps") != p_opts.end())
	{
		fps = std::stoi(p_opts.at("fps"));
	}

	if (_vcm_capturer->init(
			width,
			height,
			fps,
			p_video_url) == false)
	{
		_ret = false;
		return nullptr;
	}

	return _vcm_capturer;
}
