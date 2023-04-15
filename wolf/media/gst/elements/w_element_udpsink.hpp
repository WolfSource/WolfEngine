#pragma once

#include "media/gst/core/w_element.hpp"

namespace wolf::gst {

/**
 * @brief wrappper of udpsink gstreamer element.
 */
class w_element_udpsink : public w_element
{
    constexpr static const char* factory_name = "udpsink";

public:
    [[nodiscard]] static auto make() -> boost::leaf::result<w_element_udpsink>
    {
        BOOST_LEAF_AUTO(base_element, w_element::make(factory_name));
        return w_element_udpsink(std::move(base_element));
    }

    void set_host(const char* p_host)
    {
        g_object_set(raw(), "host", p_host, nullptr);
    }

    void set_port(unsigned short p_port)
    {
        g_object_set(raw(), "port", static_cast<gint>(p_port), nullptr);
    }

private:
    explicit w_element_udpsink(w_element&& p_base) noexcept
        : w_element(std::move(p_base))
    {}
};

}  // namespace wolf::gst
