#ifdef WOLF_MEDIA_OPENAL

#include "w_openal.hpp"

#include <iostream>

using w_openal = wolf::media::w_openal;
using w_openal_config = wolf::media::w_openal_config;

ALsizei AL_APIENTRY w_openal::s_openal_callback(_In_ void *p_user_ptr,
                                                _In_ void *p_data,
                                                _In_ ALsizei p_size) {
  const auto _openal_nn = gsl::narrow_cast<w_openal *>(p_user_ptr);
  size_t _bytes = 0;

  auto _read_offset = _openal_nn->_read_pos;
  while (_bytes < p_size) {
    /* If the write offset == read offset, there's nothing left in the
     * ring-buffer. Break from the loop and give what has been written.
     */
    const auto _write_offset = _openal_nn->_write_pos;
    if (_write_offset == _read_offset) {
      break;
    }

    // If the write offset is behind the read offset, the readable
    // portion wrapped around. Just read up to the end of the buffer in
    // that case, otherwise read up to the write offset. Also limit the
    // amount to copy given how much is remaining to write.
    auto _todo = ((_write_offset < _read_offset) ? _openal_nn->_data_size
                                                 : _write_offset) -
                 _read_offset;
    _todo = _todo > (p_size - _bytes) ? (p_size - _bytes) : _todo;

    // copy from the ring buffer to the provided output buffer.
    // wrap the resulting read offset if it reached the end of the ring-buffer.

    memcpy(p_data, &_openal_nn->_data[_read_offset], _todo);
    p_data = gsl::narrow_cast<ALbyte *>(p_data) + _todo;
    _bytes += gsl::narrow_cast<ALsizei>(_todo);

    _read_offset += _todo;
    if (_read_offset == _openal_nn->_data_size) {
      _read_offset = 0;
    }
  }
  // finally, store the updated read offset, and return how many bytes have been
  // written
  _openal_nn->_read_pos = _read_offset;

  return gsl::narrow_cast<ALsizei>(_bytes);
}

w_openal::w_openal(w_openal &&p_other) noexcept {

  if (this == &p_other)
    return;

  _release();

  this->_config = std::move(p_other._config);
  this->_device = std::exchange(p_other._device, nullptr);
  this->_ctx = std::exchange(p_other._ctx, nullptr);
  this->_data = std::exchange(p_other._data, nullptr);
  this->_data_size = std::exchange(p_other._data_size, 0);
  this->_read_pos = std::exchange(p_other._read_pos, 0);
  this->_write_pos = std::exchange(p_other._write_pos, 0);
  this->_buffer = std::exchange(p_other._buffer, 0);
  this->_source = std::exchange(p_other._source, 0);
  this->_start_offset = std::exchange(p_other._start_offset, 0);
  this->_decoder_offset = std::exchange(p_other._decoder_offset, 0);
  this->_total_read_bytes = std::exchange(p_other._total_read_bytes, 0);
  this->_size_of_chunk = std::exchange(p_other._size_of_chunk, 0);
  this->_callback_ptr = std::exchange(p_other._callback_ptr, nullptr);
}

