#pragma once

#include "media/gst/core/w_element.hpp"

namespace wolf::media::gst {

/**
 * @brief wrappper of rtmpsink gstreamer element.
 */
class w_element_rtmpsink : public w_element
{
    constexpr static const char* factory_name = "rtmpsink";

public:
    [[nodiscard]] static auto make() -> boost::leaf::result<w_element_rtmpsink>
    {
        BOOST_LEAF_AUTO(base_element, w_element::make(factory_name));
        return w_element_rtmpsink(std::move(base_element));
    }

    void set_location(const char* p_location)
    {
        g_object_set(raw(), "location", p_location, nullptr);
    }

private:
    explicit w_element_rtmpsink(w_element&& p_base) noexcept
        : w_element(std::move(p_base))
    {}
};

}  // namespace wolf::media::gst
