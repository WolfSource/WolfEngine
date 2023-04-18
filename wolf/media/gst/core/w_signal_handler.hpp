#pragma once

#include "media/gst/internal/w_common.hpp"

#include <gst/gst.h>

#include <functional>
#include <utility>
#include <stdexcept>

namespace wolf::media::gst {

/**
 * @brief internal signal handler to use in signallable wrappers.
 *
 * it holds the handler and takes ownership of its lifetime,
 * and unhook at destruction.
 *
 * @tparam Args pack of args that will be passed to hooked signal handler.
 */
template <typename ...Args>
class w_signal_handler
{
public:
    using rawptr_type = GObject*;
    using handler_type = std::function<void(Args...)>;
    using handlerid_type = gulong;

    constexpr static auto simple_invoker =
        +[](GObject* /* p_instance */, Args ...p_args, void* p_callee) {
            (*static_cast<handler_type*>(p_callee))(std::move(p_args)...);
        };

    /**
     * @brief make signal handler of given gobject instance.
     * @param p_rawptr gobject instance to hook/unhook signal on.
     */
    explicit w_signal_handler(rawptr_type p_rawptr)
        : _rawptr(p_rawptr)
    {
        // fatal error, this shouldn't be caught but to terminate.
        if (!p_rawptr) {
            throw std::invalid_argument("given raw pointer to handle signals is null.");
        }
    }

    w_signal_handler(const w_signal_handler&) = delete;
    w_signal_handler(w_signal_handler&&) noexcept = default;

    w_signal_handler& operator=(const w_signal_handler&) = delete;
    w_signal_handler& operator=(w_signal_handler&&) noexcept = default;

    ~w_signal_handler() noexcept { unhook(); }

    /**
     * @brief hook given handler on given signal name with given invoker.
     * @param p_name    name of signal to hook.
     * @param p_invoker invoker is raw signal handler
     *                  which will invoke given handler by converted args.
     * @param p_handler user handler to handle named signal.
     * @return boolean indicating successful hook or not.
     * @note invoker does raw convertions too.
     */
    template <std::invocable<Args...> F>
    bool hook(const char* p_name, auto* p_invoker, F&& p_handler)
    {
        unhook();

        auto handlerid = g_signal_connect(
            _rawptr,
            p_name,
            (GCallback)p_invoker,
            (void*)std::addressof(_handler)
        );
        if (!handlerid) {
            return false;
        }

        _handler = std::forward<F>(p_handler);
        _handlerid = handlerid;

        return true;
    }

    /**
     * @brief hook given handler on given signal name with given invoker.
     * @param p_name    name of signal to hook.
     * @param p_handler user handler to handle named signal.
     * @return boolean indicating successful hook or not.
     */
    template <std::invocable<Args...> F>
    bool hook(const char* p_name, F&& p_handler)
    {
        // simple invoker without type conversion.
        return hook(p_name, simple_invoker, std::forward<F>(p_handler));
    }

    /**
     * @brief unhook and release the previously hooked handler.
     * @return boolean indicating success or failure.
     *         returns false if nothing was hooked already.
     */
    bool unhook() noexcept
    {
        if (!_handlerid) {
            return false;
        }

        g_signal_handler_disconnect(_rawptr, _handlerid);
        _handlerid = 0;

        _handler = {};

        return true;
    }

private:
    rawptr_type _rawptr = nullptr;  //< non-onwing.
    handler_type _handler{};
    handlerid_type _handlerid{};
};

}  // namespace wolf::media::gst
