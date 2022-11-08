#include "openal.h"
#include <wolf.hpp>
#include <thread>

#include <DISABLE_ANALYSIS_BEGIN>
#include <AL/al.h>
#include <AL/alext.h>
#include <DISABLE_ANALYSIS_END>

//Sound info
struct w_openal_t
{
	ALCdevice* device = nullptr;
	ALCcontext* ctx = nullptr;

	// a lockless ring-buffer (supports single-provider, single-consumer operation)
	ALbyte* data = nullptr;
	size_t data_size = 0;
	size_t read_pos = 0;
	size_t write_pos = 0;

	// The buffer to get the callback, and source to play with
	ALuint buffer = 0;
	ALuint source = 0;
	size_t start_offset = 0;

	// Handle for the audio file to decode
	size_t decoder_offset = 0;

	// The format of the callback samples
	ALenum format = 0;

	size_t sample_rate = 0;
	size_t number_of_channels = 2;
	size_t total_read_bytes = 0;
	size_t size_of_chunk = 0;
	LPALBUFFERCALLBACKSOFT callback_ptr = nullptr;
};

const char* s_openal_get_last_error()
{
	const auto _error = alGetError();
	switch (_error)
	{
	default:
		return "unknown error code";
	case AL_NO_ERROR: return nullptr;
	case AL_INVALID_NAME: return "AL_INVALID_NAME";
	case AL_INVALID_ENUM: return "AL_INVALID_ENUM";
	case AL_INVALID_VALUE: return "AL_INVALID_VALUE";
	case AL_INVALID_OPERATION: return "AL_INVALID_OPERATION";
	case AL_OUT_OF_MEMORY: return "AL_OUT_OF_MEMORY";
	}
}

static ALsizei AL_APIENTRY s_openal_callback(
	_In_ void* p_user_ptr, 
	_In_ void* p_data, 
	_In_ ALsizei p_size)
{
	auto _openal = gsl::narrow_cast<w_openal>(p_user_ptr);
	const auto _openal_nn = gsl::not_null<w_openal>(_openal);
	size_t _bytes = 0;

	auto _read_offset = _openal_nn->read_pos;
	while (_bytes < p_size)
	{
		/* If the write offset == read offset, there's nothing left in the
		 * ring-buffer. Break from the loop and give what has been written.
		 */
		const auto _write_offset = _openal_nn->write_pos;
		if (_write_offset == _read_offset)
		{
			break;
		}

		/* If the write offset is behind the read offset, the readable
		 * portion wrapped around. Just read up to the end of the buffer in
		 * that case, otherwise read up to the write offset. Also limit the
		 * amount to copy given how much is remaining to write.
		 */
		auto _todo = ((_write_offset < _read_offset) ? _openal_nn->data_size : _write_offset) - _read_offset;
		_todo = _todo > (p_size - _bytes) ? (p_size - _bytes) : _todo;

		/* Copy from the ring buffer to the provided output buffer. Wrap
		 * the resulting read offset if it reached the end of the ring-
		 * buffer.
		 */
		memcpy(p_data, &_openal_nn->data[_read_offset], _todo);
		p_data = gsl::narrow_cast<ALbyte*>(p_data) + _todo;
		_bytes += gsl::narrow_cast<ALsizei>(_todo);

		_read_offset += _todo;
		if (_read_offset == _openal_nn->data_size)
		{
			_read_offset = 0;
		}
	}
	/* Finally, store the updated read offset, and return how many bytes
	 * have been written.
	 */
	_openal_nn->read_pos = _read_offset;

	return gsl::narrow_cast<ALsizei>(_bytes);
}

