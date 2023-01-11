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

#include <iostream>
#include <concepts>

namespace wolf::media {

namespace internal {

/**
 * @brief spsc fixed-size continuous ring buffer. read/write in chunks.
 *
 * @tparam T an integral type. buffer read/write happens with memcopy.
 */
template <typename T>
    requires std::is_integral_v<T>
class w_ring_buffer {
public:
    using value_type = std::remove_cvref_t<T>;
    using pointer = value_type*;
    using const_pointer = const value_type*;

public:
    w_ring_buffer() noexcept {}
    w_ring_buffer(const w_ring_buffer&) = delete;
    w_ring_buffer(w_ring_buffer&&) noexcept = default;
    ~w_ring_buffer() = default;

    /**
     * @brief initialize the buffer to given size.
     * @param size maximum amount to initialize this buffer with.
     */
    [[nodiscard]] boost::leaf::result<int> init(_In_ std::size_t size) noexcept
    {
        _head_offset = 0;
        _tail_offset = 0;

        _size = size;
        _buffer = std::make_unique<value_type[]>(size);
        if (!_buffer) {
            return W_FAILURE(std::errc::not_enough_memory,
                             "could not allocate ring buffer memory");
        }

        return 0;
    }

    /**
     * @return maximum amount that this buffer can hold.
     */
    [[nodiscard]] std::size_t max_size() const noexcept
    {
        return _size;
    }

    /**
     * @return available size to read.
     */
    [[nodiscard]] std::size_t size() const noexcept
    {
        auto head = _head_offset.load();
        auto tail = _tail_offset.load();

        return (tail < head) ? _size - head : tail - head;
    }

    /**
     * @brief read up to the requested size from buffer.
     *
     * only one thread (cosumer) at a time can call this method, or the behavior is undefined.
     *
     * @param outptr contiguous chunk of memory to copy into.
     * @param size   amount requested. (make sure `outptr` array can hold this much)
     *
     * @return amount that was read into `outptr`.
     */
    std::size_t read(_Out_ pointer outptr, _In_ std::size_t size)
    {
        auto head = _head_offset.load();
        auto tail = _tail_offset.load();

        // available to read continuously.
        const auto available = (tail < head) ? _size - head : tail - head;

        auto amount = (size <= available) ? size : available;
        std::memcpy(outptr, _buffer.get() + head, amount);

        // sets head to either less than or equal to max, or 0
        head = (head + amount) % _size;

        // if more requested and more is available to read on the other side of buffer.
        if (tail < head && amount < size) {
            const auto remaining = size - amount;
            const auto more = (remaining > tail) ? tail : remaining;
            std::memcpy(outptr + amount, _buffer.get(), more);
            amount += more;
            head = more;
        }

        _head_offset = head; // update real head offset...

        return amount;
    }

    /**
     * @brief read up to the given size into buffer.
     *
     * @param data chunk of data to write into buffer.
     * @param size amount of data to write into buffer.
     *
     * @return amount that was written to buffer.
     */
    std::size_t write(_In_ const_pointer data, _In_ std::size_t size)
    {
        auto tail = _tail_offset.load();
        auto head = _head_offset.load();

        // available to write continuously.
        const auto available = (head <= tail) ? _size - tail : tail - head - 1;

        auto amount = (size <= available) ? size : available;
        std::memcpy(_buffer.get() + tail, data, amount);

        tail += amount;

        // if more was given and more is available to write on the other side of buffer.
        if (head < tail && amount < size) {
            const auto remaining = size - amount;
            const auto more = (remaining > head) ? head : remaining;
            std::memcpy(_buffer.get(), data + amount, more);
            amount += more;
            tail = more;
        }

        _tail_offset = tail; // update real tail offset...

        return amount;
    }

private:
    std::size_t _size = 0; //< total/maximum size that buffer can hold.
    std::unique_ptr<value_type[]> _buffer{nullptr};

    alignas(std::hardware_destructive_interference_size)
    std::atomic<std::size_t> _head_offset = 0; //< starting point of valid data in circular buffer.

    alignas(std::hardware_destructive_interference_size)
    std::atomic<std::size_t> _tail_offset = 0; //< end point of valid data in circular buffer. (tail-1 = last byte)
};

}  // namespace internal

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
           _In_ const size_t p_audio_frame_buffer_len)
    {
      // get state
      ALenum _state = 0;
      alGetSourcei(this->_source, AL_SOURCE_STATE, &_state);

      this->_data_buffer.write(
        reinterpret_cast<const ALbyte*>(p_audio_frame_buffer),
        p_audio_frame_buffer_len * sizeof(T)
      );

      if (_state != AL_PLAYING && _state != AL_PAUSED) {
        std::cout << "playing again..." << std::endl;
        //alSourcei(this->_source, AL_SOURCE_RELATIVE, AL_TRUE);
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

  internal::w_ring_buffer<ALbyte> _data_buffer{};

  // The buffer to get the callback, and source to play with
  ALuint _buffer = 0;
  ALuint _source = 0;

  size_t _size_of_chunk = 0;
  LPALBUFFERCALLBACKSOFT _callback_ptr = nullptr;
};

} // namespace wolf::media

#endif
