#pragma once

#include "wolf.hpp"

#include "media/gst/internal/w_wrapper.hpp"

#include <gst/gst.h>

#include <cstddef>
#include <stdexcept>

namespace wolf::media::gst {

/**
 * @brief read/write map flags.
 */
enum class w_buffer_map_flags
{
    Read = GST_MAP_READ,
    Write = GST_MAP_WRITE,
    ReadWrite = GST_MAP_READWRITE
};

class w_buffer;

/**
 * helper class to map underlying data of w_buffer
 * to an accessible memory region and unmap on destruction.
 */
class w_buffer_mapped_data
{
    friend class w_buffer;

public:
    w_buffer_mapped_data(const w_buffer_mapped_data&) = delete;
    w_buffer_mapped_data(w_buffer_mapped_data&& p_other) noexcept
        : _buffer(std::exchange(p_other._buffer, nullptr))
        , _map(std::exchange(p_other._map, GstMapInfo{}))
    {}

    w_buffer_mapped_data& operator=(const w_buffer_mapped_data&) = delete;
    w_buffer_mapped_data& operator=(w_buffer_mapped_data&& p_other) noexcept
    {
        std::swap(_buffer, p_other._buffer);
        std::swap(_map, p_other._map);
        return *this;
    }

    ~w_buffer_mapped_data() noexcept
    {
        if (_buffer) {
            gst_buffer_unmap(_buffer, &_map);
        }
    }

    [[nodiscard]] std::size_t size() const noexcept { return _map.size; }

    [[nodiscard]] guint8* begin() noexcept { return _map.data; }
    [[nodiscard]] const guint8* begin() const noexcept { return _map.data; }

    [[nodiscard]] guint8* end() noexcept { return _map.data + _map.size; }
    [[nodiscard]] const guint8* end() const noexcept { return _map.data + _map.size; }

    [[nodiscard]] guint8* data() noexcept { return _map.data; }
    [[nodiscard]] const guint8* data() const noexcept { return _map.data; }

    [[nodiscard]] guint8& operator[](std::size_t p_index) { return _map.data[p_index]; }
    [[nodiscard]] const guint8& operator[](std::size_t p_index) const { return _map.data[p_index]; }

private:
    w_buffer_mapped_data() noexcept {}

    /**
     * @brief helper method to map raw buffer to a normal accessible data region.
     * @param p_buffer raw gstreamer buffer.
     * @param p_flags read/write flags. defaults to read-write.
     */
    [[nodiscard]] static auto make(internal::w_raw_tag,
                                   GstBuffer* p_buffer,
                                   w_buffer_map_flags p_flags = w_buffer_map_flags::ReadWrite)
        -> boost::leaf::result<w_buffer_mapped_data>
    {
        auto ret = w_buffer_mapped_data();

        auto flags_raw = static_cast<GstMapFlags>(p_flags);
        if (!p_buffer || !gst_buffer_map(p_buffer, &ret._map, flags_raw)) {
            return W_FAILURE(std::errc::operation_canceled, "couldn't map the buffer data.");
        }

        ret._buffer = p_buffer;

        return ret;
    }

    GstBuffer* _buffer = nullptr;
    GstMapInfo _map{};
};

/**
 * wrapper of GstBuffer.
 */
class w_buffer : public w_wrapper<w_buffer, GstBuffer, void, gst_buffer_unref>
{
    friend class internal::w_raw_access;

public:
    /**
     * @brief make a buffer with given size.
     * @return buffer on success.
     */
    [[nodiscard]] static auto make(std::size_t p_size) -> boost::leaf::result<w_buffer>;

    /**
     * @brief get timestamp in nanoseconds.
     */
    [[nodiscard]] auto get_timestamp() const noexcept
    {
        return GST_BUFFER_TIMESTAMP(raw());
    }

    /**
     * @brief get duration of this buffer's playback in nanoseconds.
     */
    [[nodiscard]] auto get_duration() const noexcept
    {
        return raw()->duration;
    }

    /**
     * @brief set timestamp in nanoseconds.
     */
    void set_timestamp(std::size_t p_nanoseconds) noexcept
    {
        GST_BUFFER_TIMESTAMP(raw()) = static_cast<GstClockTime>(p_nanoseconds);
    }

    /**
     * @brief set duration of this buffer's playback in nanoseconds.
     */
    void set_duration(std::size_t p_nanoseconds) noexcept
    {
        raw()->duration = static_cast<GstClockTime>(p_nanoseconds);
    }

    /**
     * @brief map to write-only data region.
     */
    [[nodiscard]] auto map_data_write()
    {
        return w_buffer_mapped_data::make(
            internal::w_raw_tag{},
            raw(),
            w_buffer_map_flags::Write
        );
    }

    /**
     * @brief map to read-only data region.
     */
    [[nodiscard]] auto map_data_read() const
    {
        // NOTE unfortunately raw methods need pointer to non-const data, thus const_cast.
        return w_buffer_mapped_data::make(
            internal::w_raw_tag{},
            const_cast<GstBuffer*>(raw()),
            w_buffer_map_flags::Read
        );
    }

    /**
     * @brief map to readable-writable data region.
     */
    [[nodiscard]] auto map_data()
    {
        return w_buffer_mapped_data::make(internal::w_raw_tag{}, raw());
    }

    /**
     * @brief map to readable data region.
     */
    [[nodiscard]] auto map_data() const
    {
        return map_data_read();
    }

private:
    explicit w_buffer(internal::w_raw_tag, GstBuffer* p_buffer) noexcept
        : w_wrapper(p_buffer)
    {}
};

}  // namespace wolf::media::gst
