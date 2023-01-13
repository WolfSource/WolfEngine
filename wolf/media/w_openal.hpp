/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_MEDIA_OPENAL

#pragma once

#include <wolf.hpp>
#include <system/w_ring_buffer_spsc.hpp>

#include <DISABLE_ANALYSIS_BEGIN>
#include <AL/al.h>
#include <AL/alext.h>
#include <DISABLE_ANALYSIS_END>

#include <iostream>
#include <concepts>

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
  // constructor
  W_API w_openal(_In_ const w_openal_config &p_config) noexcept;

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
  boost::leaf::result<int> init();

  /**
   * update the openal
   * @param p_audio_frame_buffer, the audio frame buffer
   * @param p_audio_frame_buffer_len, the length of audio frame buffer
   * @returns zero on success
   */
  template <typename T>
    requires std::is_integral_v<T>
  W_API boost::leaf::result<int>
  update(_In_ const T *p_audio_frame_buffer,
         _In_ const size_t p_audio_frame_buffer_len) {
    // get state
    ALenum _state = 0;
    alGetSourcei(this->_source, AL_SOURCE_STATE, &_state);

    this->_data_buffer.put(
        reinterpret_cast<const ALbyte *>(p_audio_frame_buffer),
        p_audio_frame_buffer_len * sizeof(T));

    if (_state != AL_PLAYING && _state != AL_PAUSED) {
      std::cout << "playing again..." << std::endl;
      // alSourcei(this->_source, AL_SOURCE_RELATIVE, AL_TRUE);
      alSourcePlay(this->_source);

      auto _error = get_last_error();
      if (_error != std::string("")) {
        return W_FAILURE(std::errc::operation_canceled,
                         "error while updating openal because: " + _error);
      }
    }
    return S_OK;
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

  wolf::system::w_ring_buffer_spsc<ALbyte> _data_buffer{};

  // The buffer to get the callback, and source to play with
  ALuint _buffer = 0;
  ALuint _source = 0;

  size_t _size_of_chunk = 0;
  LPALBUFFERCALLBACKSOFT _callback_ptr = nullptr;
};
} // namespace wolf::media

#endif
