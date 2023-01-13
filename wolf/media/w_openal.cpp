#ifdef WOLF_MEDIA_OPENAL

#include "w_openal.hpp"

using w_openal = wolf::media::w_openal;
using w_openal_config = wolf::media::w_openal_config;

ALsizei AL_APIENTRY w_openal::s_openal_callback(_In_ void *p_user_ptr,
                                                _In_ void *p_data,
                                                _In_ ALsizei p_size) {
  const auto _openal_nn = gsl::narrow_cast<w_openal *>(p_user_ptr);
  ALsizei _bytes =
      _openal_nn->_data_buffer.get(gsl::narrow_cast<ALbyte *>(p_data), p_size);
  std::memset(gsl::narrow_cast<ALbyte *>(p_data) + _bytes, 0, p_size - _bytes);

  return p_size;
}

w_openal::w_openal(_In_ const w_openal_config &p_config) noexcept
    : _config(p_config), _device(nullptr), _ctx(nullptr), _buffer(0),
      _source(0), _size_of_chunk(0), _callback_ptr(nullptr) {}

w_openal::w_openal(w_openal &&p_other) noexcept {

  if (this == &p_other)
    return;

  _release();

  this->_config = std::move(p_other._config);
  this->_device = std::exchange(p_other._device, nullptr);
  this->_ctx = std::exchange(p_other._ctx, nullptr);
  this->_buffer = std::exchange(p_other._buffer, 0);
  this->_source = std::exchange(p_other._source, 0);
  this->_size_of_chunk = std::exchange(p_other._size_of_chunk, 0);
  this->_callback_ptr = std::exchange(p_other._callback_ptr, nullptr);
}

w_openal::~w_openal() noexcept { _release(); }

void w_openal::reset() {
  ALenum state;
  alGetSourcei(this->_source, AL_SOURCE_STATE, &state);
  if (state == AL_PLAYING)
    alSourceStop(this->_source);
}

std::tuple<std::string, std::string> w_openal::get_all_devices() {
  std::string _input_devices;
  std::string _output_devices;

  if (alcIsExtensionPresent(nullptr, "ALC_enumeration_EXT") == AL_TRUE) {
    if (alcIsExtensionPresent(nullptr, "ALC_enumerate_all_EXT") == AL_TRUE) {
      _output_devices = alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER);
    } else {
      _output_devices = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
    }
    _input_devices = alcGetString(nullptr, ALC_CAPTURE_DEVICE_SPECIFIER);
  }

  return std::make_tuple(_output_devices, _input_devices);
}

std::string w_openal::get_last_error() {
  const auto _error = alGetError();
  switch (_error) {
  default:
    return "unknown error code";
  case AL_NO_ERROR:
    return "";
  case AL_INVALID_NAME:
    return "AL_INVALID_NAME";
  case AL_INVALID_ENUM:
    return "AL_INVALID_ENUM";
  case AL_INVALID_VALUE:
    return "AL_INVALID_VALUE";
  case AL_INVALID_OPERATION:
    return "AL_INVALID_OPERATION";
  case AL_OUT_OF_MEMORY:
    return "AL_OUT_OF_MEMORY";
  }
}

w_openal_config w_openal::get_config() const { return this->_config; }

boost::leaf::result<int> w_openal::init() {
  auto _ret = S_OK;
  DEFER {
    if (_ret != S_OK) {
      _release();
    }
  });

  // open and initialize a audio device
  this->_device = alcOpenDevice(nullptr);
  if (this->_device == nullptr) {
    _ret = -1;
    return W_FAILURE(std::errc::operation_canceled,
                     "could not open a openal device");
  }

  this->_ctx = alcCreateContext(this->_device, nullptr);
  if (this->_ctx == nullptr || alcMakeContextCurrent(this->_ctx) == ALC_FALSE) {
    _ret = -1;
    return W_FAILURE(std::errc::operation_canceled,
                     "could not get openal context");
  }

  // get the sound format, and figure out the OpenAL format
  const auto _format = this->_config.format;
  const auto _sample_rate = this->_config.sample_rate;
  const auto _number_of_channels = this->_config.channels;

  switch (_format) {
  default:
  case AL_FORMAT_MONO16:
  case AL_FORMAT_STEREO16: {
    // Signed 16-bit buffer format
    this->_size_of_chunk = sizeof(int16_t);
    break;
  }
  case AL_FORMAT_MONO8:
  case AL_FORMAT_STEREO8: {
    // Unsigned 8-bit buffer format
    this->_size_of_chunk = sizeof(uint8_t);
    break;
  }
  }

  // generate the buffer
  alGenBuffers(1, &this->_buffer);
  auto _error = get_last_error();
  if (!_error.empty()) {
    _ret = -1;
    return W_FAILURE(std::errc::operation_canceled,
                     "could not generate buffer for openAL because " + _error);
  }

  alGenSources(1, &this->_source);
  _error = get_last_error();
  if (!_error.empty()) {
    _ret = -1;
    return W_FAILURE(std::errc::operation_canceled,
                     "could not generate sources for openAL");
  }

  if (alIsExtensionPresent("AL_SOFTX_callback_buffer") != 0) {
    _ret = -1;
    return W_FAILURE(std::errc::operation_canceled,
                     "could not get AL_SOFT_callback_buffer");
  }

  this->_callback_ptr = reinterpret_cast<LPALBUFFERCALLBACKSOFT>(
      alGetProcAddress("alBufferCallbackSOFT"));
  if (this->_callback_ptr == nullptr) {
    _ret = -1;
    return W_FAILURE(std::errc::operation_canceled,
                     "could not get LPALBUFFERCALLBACKSOFT");
  }

  alcGetIntegerv(this->_device, ALC_REFRESH, 1, &this->_config.refresh_rate);

  // allocate for 1 second of buffer.
  BOOST_LEAF_CHECK(_data_buffer.init(
      gsl::narrow_cast<ALuint>(_sample_rate * _number_of_channels) *
      this->_size_of_chunk));

  this->_callback_ptr(this->_buffer, _format, _sample_rate, s_openal_callback,
                      this /* send openal object on user's data*/
  );

  alSourcei(this->_source, AL_BUFFER, (ALint)(this->_buffer));
  _error = get_last_error();
  if (!_error.empty()) {
    _ret = -1;
    return W_FAILURE(std::errc::not_enough_memory,
                     "could not set openal source because " + _error);
  }

  return _ret;
}

void w_openal::_release() noexcept {
  if (this->_source != 0) {
    alSourceRewind(this->_source);
    alSourcei(this->_source, AL_BUFFER, 0);
    alDeleteSources(1, &this->_source);
  }

  if (this->_buffer != 0) {
    alDeleteBuffers(1, &this->_buffer);
  }

  if (this->_ctx != nullptr) {
    alcDestroyContext(this->_ctx);
  }
  if (this->_device != nullptr) {
    alcCloseDevice(this->_device);
  }
}

#endif
