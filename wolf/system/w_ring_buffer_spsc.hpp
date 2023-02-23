/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <wolf/wolf.hpp>

#include <memory>
#include <type_traits>

namespace wolf::system {

/**
 * @brief spsc fixed-size continuous ring buffer. read/write in chunks.
 *
 * @tparam T an integral type. buffer read/write happens with memcopy.
 */
template <typename T>
  requires std::is_integral_v<T>
class w_ring_buffer_spsc {
 public:
  using value_type = std::remove_cvref_t<T>;

  w_ring_buffer_spsc() noexcept = default;
  w_ring_buffer_spsc(w_ring_buffer_spsc &&) noexcept = default;
  ~w_ring_buffer_spsc() = default;

  /**
   * @brief initialize the buffer to given size.
   * @param size maximum amount to initialize this buffer with.
   */
  [[nodiscard]] boost::leaf::result<int> init(_In_ std::size_t size) noexcept {
    _head_offset = 0;
    _tail_offset = 0;

    _size = size;
    _buffer = std::make_unique<value_type[]>(size);
    if (!_buffer) {
      return W_FAILURE(std::errc::not_enough_memory, "could not allocate ring buffer memory");
    }

    return 0;
  }

  /**
   * @return maximum amount that this buffer can hold.
   */
  [[nodiscard]] std::size_t max_size() const noexcept { return _size; }

  /**
   * @return available size to read.
   */
  [[nodiscard]] std::size_t size() const noexcept {
    auto head = _head_offset.load();
    auto tail = _tail_offset.load();

    return (tail < head) ? _size - head : tail - head;
  }

  /**
   * @brief read up to the requested size from buffer.
   *
   * only one thread (consumer) at a time can call this method, or the behavior
   * is undefined.
   *
   * @param p_out_ptr, contiguous chunk of memory to copy into.
   * @param size, amount requested. (make sure `p_out_ptr` array can hold this
   * much)
   *
   * @return amount that was read into `p_out_ptr`.
   */
  std::size_t get(_Out_ value_type *p_out_ptr, _In_ std::size_t p_size) {
    auto head = _head_offset.load();
    auto tail = _tail_offset.load();

    // available to read continuously.
    const auto available = (tail < head) ? _size - head : tail - head;

    auto amount = (p_size <= available) ? p_size : available;
    std::memcpy(p_out_ptr, _buffer.get() + head, amount);

    // sets head to either less than or equal to max, or 0
    head = (head + amount) % _size;

    // if more requested and more is available to read on the other side of
    // buffer.
    if (tail < head && amount < p_size) {
      const auto remaining = p_size - amount;
      const auto more = (remaining > tail) ? tail : remaining;
      std::memcpy(p_out_ptr + amount, _buffer.get(), more);
      amount += more;
      head = more;
    }

    _head_offset = head;  // update real head offset...

    return amount;
  }

  /**
   * @brief write the given size into the buffer.
   *
   * @param data chunk of data to write into buffer.
   * @param size amount of data to write into buffer.
   *
   * @return amount that was written to buffer.
   */
  std::size_t put(_In_ const value_type *p_data, _In_ std::size_t p_size) {
    auto tail = _tail_offset.load();
    auto head = _head_offset.load();

    // available to write continuously.
    const auto available = (head <= tail) ? _size - tail : tail - head - 1;

    auto amount = (p_size <= available) ? p_size : available;
    std::memcpy(_buffer.get() + tail, p_data, amount);

    tail += amount;

    // if more was given and more is available to write on the other side of
    // buffer.
    if (head < tail && amount < p_size) {
      const auto remaining = p_size - amount;
      const auto more = (remaining > head) ? head : remaining;
      std::memcpy(_buffer.get(), p_data + amount, more);
      amount += more;
      tail = more;
    }

    _tail_offset = tail;  // update real tail offset...

    return amount;
  }

 private:
  // copy conclassor.
  w_ring_buffer_spsc(const w_ring_buffer_spsc &) = delete;
  // copy assignment operator.
  w_ring_buffer_spsc &operator=(const w_ring_buffer_spsc &) = delete;

  std::size_t _size = 0;  //< total/maximum size that buffer can hold.
  std::unique_ptr<value_type[]> _buffer{nullptr};

  alignas(std::hardware_destructive_interference_size) std::atomic<std::size_t> _head_offset =
      0;  //< starting point of valid data in circular buffer.

  alignas(std::hardware_destructive_interference_size) std::atomic<std::size_t> _tail_offset =
      0;  //< end point of valid data in circular buffer. (tail-1 = last byte)
};
}  // namespace wolf::system