w_openal::~w_openal() noexcept { _release(); }

 void w_openal::reset() {
  this->_read_pos = 0;
  this->_write_pos = 0;
  this->_decoder_offset = 0;
  this->_total_read_bytes = 0;
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

boost::leaf::result<int>
w_openal::init(_In_ const w_openal_config &p_config) noexcept {

  auto _ret = 0;
  this->_config = p_config;

  DEFER{
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

  //this->_ctx = alcCreateContext(this->_device, nullptr);
  //if (this->_ctx == nullptr || alcMakeContextCurrent(this->_ctx) == ALC_FALSE) {
  //  _ret = -1;
  //  return W_FAILURE(std::errc::operation_canceled, "could not get openal context");
  //}

  //// get the sound format, and figure out the OpenAL format
  //const auto _format = this->_config.format;
  //const auto _sample_rate = this->_config.sample_rate;
  //const auto _number_of_channels = this->_config.channels;

  //this->_data = nullptr;

  //switch (_format) {
  //default:
  //case AL_FORMAT_MONO16:
  //case AL_FORMAT_STEREO16: {
  //  // Signed 16-bit buffer format
  //  this->_size_of_chunk = sizeof(int16_t);
  //  break;
  //}
  //case AL_FORMAT_MONO8:
  //case AL_FORMAT_STEREO8: {
  //  // Unsigned 8-bit buffer format
  //  this->_size_of_chunk = sizeof(uint8_t);
  //  break;
  //}
  //}

  //// generate the buffer
  //alGenBuffers(1, &this->_buffer);
  //auto _error = get_last_error();
  //if (!_error.empty()) {
  //  _ret = -1;
  //  return W_FAILURE(std::errc::operation_canceled,
  //               "could not generate buffer for openAL because " + _error);
  //}

  //alGenSources(1, &this->_source);
  //_error = get_last_error();
  //if (!_error.empty()) {
  //  _ret = -1;
  //  return W_FAILURE(std::errc::operation_canceled,
  //               "could not generate sources for openAL");
  //}

  //if (alIsExtensionPresent("AL_SOFTX_callback_buffer") != 0) {
  //  _ret = -1;
  //  return W_FAILURE(std::errc::operation_canceled,
  //               "could not get AL_SOFT_callback_buffer");
  //}

  //this->_callback_ptr = gsl::narrow_cast<LPALBUFFERCALLBACKSOFT>(
  //    alGetProcAddress("alBufferCallbackSOFT"));
  //if (this->_callback_ptr == nullptr) {
  //  _ret = -1;
  //  return W_FAILURE(std::errc::operation_canceled,
  //               "could not get LPALBUFFERCALLBACKSOFT");
  //}

  //alcGetIntegerv(this->_device, ALC_REFRESH, 1, &this->_config.refresh_rate);

  //// set a 1s ring buffer size
  //this->_data_size =
  //    gsl::narrow_cast<ALuint>(_sample_rate * _number_of_channels) *
  //    this->_size_of_chunk * sizeof(float);

  //if (this->_data != nullptr) {
  //  free(this->_data);
  //}
  //this->_data = gsl::narrow_cast<ALbyte *>(calloc(1, this->_data_size));
  //if (this->_data == nullptr) {
  //  _ret = -1;
  //  return W_FAILURE(std::errc::not_enough_memory,
  //               std::format("could not allocate {} byets for openal buffer",
  //                           this->_data_size));
  //}
  //this->_read_pos = 0;
  //this->_write_pos = 0;
  //this->_decoder_offset = 0;
  //this->_callback_ptr(this->_buffer, _format, _sample_rate, s_openal_callback,
  //                    this /* send openal object on user's data*/
  //);

  //alSourcei(this->_source, AL_BUFFER, (ALint)(this->_buffer));
  //_error = get_last_error();
  //if (!_error.empty()) {
  //  _ret = -1;
  //  return W_FAILURE(std::errc::not_enough_memory,
  //               "could not set openal source because " + _error);
  //}

  return _ret;
}

void w_openal::_release() noexcept {
  if (this->_source != 0) {
    alSourceRewind(this->_source);
    alSourcei(this->_source, AL_BUFFER, 0);
    alDeleteSources(1, &this->_source);
  }

  if (this->_data != nullptr) {
    free(this->_data);
    this->_data = nullptr;
  }

  if (this->_buffer != 0) {
    alDeleteBuffers(1, &this->_buffer);
  }

  if (this->_ctx != nullptr) {
    alcDestroyContext(this->_ctx);
  }
  if (this->_device != nullptr) {
    alcCloseDevice(this->_device);
    this->_device = nullptr;
  }

  this->_callback_ptr = nullptr;
}

#endif


