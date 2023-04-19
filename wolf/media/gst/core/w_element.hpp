#pragma once

#include "wolf.hpp"

#include "media/gst/internal/w_common.hpp"
#include "media/gst/internal/w_wrapper.hpp"
#include "media/gst/core/w_pad.hpp"
#include "media/gst/core/w_clock.hpp"

#include <gst/gst.h>

#include <stdexcept>

namespace wolf::media::gst {

/**
 * @brief wrapper of GstElement, gstreamer elements process stream data.
 *
 * elements combine to make dataflow pipeline graphs (DAG)
 * to process one or more media stream(s).
 */
class w_element : public w_wrapper<w_element, GstElement, void, gst_object_unref>
{
    friend class internal::w_raw_access;

public:
    /**
     * @brief make an element by given factory name.
     * @param p_factory_name element's factory name.
     * @return an instance of element of given name on success.
     */
    [[nodiscard]] static auto make(const char* p_factory_name)
        -> boost::leaf::result<w_element>;

    w_element(const w_element&) = delete;
    w_element(w_element&&) noexcept = default;

    w_element& operator=(const w_element&) = delete;
    w_element& operator=(w_element&&) noexcept = default;

    virtual ~w_element() = default;

    /**
     * @brief get a request=pad with given name.
     */
    [[nodiscard]] auto request_pad(const char* p_pad_name)
        -> boost::leaf::result<w_pad>
    {
        auto raw_pad = gst_element_request_pad_simple(raw(), p_pad_name);
        if (!raw_pad) {
            auto err_msg = wolf::format("request for pad `{}` failed.", p_pad_name);
            return W_FAILURE(std::errc::operation_canceled, err_msg);
        }
        return internal::w_raw_access::from_raw<w_pad>(raw_pad);
    }

    /**
     * @brief get an always=pad with given name.
     */
    [[nodiscard]] auto always_pad(const char* p_pad_name)
        -> boost::leaf::result<w_pad>
    {
        auto raw_pad = gst_element_get_static_pad(raw(), p_pad_name);
        if (!raw_pad) {
            auto err_msg = wolf::format("couldn't find always pad: `{}`", p_pad_name);
            return W_FAILURE(std::errc::operation_canceled, err_msg);
        }
        return internal::w_raw_access::from_raw<w_pad>(raw_pad);
    }

    /**
     * @brief get element's clock.
     */
    w_nonowning<w_clock> clock() noexcept
    {
        return internal::w_raw_access::from_raw<w_nonowning<w_clock>>(raw()->clock);
    }

private:
    explicit w_element(internal::w_raw_tag, GstElement* p_element) noexcept
        : w_wrapper(p_element)
    {}
};

}  // namespace wolf::media::gst