void w_openal_list_all_devices(
	_Inout_ char* p_output_devices,
	_Inout_ char* p_input_devices)
{
	if (alcIsExtensionPresent(nullptr, "ALC_enumeration_EXT") == AL_TRUE)
	{
		if (alcIsExtensionPresent(nullptr, "ALC_enumerate_all_EXT") == AL_TRUE)
		{
			s_strcpy(p_output_devices, alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER));
		}
		else
		{
			s_strcpy(p_output_devices, alcGetString(nullptr, ALC_DEVICE_SPECIFIER));
		}
		s_strcpy(p_input_devices, alcGetString(nullptr, ALC_CAPTURE_DEVICE_SPECIFIER));
	}
}

int w_openal_init(
	_Inout_ w_openal* p_openal,
	_In_ int p_format,
	_In_ int p_sample_rate,
	_In_ int p_refresh,
	_In_ int p_number_of_channels,
	_Inout_ char* p_error)
{
	constexpr auto TRACE = "w_openal_init";
	const auto _error_nn = gsl::not_null<char*>(p_error);

	int _ret = 0;
	*p_openal = nullptr;

	auto _ptr = (w_openal_t*)calloc(1, sizeof(w_openal_t));
	const auto _openal = gsl::not_null<w_openal_t*>(_ptr);

	defer _(nullptr, [&](...)
		{
			if (_ret != 0)
			{
				w_openal_fini(&_ptr);
			}
		});

	// open and initialize a audio device
	_openal->device = alcOpenDevice(nullptr);
	if (!_openal->device)
	{
		std::ignore = snprintf(
			p_error,
			W_MAX_PATH,
			"could not open a device. trace info: %s",
			TRACE);
		return _ret = -1;
	}

	_openal->ctx = alcCreateContext(_openal->device, nullptr);
	if (_openal->ctx == nullptr || alcMakeContextCurrent(_openal->ctx) == ALC_FALSE)
	{
		std::ignore = snprintf(
			p_error,
			W_MAX_PATH,
			"could not get openal context. trace info: %s",
			TRACE);
		return _ret = -1;
	}

	// get the sound format, and figure out the OpenAL format
	_openal->format = p_format;
	_openal->sample_rate = p_sample_rate;
	_openal->number_of_channels = p_number_of_channels;
	_openal->data  = nullptr;

	switch (_openal->format)
	{
	default:
	case AL_FORMAT_MONO16:
	case AL_FORMAT_STEREO16:
	{
		// Signed 16-bit buffer format
		_openal->size_of_chunk = sizeof(int16_t);
		break;
	}
	case AL_FORMAT_MONO8:
	case AL_FORMAT_STEREO8:
	{
		// Unsigned 8-bit buffer format
		_openal->size_of_chunk = sizeof(uint8_t);
		break;
	}
	}

	// generate the buffer
	alGenBuffers(1, &_openal->buffer);
	auto _error = s_openal_get_last_error();
	if (_error)
	{
		std::ignore = snprintf(
			_error_nn,
			W_MAX_PATH,
			"could not generate buffer for openAL because:%s. trace info: %s",
			_error,
			TRACE);
		return _ret = -1;
	}

	alGenSources(1, &_openal->source);
	_error = s_openal_get_last_error();
	if (_error)
	{
		std::ignore = snprintf(
			_error_nn,
			W_MAX_PATH,
			"could not generate sources for openal because:%s. trace info: %s",
			_error,
			TRACE);
		return _ret = -1;
	}

	if (alIsExtensionPresent("AL_SOFTX_callback_buffer") != 0)
	{
		std::ignore = snprintf(
			_error_nn,
			W_MAX_PATH,
			"could not get AL_SOFT_callback_buffer. trace info: %s",
			TRACE);
		return _ret = -1;
	}

	_openal->callback_ptr = gsl::narrow_cast<LPALBUFFERCALLBACKSOFT>(alGetProcAddress("alBufferCallbackSOFT"));

	alcGetIntegerv(_openal->device, ALC_REFRESH, 1, &p_refresh);

	*p_openal = _openal;

	return _ret;
}

