/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <wolf.hpp>

#include <type_traits>
#include <memory>

namespace wolf::system {

/**
 * @brief spsc fixed-size continuous ring buffer. read/write in chunks.
 *
 * @tparam T an integral type. buffer read/write happens with memcopy.
 */
template <typename T>
    requires std::is_integral_v<T>
class w_spsc_ring_buffer {
public:
    using value_type = std::remove_cvref_t<T>;
    using pointer = value_type*;
    using const_pointer = const value_type*;

public:
    w_spsc_ring_buffer() noexcept {}
    w_spsc_ring_buffer(const w_spsc_ring_buffer&) = delete;
    w_spsc_ring_buffer(w_spsc_ring_buffer&&) noexcept = default;
    ~w_spsc_ring_buffer() = default;

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

}  // namespace wolf::system

