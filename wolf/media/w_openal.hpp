/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_MEDIA_OPENAL

#pragma once

#include <wolf.hpp>

#include <DISABLE_ANALYSIS_BEGIN>
#include <AL/al.h>
#include <AL/alext.h>
#include <DISABLE_ANALYSIS_END>

namespace wolf::media {

struct w_openal_config {
  // the audio format
  ALenum format = AL_FORMAT_STEREO16;
  // sample rate of audio
  ALsizei sample_rate = 44100;
  // refresh rate of audio
  int refresh_rate = 25;
  // number of channels
  int channels = 2;
};

class w_openal {
public:
  // default constructor
  W_API w_openal() noexcept = default;

  // move constructor.
  W_API w_openal(w_openal &&p_other) noexcept;
  // move assignment operator.
  W_API w_openal &operator=(w_openal &&p_other) = default;

  // destructor
  W_API virtual ~w_openal() noexcept;

  /**
   * initialize the openal
   * @param p_config, the openal audio
   * returns zero on success as result format
   */
  W_API
  boost::leaf::result<int>
  init(_In_ const w_openal_config &p_config) noexcept;

  /**
   * update the openal
   * @param p_audio_frame_buffer, the audio frame buffer
   * @param p_audio_frame_buffer_len, the length of audio frame buffer
   * @returns zero on success
   */
   template <typename T>
     requires std::is_integral_v<int16_t> || std::is_integral_v<int8_t>
   W_API boost::leaf::result<int>
   update(_In_ const T *p_audio_frame_buffer,
          _In_ const size_t p_audio_frame_buffer_len) {
     // get state and pos
     ALenum _state = 0;
     ALint _pos = 0;
     alGetSourcei(this->_source, AL_SAMPLE_OFFSET, &_pos);
     alGetSourcei(this->_source, AL_SOURCE_STATE, &_state);

    const auto _frame_size = this->_config.channels * this->_size_of_chunk;
    auto _write_offset = this->_write_pos;

    while (this->_total_read_bytes < p_audio_frame_buffer_len) {
      size_t _read_bytes = 0;
      const size_t _read_offset = this->_read_pos;
      if (_read_offset > _write_offset) {
        /*
         * Note that the ring buffer's writable space is one byte less
         * than the available area because the write offset ending up
         * at the read offset would be interpreted as being empty
         * instead of full.
         */
        auto _writable = _read_offset - _write_offset - 1;
        if (_writable < _frame_size) {
          break;
        }

        _writable = (_writable > p_audio_frame_buffer_len)
                        ? p_audio_frame_buffer_len
                        : _writable;
        std::memcpy(&this->_data[_write_offset], p_audio_frame_buffer,
                    _frame_size *
                        gsl::narrow_cast<int>(_writable / _frame_size));

        const auto _num_frames = _writable / _frame_size;
        if (_num_frames < 1) {
          break;
        }

        _read_bytes = _num_frames * _frame_size;
        _write_offset += _read_bytes;

        this->_total_read_bytes += _read_bytes;
      } else {
        /*
         * If the read offset is at or behind the write offset, the
         * writeable area (might) wrap around. Make sure the sample
         * data can fit, and calculate how much can go in front before
         * wrapping.
         */
        auto _writable = !_read_offset ? this->_data_size - _write_offset - 1
                                       : (this->_data_size - _write_offset);
        if (_writable < _frame_size) {
          break;
        }

        _writable = (_writable > p_audio_frame_buffer_len)
                        ? p_audio_frame_buffer_len
                        : _writable;

        std::memcpy(&this->_data[_write_offset], p_audio_frame_buffer,
                    _frame_size *
                        gsl::narrow_cast<int>(_writable / _frame_size));

        const auto _num_frames = _writable / _frame_size;
        if (_num_frames < 1) {
          break;
        }

        _read_bytes = _num_frames * _frame_size;
        _write_offset += _read_bytes;
        if (_write_offset == this->_data_size) {
          _write_offset = 0;
        }
        this->_total_read_bytes += _read_bytes;
      }
      this->_write_pos = _write_offset;
      this->_decoder_offset += _read_bytes;
    }

    if (_state != AL_PLAYING && _state != AL_PAUSED) {

      /*
        if the source is not playing or paused, it either underrun
        (AL_STOPPED) or is just getting started (AL_INITIAL).
        if the ring buffer is empty, it's done,
        otherwise play the source with what's available.
      */
      const auto _readable = ((_write_offset >= this->_read_pos)
                                  ? _write_offset
                                  : (this->_data_size + _write_offset)) -
                             this->_read_pos;
      if (_readable == 0) {
        return W_FAILURE(std::errc::operation_canceled,
                     "no openal data avaiable for reading");
      }

      /*
       * store the playback offset that the source will start reading from,
       * so it can be tracked during playback.
       */
      this->_start_offset = this->_decoder_offset - _readable;
      alSourcePlay(this->_source);

      auto _error = get_last_error();
      if (!_error.empty()) {
        return W_FAILURE(std::errc::operation_canceled,
                     "error while updating openal because: " + _error);
      }
    }
    
    this->_total_read_bytes = 0;
    
    return 0;
  }

    /**
     * reset the openal
     * @returns void
     */
    W_API
    void reset();
  
    /**
     * get openal config
     * @returns the openal config
     */
    W_API
    w_openal_config get_config() const;
  
    /**
     * returns all output/input devices
     * @returns output_devices, input_devices
     */
    W_API
    static std::tuple<std::string, std::string> get_all_devices();
  
    /**
     * get the last error
     * @returns last error of openal
     */
    W_API
    static std::string get_last_error();
  
   private:
    // disable copy constructor
    w_openal(const w_openal &) = delete;
    // disable copy operator
    w_openal &operator=(const w_openal &) = delete;

  static ALsizei AL_APIENTRY s_openal_callback(_In_ void *p_user_ptr,
                                               _In_ void *p_data,
                                               _In_ ALsizei p_size);
  void _release() noexcept;

  w_openal_config _config = {};

  ALCdevice *_device = nullptr;
  ALCcontext *_ctx = nullptr;

  // a lockless ring-buffer (supports single-provider, single-consumer
  // operation)
  ALbyte *_data = nullptr;
  size_t _data_size = 0;
  size_t _read_pos = 0;
  size_t _write_pos = 0;

  // The buffer to get the callback, and source to play with
  ALuint _buffer = 0;
  ALuint _source = 0;
  size_t _start_offset = 0;

  // Handle for the audio file to decode
  size_t _decoder_offset = 0;

  size_t _total_read_bytes = 0;
  size_t _size_of_chunk = 0;
  LPALBUFFERCALLBACKSOFT _callback_ptr = nullptr;
};
} // namespace wolf::media

#endif