int w_openal_open(
	_In_ w_openal p_openal, 
	_Inout_ char* p_error)
{
	constexpr auto TRACE = "w_openal_open";
	const auto _error_nn = gsl::not_null<char*>(p_error);
	const auto _openal_nn = gsl::not_null<w_openal>(p_openal);

	w_openal_close(p_openal, true);
	
	// Set a 1s ring buffer size
	_openal_nn->data_size = gsl::narrow_cast<ALuint>(_openal_nn->sample_rate * _openal_nn->number_of_channels) * 
		_openal_nn->size_of_chunk;// sizeof(float);
	_openal_nn->data = gsl::narrow_cast<ALbyte*>(malloc(_openal_nn->data_size));
	_openal_nn->read_pos = 0;
	_openal_nn->write_pos = 0;
	_openal_nn->decoder_offset = 0;
	_openal_nn->callback_ptr(
		_openal_nn->buffer, 
		_openal_nn->format, 
		gsl::narrow_cast<ALsizei>(_openal_nn->sample_rate),
		s_openal_callback,
		_openal_nn.get() /* send openal object on user's data*/);

	auto _error = s_openal_get_last_error();
	if (_error)
	{
		std::ignore = snprintf(
			_error_nn,
			W_MAX_PATH,
			"could not set openal callbacks because: %s. trace info: %s",
			_error,
			TRACE);
		return W_FAILURE;
	}
	alSourcei(_openal_nn->source, AL_BUFFER, (ALint)(_openal_nn->buffer));
	_error = s_openal_get_last_error();
	if (_error)
	{
		std::ignore = snprintf(
			_error_nn,
			W_MAX_PATH,
			"could not set openal source because: %s. trace info: %s",
			_error,
			TRACE);
		return W_FAILURE;
	}

	return W_SUCCESS;
}

int w_openal_update_i16(
	_In_ w_openal p_openal, 
	_In_ const int16_t* p_audio_frame_buffer,
	_In_ size_t p_audio_frame_buffer_len,
    _Inout_ char* p_error)
{
	constexpr auto TRACE = "w_openal_update";

	const auto _error_nn = gsl::not_null<char*>(p_error);
	const auto _openal_nn = gsl::not_null<w_openal>(p_openal);

	ALenum _state;
	ALint _pos;
	alGetSourcei(p_openal->source, AL_SAMPLE_OFFSET, &_pos);
	alGetSourcei(p_openal->source, AL_SOURCE_STATE, &_state);

	const auto _frame_size = _openal_nn->number_of_channels * _openal_nn->size_of_chunk;
	auto _write_offset = _openal_nn->write_pos;

	while (_openal_nn->total_read_bytes < p_audio_frame_buffer_len)
	{
		size_t _read_bytes = 0;
		const size_t _read_offset = _openal_nn->read_pos;
		if (_read_offset > _write_offset)
		{
			/*
			 * Note that the ring buffer's writable space is one byte less
			 * than the available area because the write offset ending up
			 * at the read offset would be interpreted as being empty
			 * instead of full.
			 */
			auto _writable = _read_offset - _write_offset - 1;
			if (_writable < _frame_size)
			{
				break;
			}

			_writable = (_writable > p_audio_frame_buffer_len) ? p_audio_frame_buffer_len : _writable;
			std::memcpy(
				&_openal_nn->data[_write_offset],
				p_audio_frame_buffer,
				_frame_size * gsl::narrow_cast<int>(_writable / _frame_size));

			const auto _num_frames = _writable / _frame_size;
			if (_num_frames < 1)
			{
				break;
			}

			_read_bytes = _num_frames * _frame_size;
			_write_offset += _read_bytes;

			_openal_nn->total_read_bytes += _read_bytes;
		}
		else
		{
			/*
			 * If the read offset is at or behind the write offset, the
			 * writeable area (might) wrap around. Make sure the sample
			 * data can fit, and calculate how much can go in front before
			 * wrapping.
			 */
			size_t writable = !_read_offset ?
				_openal_nn->data_size - _write_offset - 1 :
				(_openal_nn->data_size - _write_offset);
			if (writable < _frame_size)
			{
				break;
			}

			writable = (writable > p_audio_frame_buffer_len) ? p_audio_frame_buffer_len : writable;
			std::memcpy(
				&_openal_nn->data[_write_offset],
				p_audio_frame_buffer,
				_frame_size * gsl::narrow_cast<int>(writable / _frame_size));

			const auto _num_frames = writable / _frame_size;
			if (_num_frames < 1)
			{
				break;
			}

			_read_bytes = _num_frames * _frame_size;
			_write_offset += _read_bytes;
			if (_write_offset == _openal_nn->data_size)
			{
				_write_offset = 0;
			}
			_openal_nn->total_read_bytes += _read_bytes;
		}
		_openal_nn->write_pos = _write_offset;
		_openal_nn->decoder_offset += _read_bytes;
	}

	if (_state != AL_PLAYING && _state != AL_PAUSED)
	{
		/*
			If the source is not playing or paused, it either underrun
			(AL_STOPPED) or is just getting started (AL_INITIAL). If the
			ring buffer is empty, it's done, otherwise play the source with
			what's available.
		 */
		const size_t _readable = ((_write_offset >= _openal_nn->read_pos) ? 
			_write_offset : 
			(_openal_nn->data_size + _write_offset)) - _openal_nn->read_pos;
		if (_readable == 0)
		{
			return W_FAILURE;
		}

		/*
			Store the playback offset that the source will start reading
			from, so it can be tracked during playback.
		 */
		_openal_nn->start_offset = _openal_nn->decoder_offset - _readable;
		alSourcePlay(_openal_nn->source);

		auto _error = s_openal_get_last_error();
		if (_error)
		{
			std::ignore = snprintf(
				_error_nn,
				W_MAX_PATH,
				"error while updating openal because: %s. trace info: %s",
				_error,
				TRACE);
			return W_FAILURE;
		}
	}
	return W_SUCCESS;
}

