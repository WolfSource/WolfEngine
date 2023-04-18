#pragma once

#include "wolf.hpp"

#include "media/gst/internal/w_common.hpp"
#include "media/gst/internal/w_wrapper.hpp"
#include "media/gst/core/w_structure.hpp"

#include <gst/gst.h>

#include <string_view>

namespace wolf::media::gst {

/**
 * @brief wrapper of GstCaps, gstreamer capabilities concept.
 */
class w_caps : public w_wrapper<w_caps, GstCaps, void, gst_caps_unref>
{
    friend class internal::w_raw_access;

public:
    /**
     * @brief make an empty caps.
     * @return an instance of w_caps on success.
     */
    [[nodiscard]] static auto make() -> boost::leaf::result<w_caps>
    {
        auto caps_raw = gst_caps_new_empty();
        if (!caps_raw) {
            return W_FAILURE(std::errc::operation_canceled,
                             "couldn't make caps.");
        }

        return w_caps(internal::w_raw_tag{}, caps_raw);
    }

    /**
     * @brief make a caps that acceps any kind of media.
     * @return an instance of w_caps on success.
     */
    [[nodiscard]] static auto make_any() -> boost::leaf::result<w_caps>
    {
        auto caps_raw = gst_caps_new_any();
        if (!caps_raw) {
            return W_FAILURE(std::errc::operation_canceled,
                             "couldn't make caps as any.");
        }

        return w_caps(internal::w_raw_tag{}, caps_raw);
    }

    /**
     * @brief make an empty caps with media name.
     * @return an instance of w_caps on success.
     */
    [[nodiscard]] static auto make_simple(const char* p_media_name)
        -> boost::leaf::result<w_caps>
    {
        auto caps_raw = gst_caps_new_empty_simple(p_media_name);
        if (!caps_raw) {
            return W_FAILURE(
                std::errc::operation_canceled,
                "couldn't make caps with given media name."
            );
        }

        return w_caps(internal::w_raw_tag{}, caps_raw);
    }

    /**
     * @brief add a capability structure.
     * @param p_structure a capability structure.
     */
    void add(w_structure&& p_structure)
    {
        gst_caps_append_structure(raw(), internal::w_raw_access::disown_raw(p_structure));
    }

private:
    w_caps(internal::w_raw_tag, GstCaps* p_caps_raw) noexcept
        : w_wrapper(p_caps_raw)
    {}
};

}  // namespace wolf::media::gst
