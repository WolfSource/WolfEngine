#pragma once

#include "media/gst/core/w_element.hpp"

namespace wolf::gst {

/**
 * @brief wrappper of wasapisrc gstreamer element.
 */
class w_element_wasapisrc : public w_element
{
    constexpr static const char* factory_name = "wasapisrc";

public:
    [[nodiscard]] static auto make() -> boost::leaf::result<w_element_wasapisrc>
    {
        BOOST_LEAF_AUTO(base_element, w_element::make(factory_name));
        return w_element_wasapisrc(std::move(base_element));
    }

    void set_device(const char* p_device)
    {
        g_object_set(raw(), "device", p_device, nullptr);
    }

    void set_low_latency(bool p_enable)
    {
        g_object_set(raw(), "low-latency", p_enable, nullptr);
    }

private:
    explicit w_element_wasapisrc(w_element&& p_base) noexcept
        : w_element(std::move(p_base))
    {}
};

}  // namespace wolf::gst
