#pragma once

#include "wolf.hpp"

#include "media/gst/internal/w_wrapper.hpp"

#include <gst/gst.h>

#include <stdexcept>
#include <functional>
#include <utility>
#include <vector>

namespace wolf::media::gst {

/**
 * @brief wrapper of GMainLoop, GLib's mainloop/eventloop facility.
 */
class w_mainloop : public w_wrapper<w_mainloop, GMainLoop, void, g_main_loop_unref>
{
public:
    /**
     * @brief create a simple instance of mainloop.
     * @return a w_mainloop instance on success.
     */
    [[nodiscard]] static auto make() -> boost::leaf::result<w_mainloop>
    {
        auto main_loop_raw = g_main_loop_new(nullptr, false);
        if (!main_loop_raw) {
            return W_FAILURE(std::errc::operation_canceled,
                             "mainloop construction error.");
        }

        return w_mainloop(internal::w_raw_tag{}, main_loop_raw);
    }

    /**
     * @brief add a callable to be called on event loop idle times.
     * @param p_func callable to be called on idle times.
     * @return a sourceid to remove it later.
     * @note `p_func` must have a consistent lifetime and address
     *       until either being removed by `idle_remove` or the instance
     *       of this class be destructed.
     */
    template <typename F>
    std::size_t idle_add(F& p_func)
    {
        constexpr auto invoker = +[](F* p_funcptr) { (*p_funcptr)(); };
        return g_idle_add(GSourceFunc(invoker), std::addressof(p_func));
    }

    bool idle_remove(std::size_t p_sourceid)
    {
        if (!p_sourceid) {
            return false;
        }

        return g_source_remove(gsl::narrow_cast<int>(p_sourceid));
    }

    /**
     * @brief run the main/event loop.
     */
    void run()
    {
        g_main_loop_run(raw());
    }

    /**
     * @brief stop the main/event loop.
     */
    void stop()
    {
        g_main_loop_quit(raw());
    }

private:
    w_mainloop(internal::w_raw_tag, GMainLoop* p_main_loop_raw) noexcept
        : w_wrapper(p_main_loop_raw)
    {}
};

}  // namespace wolf::media::gst
