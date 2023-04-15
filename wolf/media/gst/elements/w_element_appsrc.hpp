#pragma once

#include "media/gst/core/w_element.hpp"
#include "media/gst/core/w_signal_handler.hpp"
#include "media/gst/core/w_caps.hpp"
#include "media/gst/core/w_format.hpp"

#include <gst/gst.h>

namespace wolf::gst {

/**
 * @brief wrappper of appsrc gstreamer element.
 */
class w_element_appsrc : public w_element
{
    constexpr static const char* factory_name = "appsrc";

public:
    [[nodiscard]] static auto make() -> boost::leaf::result<w_element_appsrc>
    {
        BOOST_LEAF_AUTO(base_element, w_element::make(factory_name));
        return w_element_appsrc(std::move(base_element));
    }

    //- properties

    void set_block(bool p_block) {
        g_object_set(raw(), "block", p_block, nullptr);
    }

    void set_caps(w_caps& p_caps)
    {
        g_object_set(raw(), "caps", internal::w_raw_access::raw(p_caps), nullptr);
    }

    void set_format(w_format p_format)
    {
        g_object_set(raw(), "format", static_cast<GstFormat>(p_format), nullptr);
    }

    void set_live(bool p_is_live)
    {
        g_object_set(raw(), "is-live", p_is_live, nullptr);
    }

    void set_max_buffer(std::size_t p_count)
    {
        g_object_set(raw(), "max-buffers", p_count, nullptr);
    }

    void set_max_latency(std::size_t p_nanoseconds)
    {
        g_object_set(raw(), "max-latency", p_nanoseconds, nullptr);
    }

    void set_max_time(std::size_t p_nanoseconds)
    {
        g_object_set(raw(), "max-time", p_nanoseconds, nullptr);
    }

    //- signals

    template <typename F>
    void hook_enough_data(F&& p_callback)
    {
        _sighandlers.enough_data.hook("enough-data", std::forward<F>(p_callback));
    }

    void unhook_enough_data()
    {
        _sighandlers.seek_data.unhook();
    }

    template <typename F>
    void hook_need_data(F&& p_callback)
    {
        _sighandlers.need_data.hook("need-data", std::forward<F>(p_callback));
    }

    void unhook_need_data()
    {
        _sighandlers.seek_data.unhook();
    }

    template <typename F>
    void hook_seek_data(F&& p_callback)
    {
        _sighandlers.seek_data.hook("seek-data", std::forward<F>(p_callback));
    }

    void unhook_seek_data()
    {
        _sighandlers.seek_data.unhook();
    }

    //- action signals

    bool emit_eos()
    {
        GstFlowReturn ret;
        g_signal_emit_by_name(raw(), "end-of-stream", &ret);
        return static_cast<bool>(ret);
    }

    template <typename BufferT>
    bool emit_push_buffer(BufferT&& p_buffer)
    {
        GstFlowReturn ret;
        g_signal_emit_by_name(raw(), "push-buffer", internal::w_raw_access::raw(p_buffer), &ret);
        return static_cast<bool>(ret);
    }

private:
    explicit w_element_appsrc(w_element&& p_base)
        : w_element(std::move(p_base))
        , _sighandlers(G_OBJECT(raw()))
    {}

    // signals
    struct signal_handler_set {
        w_signal_handler<> enough_data;
        w_signal_handler<std::size_t> need_data; /// params: length
        w_signal_handler<std::size_t> seek_data; /// params: offset

        signal_handler_set(auto* p_rawptr)
            : enough_data(p_rawptr)
            , need_data(p_rawptr)
            , seek_data(p_rawptr)
        {}
    } _sighandlers;
};

}  // namespace wolf::gst
