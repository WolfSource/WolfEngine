#pragma once

#include "media/gst/core/w_signal_handler.hpp"
#include "media/gst/core/w_message.hpp"
#include "media/gst/internal/w_wrapper.hpp"

#include <gst/gst.h>

namespace wolf::gst {

/**
 * @brief wrapper of GstBus. a bus to send messages to and notify listeners.
 */
class w_bus : public w_wrapper<w_bus, GstBus, void, gst_object_unref>
{
    friend class internal::w_raw_access;

public:
    w_bus() = delete;

    w_bus(const w_bus&) = delete;
    w_bus(w_bus&&) noexcept = default;

    w_bus& operator=(const w_bus&) = delete;
    w_bus& operator=(w_bus&&) noexcept = default;

    ~w_bus() noexcept
    {
        for (auto i = 0u; i < _watch_count; ++i) {
            gst_bus_remove_signal_watch(raw());
        }
    }

    /**
     * @brief hook a listener handler on message signal.
     * @param p_handler message listener handler. it will be passed `w_nonowning<w_message>`.
     */
    template <typename F>
    void hook_message(F&& p_handler)
    {
        ensure_watch();
        constexpr auto invoker = +[](GstBus* /* p_self */, GstMessage* p_message, gpointer p_callee) {
            auto& func = (*static_cast<decltype(_sighandlers.message)::handler_type*>(p_callee));
            func(internal::w_raw_access::from_raw<w_nonowning<w_message>>(p_message));
        };
        _sighandlers.message.hook("message", invoker, std::forward<F>(p_handler));
    }

    /**
     * @brief unhook the message listener handler on message signal.
     */
    void unhook_message()
    {
        if (_sighandlers.message.unhook()) {
            gst_bus_remove_signal_watch(raw());
            --_watch_count;
        }
    }

private:
    explicit w_bus(internal::w_raw_tag, GstBus* p_bus_raw) noexcept
        : w_wrapper(p_bus_raw)
        , _sighandlers(G_OBJECT(p_bus_raw))
    {}

    /**
     * @brief make sure there is at least one signal watch.
     */
    void ensure_watch() noexcept
    {
        if (_watch_count == 0) {
            watch();
        }
    }

    /**
     * @brief add a signal watch so a listener can be added.
     */
    void watch() noexcept
    {
        gst_bus_add_signal_watch(raw());
        ++_watch_count;
    }

    // signals
    struct signal_handler_set {
        w_signal_handler<w_nonowning<w_message>> message;

        signal_handler_set(auto* p_rawptr) : message(p_rawptr) {}
    } _sighandlers;

    std::size_t _watch_count = 0;
};

}  // namespace wolf::gst