void w_openal_reset(_In_ w_openal p_openal)
{
	const auto _openal_nn = gsl::not_null<w_openal>(p_openal);

	_openal_nn->read_pos = 0;
	_openal_nn->write_pos = 0;
	_openal_nn->decoder_offset = 0;
	_openal_nn->total_read_bytes = 0;
}

void w_openal_close(_In_ w_openal p_openal, bool p_force_close)
{
	const auto _openal_nn = gsl::not_null<w_openal>(p_openal);

	if (!p_force_close)
	{
		ALenum _state;
		alGetSourcei(_openal_nn->source, AL_SOURCE_STATE, &_state);
		while (_state == AL_PLAYING && _openal_nn->read_pos != _openal_nn->write_pos)
		{
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1s);
		}
	}

	alSourceRewind(_openal_nn->source);
	alSourcei(_openal_nn->source, AL_BUFFER, 0);

	if (_openal_nn->data)
	{
		free(_openal_nn->data);
		_openal_nn->data = nullptr;
	}
}

void w_openal_fini(_Inout_ w_openal* p_openal)
{
	const auto _openal_ptr_nn = gsl::not_null<w_openal*>(p_openal);
	const auto _openal_nn = gsl::not_null<w_openal>(*_openal_ptr_nn);

	// close openal
	w_openal_close(_openal_nn.get(), true);

	if (_openal_nn->source)
	{
		alDeleteSources(1, &_openal_nn->source);
	}

	if (_openal_nn->buffer)
	{
		alDeleteBuffers(1, &_openal_nn->buffer);
	}

	if (_openal_nn->ctx)
	{
		alcDestroyContext(_openal_nn->ctx);
	}
	if (_openal_nn->device)
	{
		alcCloseDevice(_openal_nn->device);
	}

	free(_openal_nn);
	*p_openal = nullptr;
}


